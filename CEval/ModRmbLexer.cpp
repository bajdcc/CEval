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
        switch (type)
        {
        case BEGIN_RECODE:
            if (env.ref)
            {
                throw cc_exception("Duplicated recoding");
            }
            env.ref = make_shared<RefString>(text);
            env.ref->setStart(env.index);
            break;
        case END_RECORD:
            if (env.ref)
            {
                env.ref->setEnd(env.index);
                env.ref->normalize();
                group.push_back(env.ref);
                env.ref.reset();
            }
            break;
        case EXEC_PASS:
            if (env.pass)
            {
                env.pass = false;
                itText->next();
            }
            break;
        case EXIT:
            env.jmp = true;
            env.addr = -1;
            break;
        case EXIT_STEP:
            env.jmp = true;
            env.addr = -1;
            env.exitStep = true;
            break;
        case IF:
            if (env.reg == 1)
            {
                env.jmp = true;
                env.addr = inst->getData();
            }
            break;
        case IS_END:
            env.reg = itText->available() ? 0 : 1;
            break;
        case IS_RECOEDING:
            env.reg = env.ref ? 1 : 0;
            break;
        case JMP:
            env.jmp = true;
            env.addr = inst->getData();
            break;
        case JUMP_STATE:
            env.state = inst->getData();
            break;
        case LOAD:
            {
                auto f = env.scope.find(inst->getData());
                env.reg = f != env.scope.end() ? f->second : 0;
            }
            break;
        case MATCH:
            env.reg = matchers.at(env.reg)->match(env.ch);
            break;
        case MISSING:
            throw cc_exception("Required: " + matchers.at(inst->getData())->toString());
        case MOV:
            env.reg = inst->getData();
            break;
        case NEG:
            env.reg = env.reg == 0 ? 1 : 0;
            break;
        case PANIC:
            env.panic = true;
            break;
        case PASS:
            env.pass = true;
            break;
        case STOP:
            return false;
        case STORE:
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
