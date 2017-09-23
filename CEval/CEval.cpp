#include "stdafx.h"
#include "CEval.h"

// 实现，一开始都是这么简陋，不是吗？

namespace cc_eval
{
    CEvalLexer::CEvalLexer(const string& s)
    {
        ref = make_shared<RefString>(s);
        iter = ref->iterator()->lookAhead();
    }

    value_t CEvalLexer::current() const
    {
        return type;
    }

    value_t CEvalLexer::next()
    {
        // 词法分析参考：https://github.com/bajdcc/CParser/blob/master/CParser/Lexer.cpp
        auto c = iter->current();
        if (!c) // 末尾
        {
            return type = v_end;
        }
        try
        {

            if (isalpha(c) || c == '_') // 变量名或关键字
            {
                type = next_alpha();
            }
            else if (isdigit(c) || c == '-' || c == '.') // 数字
            {
                type = next_digit();
            }
            else if (isspace(c)) // 空白字符
            {
                type = v_space;
            }
            else if (c == '\"') // 字符串
            {
                type = next_string();
            }
            else // 操作符
            {
                type = next_operator();
            }
        }
        catch (cc_exception& e)
        {
            cerr << "Lexer error: " << e.toString() << endl;
            type = v_error;
        }
        return type;
    }

    double CEvalLexer::getDouble() const
    {
        return vDouble;
    }

    int CEvalLexer::getInt() const
    {
        return vInt;
    }

    string CEvalLexer::getString() const
    {
        return vString;
    }

    string CEvalLexer::getId() const
    {
        return vId;
    }

    operator_t CEvalLexer::getOper() const
    {
        return vOper;
    }

    static double calc_exp(double d, int e)
    {
        if (e == 0)
            return d;
        else if (e > 0)
            for (int i = 0; i < e; i++)
                d *= 10;
        else
            for (int i = e; i < 0; i++)
                d /= 10;
        return d;
    }

    value_t CEvalLexer::next_digit()
    {
        // 手动实现atof/atoi，并类型转换
        // ** 测试已通过（在main.cpp）

        // 词法规则不表示了，见main.cpp的测试用例
        // 其他功能：int溢出转double，e科学记数法
        // PS：这可能是最复杂的一个parser了
        //     实现atod，至少需要向前看一字符

        type = v_int; // 假设是整型
        bool neg = iter->current() == '-';
        bool has_exp = false;
        bool neg_exp = false;
        bool has_dot = false;
        if (neg) iter->next();
        int c = iter->current();
        int n = 0, l = 0, e = 0;
        double d = 0.0;
        if (isdigit(c))
            n = iter->current() - '0';
        else if (c == '.')
        {
            type = v_double;
            has_dot = true;
        }
        else
        {
            vOper = op_sub;
            return v_oper;
        }
        for (;;)
        {
            c = iter->ahead();
            if (isdigit(c))
            {
                if (type == v_int)
                {
                    auto _n = n;
                    n *= 10;
                    n += c - '0';
                    if (n < 0)
                    {
                        n = 0;
                        type = v_double;
                        d = double(_n) * 10 + double(c - '0');
                    }
                }
                else if (has_exp)
                {
                    e *= 10;
                    e += c - '0';
                }
                else
                {
                    if (has_dot)
                        l++;
                    d *= 10.0;
                    d += c - '0';
                }
                iter->next();
            }
            else if (c == '.')
            {
                type = v_double;
                has_dot = true;
                iter->next();
            }
            else if (c == 'e' || c == 'E')
            {
                if (type == v_int)
                {
                    d = double(n);
                    type = v_double;
                }
                else
                    d = double(n) + calc_exp(d, -l);
                has_exp = true;
                iter->next();
                if (iter->ahead() == '-')
                {
                    neg_exp = true;
                    iter->next();
                    if (!isdigit(iter->ahead()))
                        throw cc_exception("invalid negative number of exp");
                }
                if (!isdigit(iter->ahead()))
                    throw cc_exception("invalid exp");
            }
            else
            {
                if (type == v_int)
                    vInt = neg ? -n : n;
                else
                {
                    if (has_exp)
                        vDouble = calc_exp(d, neg_exp ? -e : e);
                    else
                        vDouble = double(n) + calc_exp(d, -l);
                    if (neg) vDouble = -vDouble;
                }
                return type;
            }
        }
    }

    value_t CEvalLexer::next_alpha()
    {
        ostringstream oss;
        oss << iter->current();
        for (;;)
        {
            auto c = iter->ahead();
            if (!(isalnum(c) || c == '_'))
                break;
            oss << c;
            iter->next();
        }
        vId = oss.str();
        return v_id;
    }

    value_t CEvalLexer::next_string()
    {
        iter->next();
        ostringstream oss;
        for (;;)
        {
            auto c = iter->current();
            if (!c)
                throw cc_exception("incomplete string");
            if (c == '"')
            {
                vString = oss.str();
                break;
            }
            if (c == '\\')
            {
                iter->next();
                switch (iter->current())
                {
                case 'b':
                    oss << '\b';
                    break;
                case 'f':
                    oss << '\f';
                    break;
                case 'n':
                    oss << '\n';
                    break;
                case 'r':
                    oss << '\r';
                    break;
                case 't':
                    oss << '\t';
                    break;
                case 'v':
                    oss << '\v';
                    break;
                case '\'':
                    oss << '\'';
                    break;
                case '\"':
                    oss << '\"';
                    break;
                case '\\':
                    oss << '\\';
                    break;
                case '0': // skip
                    break;
                case 'x':
                case 'X':
                {
                    auto xx = iter->ahead(); // current = \\x, ahead = X1
                    if (!isxdigit(xx))
                        throw cc_exception("incomplete string escape \\xXX");
                    auto n1 = xx <= '9' ? xx - '0' : ((xx | 0x20) - 'a');
                    iter->next();
                    xx = iter->ahead(); // current = X1, ahead = X2
                    if (isxdigit(xx))
                    {
                        auto n2 = xx <= '9' ? xx - '0' : ((xx | 0x20) - 'a');
                        iter->next();
                        oss << char((n1 << 4) | n2);
                    }
                    else
                    {
                        oss << char(n1);
                    }
                }
                    break;
                default:
                    throw cc_exception("incomplete string escape");
                }
            }
            else
            {
                oss << c;
            }
            iter->next();
        }
        return v_string;
    }

    value_t CEvalLexer::next_operator()
    {
        static map<char, operator_t> mapOper =
        {
            { '=', op_equ },
            { '+', op_add },
            { '-', op_sub },
            { '*', op_mul },
            { '/', op_div },
            { '%', op_mod },
            { '\\', op_esc },
            { '?', op_ask },
            { '%', op_mod },
            { '&', op_and },
            { '|', op_bar },
            { '~', op_til },
            { '^', op_xor },
            { '!', op_exc },
            { '(', op_lpa },
            { ')', op_rpa },
            { '[', op_lsq },
            { ']', op_rsq },
            { '{', op_lbr },
            { '}', op_rbr },
            { ',', op_com },
            { '.', op_dot },
            { ';', op_sem },
            { ':', op_col },
        };
        auto op1 = mapOper.find(iter->current());
        if (op1 != mapOper.end())
        {
            vOper = op1->second;
            return v_oper;
        }
        return v_error;
    }

    CEval::CEval()
    {
    }


    CEval::~CEval()
    {
    }

    shared_ptr<Object> CEval::eval(const string& s)
    {
        return nullptr;
    }
}