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

    class Integer : public Object
    {
    private:
        int n{ 0 };
    public:
        explicit Integer(int n);
        int value() const;
        string toString() override;
    };

    class String : public Object
    {
    private:
        string s;
    public:
        explicit String(string s);
        string value() const;
        string toString() override;
    };

    class cc_exception : public Object
    {
    private:
        string reason;

    public:
        explicit cc_exception(string reason);
        cc_exception(const cc_exception& e);

        string toString() override;
    };
}