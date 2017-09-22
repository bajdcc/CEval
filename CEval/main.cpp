#include "stdafx.h"
#include "RefStringBase.h"
#include "ModRmbParser.h"


int main()
{
    try
    {
        auto parser = make_shared<cc_mod_rmb_parser::RmbParser>("000905000234.0678");
        parser->parse();
        cout << parser->toNumberString() << endl;
        cout << parser->toString() << endl;
    }
    catch (cc_ref_string_base::cc_exception& e) {
        cerr << "Error: " << e.toString() << endl;
    }
    return 0;
}

