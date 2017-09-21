#include "stdafx.h"
#include "ModRmbLexer.h"


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
        return -1;
    }

    string OneInst::toString()
    {
        ostringstream oss;
        oss << ZeroInst::toString() << ' ' << data;
        return oss.str();
    }

    shared_ptr<ZeroInst> InstFactory::createInst(LexerInstType type)
    {
        return make_shared<ZeroInst>(type);
    }

    shared_ptr<OneInst> InstFactory::createInst(LexerInstType type, int data)
    {
        return make_shared<OneInst>(type, data);
    }

    void LexerStep::setStep(LexerStepType type, vector<shared_ptr<ILexerInst>> inst)
    {
        steps.insert(make_pair(type, inst));
    }

    vector<shared_ptr<ILexerInst>> LexerStep::getStep(LexerStepType type) const
    {
        return steps.at(type);
    }
}
