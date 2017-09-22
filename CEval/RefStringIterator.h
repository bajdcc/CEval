#pragma once

#include <memory>
#include "RefStringMatcher.h"

using namespace std;
using namespace cc_ref_string_base;
using namespace cc_ref_string_matcher;

namespace cc_ref_string_iterator
{
    class IRefStringIterator;
    class IRefStringFindIterator;
    class RefString;
    class RefStringIterator;
    class RefStringReverseIterator;
    class RefStringIteratorBase;
    class RefStringIteratorDecorator;

    class SkipIterator;
    class MultiSkipIterator;
    class SkipHeadIterator;
    class LookAheadOneIterator;
    class FindFirstIterator;
    class FindFirstTakeIterator;
    class TakeIterator;

    /**
    * 引用字串的迭代器
    * <p><i>不向前看</i></p>
    * @author bajdcc
    */
    class IRefStringIterator : public Object
    {
    public:
        /**
        * @return 当前位置
        */
        virtual int index() = 0;

        /**
        * @return 当前字符
        */
        virtual char current() = 0;

        /**
        * @return 向前看一个字符
        */
        virtual char ahead() = 0;

        /**
        * @return 是否可以继续
        */
        virtual bool available() = 0;

        /**
        * 指针移动至下一字符
        */
        virtual void next() = 0;

        ///////////////////////////////////////////////

        /**
        * 过滤单个字符
        * @param matcher 过滤的字符
        * @return 迭代器
        */
        virtual shared_ptr<IRefStringIterator> skip(char matcher) = 0;

        /**
        * 过滤字符区间
        * @param matcher 匹配器
        * @return 迭代器
        */
        virtual shared_ptr<IRefStringIterator> multiSkip(shared_ptr<ILexerMatcher> matcher) = 0;

        /**
        * 过滤开头连续的单个字符
        * @param matcher 过滤的字符
        * @return 迭代器
        */
        virtual shared_ptr<IRefStringIterator> skipHead(char matcher) = 0;

        /**
        * 提供向前看一个字符的功能
        * @return 迭代器
        */
        virtual shared_ptr<LookAheadOneIterator> lookAhead() = 0;


        /**
        * 定位第一次找到的该字符的位置，从左到右
        * @param matcher 字符
        * @return 查找迭代器
        */
        virtual shared_ptr<IRefStringFindIterator> findFirst(char matcher) = 0;

        /**
        * 指定从流中提取字符的数量
        * @param count 数量
        * @return 迭代器
        */
        virtual shared_ptr<IRefStringIterator> take(int count) = 0;
    };

    /**
    * 基于查找的装饰器
    * @author bajdcc
    */
    class IRefStringFindIterator : public Object
    {
    public:
        /**
        * @return 是否查找成功，确定位置
        */
        virtual bool found() = 0;

        /**
        * @return 包装的迭代器
        */
        virtual shared_ptr<IRefStringIterator> iterator() = 0;

        ///////////////////////////////////////////////

        /**
        * 从查找模式指定的位置开始计数，取指定数量的字符为止
        * @param count 数量
        * @return 迭代器
        */
        virtual shared_ptr<IRefStringIterator> take(int count) = 0;
    };

    /**
    * 迭代器基类，实现装饰方法
    * @author bajdcc
    */
    class RefStringIteratorDecorator : public IRefStringIterator, public enable_shared_from_this<RefStringIteratorDecorator>
    {
    public:
        RefStringIteratorDecorator();

        shared_ptr<IRefStringIterator> skip(char matcher) override;
        shared_ptr<IRefStringIterator> multiSkip(shared_ptr<ILexerMatcher> matcher) override;
        shared_ptr<IRefStringIterator> skipHead(char matcher) override;
        shared_ptr<LookAheadOneIterator> lookAhead() override;
        shared_ptr<IRefStringFindIterator> findFirst(char matcher) override;
        shared_ptr<IRefStringIterator> take(int count) override;
    };

    /**
    * 引用类型的字符串
    * @author bajdcc
    */
    class RefString :public Object,public enable_shared_from_this<RefString>
    {
    private:
        string ref;
        int start, end;

    public:
        RefString(string ref);

        int getStart() const;
        void setStart(int start);
        int getEnd() const;
        void setEnd(int end);
        void normalize();
        char charAt(int index) const;
        int length() const;

        shared_ptr<IRefStringIterator> iterator();
        shared_ptr<IRefStringIterator> reverse();
        string toString() override;
    };

    /**
    * 简易迭代器
    * @author bajdcc
    */
    class RefStringIterator :public RefStringIteratorDecorator
    {
    private:
        int ptr{0};
        shared_ptr<RefString> ref;
        int length;

    public:
        RefStringIterator(shared_ptr<RefString> ref);

        int index() override;
        char current() override;
        char ahead() override;
        bool available() override;
        void next() override;
    };

    /**
    * 反向遍历
    * @author bajdcc
    */
    class RefStringReverseIterator : public RefStringIteratorDecorator
    {
    private :
        int ptr;
        shared_ptr<RefString> ref;

    public:
        RefStringReverseIterator(shared_ptr<RefString> ref): ptr(ref->length() - 1), ref(ref)
        {
        }

        int index()
        {
            return ptr;
        }

        char current()
        {
            return ref->charAt(ptr);
        }

        char ahead()
        {
            return 0;
        }

        bool available()
        {
            return ptr >= 0;
        }

        void next()
        {
            if (ptr >= 0)
            {
                ptr--;
            }
        }
    };

    /**
    * 迭代器基类，拥有外部装饰器
    * @author bajdcc
    */
    class RefStringIteratorBase : public RefStringIteratorDecorator
    {
    protected:
        shared_ptr<IRefStringIterator> iter;

    public:
        RefStringIteratorBase(shared_ptr<IRefStringIterator> iterator);
    };

    /**
    * 过滤器基类
    * @author bajdcc
    */
    class SkipIteratorBase : public RefStringIteratorBase
    {
    protected:
        /**
        * 判断不同
        * @param matcher
        * @return
        */
        virtual bool diff(char matcher) const = 0;

    public:
        SkipIteratorBase(shared_ptr<IRefStringIterator> iterator);

        int index() override;
        char current() override;
        char ahead() override;
        bool available() override;
        void next() override;
    };

    /**
    * 单字符过滤
    *
    * @author bajdcc
    */
    class SkipIterator : public SkipIteratorBase
    {
    private:
        char matcher;

    public:
        SkipIterator(shared_ptr<IRefStringIterator> iterator, char matcher);

    protected:
        bool diff(char ch) const override;
    };

    /**
    * 多区间过滤
    *
    * @author bajdcc
    */
    class MultiSkipIterator : public SkipIteratorBase
    {
    private:
        shared_ptr<ILexerMatcher> matcher;

    public:
        MultiSkipIterator(shared_ptr<IRefStringIterator> iterator, shared_ptr<ILexerMatcher> matcher);

    protected:
        bool diff(char ch) const override;
    };

    /**
    * 单字符过滤
    * <p>
    * <b>只过滤开头的连续字符</b>
    * </p>
    *
    * @author bajdcc
    */
    class SkipHeadIterator : public SkipIteratorBase
    {
    private:
        char matcher;
        bool skip{true};

    public:
        SkipHeadIterator(shared_ptr<IRefStringIterator> iterator, char matcher);

    protected:
        bool diff(char ch) const override;

    public:
        bool available() override;
    };

    /**
    * 字符串的迭代器
    * <p>向前看一位</p>
    * @author bajdcc
    */
    class LookAheadOneIterator : public RefStringIteratorBase
    {
    private:
        char chCurrent;
        char chNext;
        int idx;

    public:
        LookAheadOneIterator(shared_ptr<IRefStringIterator> iterator);

        int index() override;
        char current() override;
        char ahead() override;
        void next() override;
        bool available() override;
    };

    /**
    * 单字符查找条件的迭代器
    * @author bajdcc
    */
    class FindFirstIterator : public RefStringIteratorBase, public IRefStringFindIterator
    {
    private:
        char matcher;
        bool fnd{false};

    public:
        FindFirstIterator(shared_ptr<IRefStringIterator> iterator, char matcher);

    private:
        void check();

        int index() override;
        char current() override;
        char ahead() override;
        bool available() override;
        void next() override;
        bool found() override;

        shared_ptr<IRefStringIterator> iterator() override;
        shared_ptr<IRefStringIterator> take(int count) override;
    };

    /**
    * 从指定位置开始，从流中提取指定数量的字符
    * @author bajdcc
    */
    class FindFirstTakeIterator : public RefStringIteratorBase
    {
    private:
        shared_ptr<IRefStringFindIterator> find;

        int taken{1};
        int count;
        bool start{false};
        bool stop{false};

    public:
        FindFirstTakeIterator(shared_ptr<IRefStringFindIterator> iterator, int count);

    private:
        void check();

    public:
        int index() override;
        char current() override;
        char ahead() override;
        bool available() override;
        void next() override;
    };

    /**
    * 从流中提取指定数量的字符
    * @author bajdcc
    */
    class TakeIterator : public RefStringIteratorBase
    {
    private:
        int taken{1};
        int count;
        bool stop{false};

    public:
        TakeIterator(shared_ptr<IRefStringIterator> iterator, int count);

    private:
        void check();

    public:
        int index() override;
        char ahead() override;
        char current() override;
        bool available() override;
        void next() override;
    };
}
