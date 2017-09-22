#include "stdafx.h"
#include "ModRmbLexer.h"
#include "ModRmbStyle.h"


namespace cc_mod_rmb_lexer
{
    ZeroInst::ZeroInst(LexerInstType type): type(type)
    {
    }

    LexerInstType ZeroInst::getType() const
    {
        return type;
    }

    int ZeroInst::getData() const
    {
        return -1;
    }

    string ZeroInst::toString()
    {
        ostringstream oss;
        oss << type;
        return oss.str();
    }

    OneInst::OneInst(LexerInstType type, int data): ZeroInst(type), data(data)
    {
    }

    int OneInst::getData() const
    {
        return data;
    }

    string OneInst::toString()
    {
        ostringstream oss;
        oss << ZeroInst::toString() << ' ' << data;
        return oss.str();
    }

    shared_ptr<ILexerInst> InstFactory::createInst(LexerInstType type)
    {
        return dynamic_pointer_cast<ILexerInst>(make_shared<ZeroInst>(type));
    }

    shared_ptr<ILexerInst> InstFactory::createInst(LexerInstType type, int data)
    {
        return dynamic_pointer_cast<ILexerInst>(make_shared<OneInst>(type, data));
    }

    void LexerStep::setStep(LexerStepType type, vector<shared_ptr<ILexerInst>> inst)
    {
        steps.insert(make_pair(type, inst));
    }

    vector<shared_ptr<ILexerInst>> LexerStep::getStep(LexerStepType type) const
    {
        return steps.at(type);
    }

    bool LexerStep::hasStep(LexerStepType type) const
    {
        return steps.find(type) != steps.end();
    }

    Lexer::Lexer(string text, shared_ptr<IStyle> style): text(text), style(style)
    {
        auto factory = MatcherFactory::singleton();
        matchers.push_back(factory->createMatcher(MatcherFactory::LETTER));
        matchers.push_back(factory->createMatcher(MatcherFactory::NUMBER));
        matchers.push_back(factory->createMatcher(MatcherFactory::WORD));
    }

    string Lexer::getText() const
    {
        return text;
    }

    shared_ptr<IStyle> Lexer::getStyle() const
    {
        return style;
    }

    vector<shared_ptr<RefString>> Lexer::getGroup() const
    {
        return group;
    }

    bool Lexer::match()
    {
        Env env;
        try
        {
            while (env.state != -1)
            {
                // 按照当前状态，从steps里面取出相应的指令，并且运行指令
                // 当且仅当指令为stop时，退出

                // 运行步骤
                if (!runStep(env))
                {
                    break;
                }
            }
        }
        catch (cc_exception& cc)
        {
            cerr << cc.toString() << endl;
            return false;
        }
        return true;
    }

    bool Lexer::runStep(Env& env)
    {
        for (int i = BEGIN; i <= END; i++)
        {
            LexerStepType type = LexerStepType(i);
            if (env.exitStep)
            {
                break;
            }
            swapEnvironment(type, env);
            if (env.step->hasStep(type))
            {
                auto insts = env.step->getStep(type);
                for (env.addr = 0; env.addr != -1 && env.addr < int(insts.size());)
                {
                    env.inst = insts[env.addr];
                    if (!runInst(env))
                    {
                        return false;
                    }
                    if (!env.jmp)
                    {
                        env.addr++;
                    }
                    else
                    {
                        env.jmp = false;
                    }
                }
            }
        }
        if (env.pass)
        {
            env.pass = false;
            itText->next();
        }
        if (env.exitStep)
        {
            env.exitStep = false;
        }
        return !env.panic;
    }

    bool Lexer::runInst(Env& env)
    {
        auto inst = env.inst;
        auto type = inst->getType();
        // 这里是匹配中精华的部分，指令的实现
        switch (type)
        {
        case BEGIN_RECORD: // 匹配开始，保存起始位置
            if (env.ref)
            {
                throw cc_exception("Duplicated recoding");
            }
            env.ref = make_shared<RefString>(text);
            env.ref->setStart(env.index);
            break;
        case END_RECORD: // 匹配结束，保存结束位置
            if (env.ref)
            {
                env.ref->setEnd(env.index);
                env.ref->normalize(); // 处理从右往左的情况
                group.push_back(env.ref); // 保存匹配的内容
                env.ref.reset(); // 重置，等待下次匹配
            }
            break;
        case EXEC_PASS: // 通过一个字符，遍历指针+1
            if (env.pass)
            {
                env.pass = false;
                itText->next(); // 下一个！
            }
            break;
        case EXIT: // 返回，不执行本阶段后面的指令
            env.jmp = true;
            env.addr = -1;
            break;
        case EXIT_STEP: // 返回并跳过当前阶段，在Jump_State后使用
            env.jmp = true;
            env.addr = -1;
            env.exitStep = true;
            break;
        case IF: // 判断reg是否是1，是1则跳转
            if (env.reg == 1)
            {
                env.jmp = true;
                env.addr = inst->getData();
            }
            break;
        case IS_END: // 是否到流末尾
            env.reg = itText->available() ? 0 : 1;
            break;
        case IS_RECORDING: // 是否在记录
            env.reg = env.ref ? 1 : 0;
            break;
        case JMP: // 跳转
            env.jmp = true;
            env.addr = inst->getData();
            break;
        case JUMP_STATE: // 状态转移
            env.state = inst->getData();
            break;
        case LOAD: // 从字典中读取数据，键为指令参数
            {
                auto f = env.scope.find(inst->getData());
                env.reg = f != env.scope.end() ? f->second : 0;
            }
            break;
        case MATCH: // 匹配当前字符，匹配编号在reg中，意味着必须跟在Mov指令后面
            env.reg = matchers.at(env.reg)->match(env.ch);
            break;
        case MISSING: // 报错指令
            throw cc_exception("Required: " + matchers.at(inst->getData())->toString());
        case MOV: // 设置reg
            env.reg = inst->getData();
            break;
        case NEG: // 取反
            env.reg = env.reg == 0 ? 1 : 0;
            break;
        case PANIC: // gg
            env.panic = true;
            break;
        case PASS: // 通过，使得可以next
            env.pass = true;
            break;
        case STOP:
            return false;
        case STORE: // 保存reg到字典中，键为指令参数
            env.scope.insert(make_pair(inst->getData(), env.reg));
            break;
        default:
            throw cc_exception("Invalid inst");
        }
        return true;
    }

    void Lexer::swapEnvironment(LexerStepType type, Env& env)
    {
        // 取出当前步骤
        auto steps = style->getLexerStep();
        switch (type)
        {
        case BEGIN:
            env.ch = 0;
            env.step = steps.at(env.state);
            break;
        case CURRENT:
            // 准备好当前字符
            env.ch = itText->current();
            env.index = itText->index();
            break;
        case END:
            break;
        case NEXT:
            // 准备好下一字符
            env.ch = itText->ahead();
            env.index++;
            break;
        default:
            break;
        }
    }

    string Lexer::toString()
    {
        ostringstream oss;
        oss << "Lexer, " << text;
        return oss.str();
    }

    RmbLexer::RmbLexer(string string, shared_ptr<IStyle> style): Lexer(string, style)
    {
        itText = decorator(text);
        matchers.push_back(make_shared<CharacterMatcher>('.'));
    }

    shared_ptr<IRefStringIterator> RmbLexer::decorator(string text)
    {
        return make_shared<RefString>(text)
            ->iterator()
            ->skipHead('0')
            ->findFirst('.')
            ->take(2)
            ->lookAhead();
    }
}
