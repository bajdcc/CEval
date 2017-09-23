#include "stdafx.h"
#include "RefStringBase.h"
#include "ModRmbParser.h"

using namespace cc_mod_rmb_parser;

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
    }
    catch (cc_exception& e) {
        cerr << "Error: " << e.toString() << endl;
    }
    return 0;
}

