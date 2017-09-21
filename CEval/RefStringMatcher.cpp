#include "stdafx.h"
#include "RefStringMatcher.h"


namespace cc_ref_string_matcher
{
    shared_ptr<LetterMatcher> LetterMatcher::one;
    shared_ptr<NumberMatcher> NumberMatcher::one;
    shared_ptr<WordMatcher> WordMatcher::one;
    shared_ptr<NoneMatcher> NoneMatcher::one;
    shared_ptr<IMatcherFactory> MatcherFactory::one;

    CharacterMatcher::CharacterMatcher(char character): character(character)
    {
    }

    int CharacterMatcher::match(char ch)
    {
        return character == ch ? 1 : 0;
    }

    int NoneMatcher::match(char ch)
    {
        return ch == 0 ? 1 : 0;
    }

    shared_ptr<NoneMatcher> NoneMatcher::singleton()
    {
        if (!one)
        {
            one = make_shared<NoneMatcher>();
        }
        return one;
    }

    string NoneMatcher::toString()
    {
        return "End of text";
    }

    shared_ptr<ILexerMatcher> MatcherFactory::createMatcher(LexerMatcherType type)
    {
        switch (type)
        {
        case LETTER:
            return LetterMatcher::singleton();
        case NUMBER:
            return NumberMatcher::singleton();
        case WORD:
            return WordMatcher::singleton();
        default:
            return nullptr;
        }
    }

    shared_ptr<IMatcherFactory> MatcherFactory::singleton()
    {
        if (!one)
        {
            one = make_shared<MatcherFactory>();
        }
        return one;
    }

    string CharacterMatcher::toString()
    {
        ostringstream oss;
        oss << "Character '" << character << "'";
        return oss.str();
    }

    int LetterMatcher::match(char ch)
    {
        return isalpha(ch) ? 1 : 0;
    }

    shared_ptr<LetterMatcher> LetterMatcher::singleton()
    {
        if (!one)
        {
            one = make_shared<LetterMatcher>();
        }
        return one;
    }

    string LetterMatcher::toString()
    {
        return "Letter";
    }

    int NumberMatcher::match(char ch)
    {
        return isdigit(ch) ? 1 : 0;
    }

    shared_ptr<NumberMatcher> NumberMatcher::singleton()
    {
        if (!one)
        {
            one = make_shared<NumberMatcher>();
        }
        return one;
    }

    string NumberMatcher::toString()
    {
        return "Number";
    }

    int WordMatcher::match(char ch)
    {
        return isalnum(ch) ? 1 : 0;
    }

    shared_ptr<WordMatcher> WordMatcher::singleton()
    {
        if (!one)
        {
            one = make_shared<WordMatcher>();
        }
        return one;
    }

    string WordMatcher::toString()
    {
        return "Letter or Digit";
    }
}
