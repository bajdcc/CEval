#pragma once

#include <string>

using namespace std;

namespace cc_ref_string_base
{
    class Object
    {
    public:
        Object();
        virtual ~Object();

        virtual string toString();
    };
}