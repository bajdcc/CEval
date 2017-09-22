#include "stdafx.h"
#include "RefStringBase.h"


namespace cc_ref_string_base
{
    Object::Object()
    {
    }

    Object::~Object()
    {
    }

    string Object::toString()
    {
        return "None";
    }

    Integer::Integer(int n): n(n)
    {
    }

    int Integer::value() const
    {
        return n;
    }

    string Integer::toString()
    {
        ostringstream oss;
        oss << n;
        return oss.str();
    }

    String::String(string s): s(s)
    {
    }

    string String::value() const
    {
        return s;
    }

    string String::toString()
    {
        return s;
    }

    cc_exception::cc_exception(string reason): reason(reason)
    {
    }

    cc_exception::cc_exception(const cc_exception& e) : reason(e.reason)
    {
    }

    string cc_exception::toString()
    {
        return reason;
    }
}
