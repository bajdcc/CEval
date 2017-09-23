#include "stdafx.h"
#include "RefStringBase.h"
#include "ModRmbParser.h"
#include "CEval.h"

#define TEST_DOUBLE 1

using namespace cc_mod_rmb_parser;
using namespace cc_eval;

int main()
{
    try
    {
        auto input = string("000905000234.0678");
        cout << "Input: " << input << endl;
        auto parser = make_shared<RmbParser>(input);
        parser->parse();
        cout << "Number Style: " << parser->toNumberString() << endl;
        cout << "RMB Style: " << parser->toString() << endl;

#ifdef TEST_DOUBLE
        { CEvalLexer lexer("123"); assert(lexer.next() == v_int); assert(lexer.getInt() == 123); }
        { CEvalLexer lexer("123."); assert(lexer.next() == v_double); assert(lexer.getDouble() == 123.0); }
        { CEvalLexer lexer("123.4"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 123.4); }
        { CEvalLexer lexer("-123"); assert(lexer.next() == v_int); assert(lexer.getInt() == -123); }
        { CEvalLexer lexer("-123."); assert(lexer.next() == v_double); assert(lexer.getDouble() == -123.0); }
        { CEvalLexer lexer("-123.4"); assert(lexer.next() == v_double); assert(lexer.getDouble() == -123.4); }
        { CEvalLexer lexer("123e1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 1230.0); }
        { CEvalLexer lexer("123.e1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 1230.0); }
        { CEvalLexer lexer("123.4e1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 1234.0); }
        { CEvalLexer lexer("-123e1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == -1230.0); }
        { CEvalLexer lexer("-123.e1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == -1230.0); }
        { CEvalLexer lexer("-123.4e1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == -1234.0); }
        { CEvalLexer lexer("123e-1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 12.3); }
        { CEvalLexer lexer("123.e-1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 12.3); }
        { CEvalLexer lexer("123.4e-1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 12.34); }
        { CEvalLexer lexer("-123e-1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == -12.3); }
        { CEvalLexer lexer("-123.e-1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == -12.3); }
        { CEvalLexer lexer("-123.4e-1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == -12.34); }
        { CEvalLexer lexer("12345678987654321"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 12345678987654321.0); }
        { CEvalLexer lexer("-"); assert(lexer.next() == v_error); }
        { CEvalLexer lexer(".1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == .1); }
        { CEvalLexer lexer("-.1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == -.1); }
        { CEvalLexer lexer(".e1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 0.0); }
        { CEvalLexer lexer("-.e-1"); assert(lexer.next() == v_double); assert(lexer.getDouble() == 0.0); }
        { CEvalLexer lexer("-123e"); assert(lexer.next() == v_error); }
        { CEvalLexer lexer("-123e-"); assert(lexer.next() == v_error); }
#endif
    }
    catch (cc_exception& e) {
        cerr << "Error: " << e.toString() << endl;
    }
    return 0;
}

