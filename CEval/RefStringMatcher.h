#pragma once

#include <string>
#include "RefStringBase.h"

using namespace std;
using namespace cc_ref_string_base;

namespace cc_ref_string_matcher
{
    class ILexerMatcher;
    class CharacterMatcher;
    class LetterMatcher;
    class NumberMatcher;
    class WordMatcher;
    class NoneMatcher;

    class IMatcherFactory;
    class MatcherFactory;

    /**
    * 判断当前字符是否处于区间内
    * @author bajdcc
    */
    class ILexerMatcher : public Object
    {
    public:
        /**
        * 匹配函数
        * @param ch 输入的字符
        * @return 是否匹配
        */
        virtual int match(char ch) = 0;
    };

    /**
    * 匹配单个字符
    * @author bajdcc
    */
    class CharacterMatcher : public ILexerMatcher
    {
    private:
        char character;

    public:
        explicit CharacterMatcher(char character);

        int match(char ch) override;

        string toString() override;
    };

    /**
    * 匹配字符
    * @author bajdcc
    */
    class LetterMatcher : public ILexerMatcher
    {
    private:
        static shared_ptr<LetterMatcher> one;

    public:
        int match(char ch) override;

        static shared_ptr<LetterMatcher> singleton();

        string toString() override;
    };

    /**
    * 匹配数字
    * @author bajdcc
    */
    class NumberMatcher : public ILexerMatcher
    {
    private:
        static shared_ptr<NumberMatcher> one;

    public:
        int match(char ch) override;

        static shared_ptr<NumberMatcher> singleton();

        string toString() override;
    };

    /**
    * 匹配字符和数字
    * @author bajdcc
    */
    class WordMatcher : public ILexerMatcher
    {
    private:
        static shared_ptr<WordMatcher> one;

    public:
        int match(char ch) override;

        static shared_ptr<WordMatcher> singleton();

        string toString() override;
    };

    /**
    * 匹配结尾
    * @author bajdcc
    */
    class NoneMatcher : public ILexerMatcher
    {
    private:
        static shared_ptr<NoneMatcher> one;

    public:
        int match(char ch) override;

        static shared_ptr<NoneMatcher> singleton();

        string toString() override;
    };

    class IMatcherFactory : Object
    {
    public:
        enum LexerMatcherType
        {
            LETTER,
            NUMBER,
            WORD,
        };

        /**
        * 创建匹配器
        * @param type 类型
        * @return 匹配器
        */
        virtual shared_ptr<ILexerMatcher> createMatcher(LexerMatcherType type) = 0;
    };

    /**
    * 匹配器构造工厂
    * @author bajdcc
    */
    class MatcherFactory :public IMatcherFactory
    {
    private:
        static shared_ptr<IMatcherFactory> one;

    public:
        shared_ptr<ILexerMatcher> createMatcher(LexerMatcherType type) override;

        static shared_ptr<IMatcherFactory> singleton();
    };
}
