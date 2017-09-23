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
        v_oper,    // 操作符
        v_end,     // 末尾
    };

    // 引用自CParser中的操作符类型
    enum operator_t
    {
        op_equ,
        op_add,
        op_sub,
        op_mul,
        op_div,
        op_esc,
        op_ask,
        op_mod,
        op_and,
        op_bar,
        op_til,
        op_xor,
        op_exc,
        op_lpa,
        op_rpa,
        op_lsq,
        op_rsq,
        op_lbr,
        op_rbr,
        op_com,
        op_dot,
        op_sem,
        op_col,
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
        operator_t getOper() const;

    private:
        value_t next_digit();
        value_t next_alpha();
        value_t next_string();
        value_t next_operator();

    private:
        value_t type{ v_error };
        double vDouble{ 0.0 };
        int vInt{ 0 };
        string vString;
        string vId;
        operator_t vOper;
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