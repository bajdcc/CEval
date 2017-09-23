#pragma once

#include <string>
#include "RefStringBase.h"
#include "RefStringIterator.h"

using namespace std;
using namespace cc_ref_string_base;
using namespace cc_ref_string_iterator;

namespace cc_eval
{
    enum value_t
    {
        v_error,   // 出错
        v_id,      // 标识
        v_double,  // 浮点
        v_int,     // 整型
        v_string,  // 字符串
        v_space,   // 空白字符
        v_end,     // 末尾
    };

    class CEvalLexer
    {
    private:
        shared_ptr<RefString> ref;
        shared_ptr<IRefStringIterator> iter;
    public:
        explicit CEvalLexer(const string& s);

        value_t current() const;
        value_t next();

        double getDouble() const;
        int getInt() const;
        string getString() const;
        string getId() const;

    private:
        value_t next_digit();
        value_t next_alpha();
        value_t next_space();
        value_t next_char();
        value_t next_string();
        value_t next_comment();
        value_t next_operator();

    private:
        value_t type{ v_error };
        double vDouble{ 0.0 };
        int vInt{ 0 };
        string vString;
        string vId;
    };

    /**
     * 计算器类，接收输入，返回结果
     */
    class CEval
    {
    public:
        CEval();
        ~CEval();

        shared_ptr<Object> eval(const string &s);
    };
}