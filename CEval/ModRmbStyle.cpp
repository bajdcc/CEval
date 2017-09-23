#include "stdafx.h"
#include "ModRmbStyle.h"


namespace cc_mod_rmb_style
{
    shared_ptr<StyleFactory> StyleFactory::one;
    shared_ptr<RmbStyle> RmbStyle::one;

    shared_ptr<IStyle> StyleFactory::createStyle(StyleType type)
    {
        switch (type)
        {
        case RMB:
            return RmbStyle::singleton();
        default:
            return nullptr;
        }
    }

    shared_ptr<IStyleController> StyleFactory::singleton()
    {
        if (!one)
        {
            one = make_shared<StyleFactory>();
        }
        return one;
    }

    RmbStyle::RmbStyle()
    {
        initLexerSteps();
    }

    void RmbStyle::initLexerSteps()
    {
        shared_ptr<ILexerStep> step;
        vector<shared_ptr<ILexerInst>> insts;

        // 代码是N年前的了，所以下面的汇编等我想明白再写注释

        /* 四大阶段（BEGIN CURRENT NEXT END）的设计我感觉很不错
         * 因为做的流过滤是向前看一字符的，所以四大阶段也是如此
         * 
         * BEGIN：最开始执行的阶段
         * CURRENT：当前字符的阶段
         * NEXT：向前看一字符的阶段
         * END：最后的阶段
         * 
         * 目前看来，也没有要向前看N>1个字符的必要
         */ 

        /* STATE 0 - Number */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        // State 0 就是匹配数字的状态
        /**
        * If IsRecording   如果正在记录，就返回（合法判定是在Next阶段，即look ahead阶段）
        *   Exit
        * If Match 1       第一次到这个状态（因为没开始记录呢）
        *   Pass           通过（即表示当前字符可以被记录了）
        *   BeginRecord    开始记录！
        * Else
        *   JumpState 2    跳到小数点的状态
        *   ExitStep       状态跳转
        */
        /*[00]*/insts.push_back(InstFactory::createInst(IS_RECORDING));
        /*[01]*/insts.push_back(InstFactory::createInst(IF, -1));
        /*[02]*/insts.push_back(InstFactory::createInst(MOV, 1));
        /*[03]*/insts.push_back(InstFactory::createInst(MATCH));
        /*[04]*/insts.push_back(InstFactory::createInst(IF, 7));
        /*[05]*/insts.push_back(InstFactory::createInst(JUMP_STATE, 2));
        /*[06]*/insts.push_back(InstFactory::createInst(EXIT_STEP));
        /*[07]*/insts.push_back(InstFactory::createInst(PASS));
        /*[08]*/insts.push_back(InstFactory::createInst(BEGIN_RECORD));
        step->setStep(CURRENT, insts);
        insts.clear();
        /**
        * If Match 1        Look ahead阶段，如果是数字
        *   Pass            记录下这个数字
        * Else
        *   EndRecord       不是数字的话就停止记录，保存提取到的数字
        *   If Load 100     如果[100]的数据是1 （当匹配过小数点时，会将[100]置1）
        *     JumpState 4   那就是小数点后的数字匹配完了，结果匹配成功
        *     ExitStep
        *   If Match 3      如果是小数点的话，记录并就跳转到状态1（匹配数字）
        *     Pass
        *     JumpState 1
        *     ExitStep
        *   Else            检测到非法字符，跳转到报错阶段
        *     JumpState 3
        *     ExitStep
        */
        /*[00]*/insts.push_back(InstFactory::createInst(MOV, 1));
        /*[01]*/insts.push_back(InstFactory::createInst(MATCH));
        /*[02]*/insts.push_back(InstFactory::createInst(IF, 13));
        /*[03]*/insts.push_back(InstFactory::createInst(JMP, 15));
        /*[04]*/insts.push_back(InstFactory::createInst(MOV, 3));
        /*[05]*/insts.push_back(InstFactory::createInst(MATCH));
        /*[06]*/insts.push_back(InstFactory::createInst(IF, 10));
        /*[07]*/insts.push_back(InstFactory::createInst(JUMP_STATE, 3));
        /*[08]*/insts.push_back(InstFactory::createInst(PASS));
        /*[09]*/insts.push_back(InstFactory::createInst(EXIT_STEP));
        /*[10]*/insts.push_back(InstFactory::createInst(PASS));
        /*[11]*/insts.push_back(InstFactory::createInst(JUMP_STATE, 1));
        /*[12]*/insts.push_back(InstFactory::createInst(EXIT_STEP));
        /*[13]*/insts.push_back(InstFactory::createInst(PASS));
        /*[14]*/insts.push_back(InstFactory::createInst(EXIT));
        /*[15]*/insts.push_back(InstFactory::createInst(END_RECORD));
        /*[16]*/insts.push_back(InstFactory::createInst(LOAD, 100));
        /*[17]*/insts.push_back(InstFactory::createInst(IF, 19));
        /*[18]*/insts.push_back(InstFactory::createInst(JMP, 4));
        /*[19]*/insts.push_back(InstFactory::createInst(JUMP_STATE, 4));
        /*[20]*/insts.push_back(InstFactory::createInst(PASS));
        /*[21]*/insts.push_back(InstFactory::createInst(EXIT_STEP));
        step->setStep(NEXT, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(EXEC_PASS));    // 取下一字符
        insts.push_back(InstFactory::createInst(IS_END));       // 是否到末尾
        insts.push_back(InstFactory::createInst(IF, 4));        // 如果到末尾
        insts.push_back(InstFactory::createInst(EXIT));         // 就退出
        insts.push_back(InstFactory::createInst(END_RECORD));   // 不然就停止记录并退出
        insts.push_back(InstFactory::createInst(EXIT_STEP));
        step->setStep(END, insts);

        /* STATE 1 - Dot */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        insts.clear();
        insts.push_back(InstFactory::createInst(BEGIN_RECORD)); // 记录下小数点
        step->setStep(CURRENT, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(END_RECORD));
        step->setStep(NEXT, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(PASS));
        insts.push_back(InstFactory::createInst(JUMP_STATE, 0));// 跳转到状态1（检测数字）
        insts.push_back(InstFactory::createInst(MOV, 1));       // 标记[100]为1
        insts.push_back(InstFactory::createInst(STORE, 100));
        insts.push_back(InstFactory::createInst(EXIT_STEP));
        step->setStep(END, insts);

        /* STATE 2 - Missing Number */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        insts.clear();
        insts.push_back(InstFactory::createInst(PANIC));        // 标记状态为Final
        step->setStep(BEGIN, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(MISSING, 1));   // 报错
        step->setStep(CURRENT, insts);

        /* STATE 3 - Missing Dot */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        insts.clear();
        insts.push_back(InstFactory::createInst(PANIC));        // 标记状态为Final
        step->setStep(BEGIN, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(IS_END));
        insts.push_back(InstFactory::createInst(IF, -1));       // 只是整数，没有小数部分
        insts.push_back(InstFactory::createInst(MISSING, 1));   // 报错
        step->setStep(NEXT, insts);

        /* STATE 4 - Match End */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        insts.clear();
        insts.push_back(InstFactory::createInst(PANIC));        // 标记状态为Final
        step->setStep(BEGIN, insts);
    }

    vector<shared_ptr<ILexerStep>> RmbStyle::getLexerStep() const
    {
        return lexerSteps;
    }

    shared_ptr<Lexer> RmbStyle::createLexer(string text)
    {
        return make_shared<RmbLexer>(text, dynamic_pointer_cast<IStyle>(shared_from_this()));
    }

    shared_ptr<IStyle> RmbStyle::singleton()
    {
        if (!one)
        {
            one = make_shared<RmbStyle>();
        }
        return one;
    }
}
