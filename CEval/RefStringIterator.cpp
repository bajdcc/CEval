#include "stdafx.h"
#include "RefStringIterator.h"


namespace cc_ref_string_iterator
{
    RefStringIteratorDecorator::RefStringIteratorDecorator()
    {
    }

    shared_ptr<IRefStringIterator> RefStringIteratorDecorator::skip(char matcher)
    {
        return make_shared<SkipIterator>(lookAhead(), matcher);
    }

    shared_ptr<IRefStringIterator> RefStringIteratorDecorator::multiSkip(shared_ptr<ILexerMatcher> matcher)
    {
        return make_shared<MultiSkipIterator>(lookAhead(), matcher);
    }

    shared_ptr<IRefStringIterator> RefStringIteratorDecorator::skipHead(char matcher)
    {
        return make_shared<SkipHeadIterator>(lookAhead(), matcher);
    }

    shared_ptr<LookAheadOneIterator> RefStringIteratorDecorator::lookAhead()
    {
        return make_shared<LookAheadOneIterator>(shared_from_this());
    }

    shared_ptr<IRefStringFindIterator> RefStringIteratorDecorator::findFirst(char matcher)
    {
        return make_shared<FindFirstIterator>(shared_from_this(), matcher);
    }

    shared_ptr<IRefStringIterator> RefStringIteratorDecorator::take(int count)
    {
        return make_shared<TakeIterator>(shared_from_this(), count);
    }

    RefString::RefString(string ref): ref(ref), start(0), end(ref.length())
    {
    }

    int RefString::getStart() const
    {
        return start;
    }

    void RefString::setStart(int start)
    {
        this->start = start;
    }

    int RefString::getEnd() const
    {
        return end;
    }

    void RefString::setEnd(int end)
    {
        this->end = end;
    }

    void RefString::normalize()
    {
        if (start > end)
        {
            int tmp = start;
            start = end - 1;
            end = tmp + 1;
        }
    }

    char RefString::charAt(int index) const
    {
        if (index < length())
        {
            return ref[start + index];
        }
        return 0;
    }

    int RefString::length() const
    {
        return end - start;
    }

    shared_ptr<IRefStringIterator> RefString::iterator()
    {
        return make_shared<RefStringIterator>(shared_from_this());
    }

    shared_ptr<IRefStringIterator> RefString::reverse()
    {
        return make_shared<RefStringReverseIterator>(shared_from_this());
    }

    string RefString::toString()
    {
        return ref.substr(start, end - start + 1);
    }

    RefStringIterator::RefStringIterator(shared_ptr<RefString> ref): ref(ref), length(ref->length())
    {
    }

    int RefStringIterator::index()
    {
        return ptr;
    }

    char RefStringIterator::current()
    {
        return ref->charAt(ptr);
    }

    char RefStringIterator::ahead()
    {
        return 0;
    }

    bool RefStringIterator::available()
    {
        return ptr < length;
    }

    void RefStringIterator::next()
    {
        if (ptr < length)
        {
            ptr++;
        }
    }

    RefStringIteratorBase::RefStringIteratorBase(shared_ptr<IRefStringIterator> iterator): iter(iterator)
    {
    }

    SkipIteratorBase::SkipIteratorBase(shared_ptr<IRefStringIterator> iterator): RefStringIteratorBase(iterator)
    {
    }

    int SkipIteratorBase::index()
    {
        return available() ? iter->index() : -1;
    }

    char SkipIteratorBase::current()
    {
        return available() ? iter->current() : 0;
    }

    char SkipIteratorBase::ahead()
    {
        return available() ? iter->ahead() : 0;
    }

    bool SkipIteratorBase::available()
    {
        if (!iter->available())
            return false;
        if (!diff(iter->current()))
        {
            for (; iter->available(); iter->next())
            {
                if (diff(iter->ahead()))
                {
                    iter->next();
                    break;
                }
            }
            return iter->available();
        }
        return true;
    }

    void SkipIteratorBase::next()
    {
        if (available())
        {
            iter->next();
        }
    }

    SkipIterator::SkipIterator(shared_ptr<IRefStringIterator> iterator, char matcher): SkipIteratorBase(iterator), matcher(matcher)
    {
    }

    bool SkipIterator::diff(char ch) const
    {
        return ch != 0 && matcher != ch;
    }

    MultiSkipIterator::MultiSkipIterator(shared_ptr<IRefStringIterator> iterator, shared_ptr<ILexerMatcher> matcher): SkipIteratorBase(iterator), matcher(matcher)
    {
    }

    bool MultiSkipIterator::diff(char ch) const
    {
        return ch != 0 && matcher->match(ch) == 0;
    }

    SkipHeadIterator::SkipHeadIterator(shared_ptr<IRefStringIterator> iterator, char matcher): SkipIteratorBase(iterator), matcher(matcher)
    {
    }

    bool SkipHeadIterator::diff(char ch) const
    {
        return ch != 0 && matcher != ch;
    }

    bool SkipHeadIterator::available()
    {
        if (!iter->available())
            return false;
        if (skip)
        {
            if (!diff(matcher))
            {
                for (; iter->available(); iter->next())
                {
                    if (diff(iter->ahead()))
                    {
                        skip = false;
                        iter->next();
                        break;
                    }
                }
                return iter->available();
            }
        }
        return true;
    }

    LookAheadOneIterator::LookAheadOneIterator(shared_ptr<IRefStringIterator> iterator): RefStringIteratorBase(iterator)
    {
        idx = this->iter->index();
        chCurrent = this->iter->current();
        iter->next();
        chNext = this->iter->current();
    }

    int LookAheadOneIterator::index()
    {
        return idx;
    }

    char LookAheadOneIterator::current()
    {
        return chCurrent;
    }

    char LookAheadOneIterator::ahead()
    {
        return chNext;
    }

    void LookAheadOneIterator::next()
    {
        idx = iter->index();
        chCurrent = chNext;
        iter->next();
        chNext = iter->current();
    }

    bool LookAheadOneIterator::available()
    {
        return chCurrent != 0 || chNext != 0;
    }

    FindFirstIterator::FindFirstIterator(shared_ptr<IRefStringIterator> iterator, char matcher): RefStringIteratorBase(iterator), matcher(matcher)
    {
        check();
    }

    void FindFirstIterator::check()
    {
        if (!fnd)
            fnd = iter->current() == matcher;
    }

    int FindFirstIterator::index()
    {
        return iter->index();
    }

    char FindFirstIterator::current()
    {
        return iter->current();
    }

    char FindFirstIterator::ahead()
    {
        return iter->ahead();
    }

    bool FindFirstIterator::available()
    {
        return iter->available();
    }

    void FindFirstIterator::next()
    {
        iter->next();
        check();
    }

    bool FindFirstIterator::found()
    {
        return fnd;
    }

    shared_ptr<IRefStringIterator> FindFirstIterator::iterator()
    {
        return shared_from_this();
    }

    shared_ptr<IRefStringIterator> FindFirstIterator::take(int count)
    {
        return make_shared<FindFirstTakeIterator>(dynamic_pointer_cast<IRefStringFindIterator>(shared_from_this()), count);
    }

    FindFirstTakeIterator::FindFirstTakeIterator(shared_ptr<IRefStringFindIterator> iterator, int count): RefStringIteratorBase(iterator->iterator()), find(iterator), count(count)
    {
        check();
    }

    void FindFirstTakeIterator::check()
    {
        if (stop)
        {
            return;
        }
        if (find->found())
        {
            start = true;
        }
        if (start)
        {
            stop = count < taken;
        }
    }

    int FindFirstTakeIterator::index()
    {
        return stop ? -1 : iter->index();
    }

    char FindFirstTakeIterator::current()
    {
        return stop ? 0 : iter->current();
    }

    char FindFirstTakeIterator::ahead()
    {
        return stop ? 0 : iter->ahead();
    }

    bool FindFirstTakeIterator::available()
    {
        return !stop && iter->available();
    }

    void FindFirstTakeIterator::next()
    {
        if (available())
        {
            if (start)
                taken++;
            check();
            iter->next();
        }
    }

    TakeIterator::TakeIterator(shared_ptr<IRefStringIterator> iterator, int count): RefStringIteratorBase(iterator), count(count)
    {
        check();
    }

    void TakeIterator::check()
    {
        stop = count >= taken;
    }

    int TakeIterator::index()
    {
        return iter->index();
    }

    char TakeIterator::ahead()
    {
        return iter->ahead();
    }

    char TakeIterator::current()
    {
        return stop ? iter->current() : 0;
    }

    bool TakeIterator::available()
    {
        return stop && iter->available();
    }

    void TakeIterator::next()
    {
        if (available())
        {
            taken++;
            check();
            iter->next();
        }
    }
}
