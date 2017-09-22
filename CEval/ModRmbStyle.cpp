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

        /* STATE 0 - Number */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        /**
        * If IsRecording
        *   Exit
        * If Match 1
        *   Pass
        *   BeginRecord
        * Else
        *   JumpState 2
        *   ExitStep
        */
        /*[00]*/insts.push_back(InstFactory::createInst(IS_RECOEDING));
        /*[01]*/insts.push_back(InstFactory::createInst(IF, -1));
        /*[02]*/insts.push_back(InstFactory::createInst(MOV, 1));
        /*[03]*/insts.push_back(InstFactory::createInst(MATCH));
        /*[04]*/insts.push_back(InstFactory::createInst(IF, 7));
        /*[05]*/insts.push_back(InstFactory::createInst(JUMP_STATE, 2));
        /*[06]*/insts.push_back(InstFactory::createInst(EXIT_STEP));
        /*[07]*/insts.push_back(InstFactory::createInst(PASS));
        /*[08]*/insts.push_back(InstFactory::createInst(BEGIN_RECODE));
        step->setStep(CURRENT, insts);
        insts.clear();
        /**
        * If Match 1
        *   Pass
        * Else
        *   EndRecord
        *   If Load 100
        *     JumpState 4
        *     ExitStep
        *   If Match 3
        *     Pass
        *     JumpState 1
        *     ExitStep
        *   Else
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
        insts.push_back(InstFactory::createInst(EXEC_PASS));
        insts.push_back(InstFactory::createInst(IS_END));
        insts.push_back(InstFactory::createInst(IF, 4));
        insts.push_back(InstFactory::createInst(EXIT));
        insts.push_back(InstFactory::createInst(END_RECORD));
        insts.push_back(InstFactory::createInst(EXIT_STEP));
        step->setStep(END, insts);

        /* STATE 1 - Dot */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        insts.clear();
        insts.push_back(InstFactory::createInst(BEGIN_RECODE));
        step->setStep(CURRENT, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(END_RECORD));
        step->setStep(NEXT, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(PASS));
        insts.push_back(InstFactory::createInst(JUMP_STATE, 0));
        insts.push_back(InstFactory::createInst(MOV, 1));
        insts.push_back(InstFactory::createInst(STORE, 100));
        insts.push_back(InstFactory::createInst(EXIT_STEP));
        step->setStep(END, insts);

        /* STATE 2 - Missing Number */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        insts.clear();
        insts.push_back(InstFactory::createInst(PANIC));
        step->setStep(BEGIN, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(MISSING, 1));
        step->setStep(CURRENT, insts);

        /* STATE 3 - Missing Dot */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        insts.clear();
        insts.push_back(InstFactory::createInst(PANIC));
        step->setStep(BEGIN, insts);
        insts.clear();
        insts.push_back(InstFactory::createInst(IS_END));
        insts.push_back(InstFactory::createInst(IF, -1));
        insts.push_back(InstFactory::createInst(MISSING, 1));
        step->setStep(NEXT, insts);

        /* STATE 4 - Match End */
        step = make_shared<LexerStep>();
        lexerSteps.push_back(step);
        insts.clear();
        insts.push_back(InstFactory::createInst(PANIC));
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
