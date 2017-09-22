#pragma once

#include <string>
#include "RefStringBase.h"
#include "ModRmbLexer.h"

using namespace std;
using namespace cc_ref_string_base;
using namespace cc_mod_rmb_lexer;

namespace cc_mod_rmb_style
{
    /**
    * 翻译方案的通式
    * @author bajdcc
    */
    class IStyle : public Object
    {
    public:
        /**
        * 创建词法分析器
        * @param string 输入
        * @return 词法分析器
        */
        virtual shared_ptr<Lexer> createLexer(string string) = 0;

        /**
        * 返回词法分析的指令步骤
        * @return 指令步骤
        */
        virtual vector<shared_ptr<ILexerStep>> getLexerStep() const = 0;
    };

    enum StyleType
    {
        RMB
    };

    /**
    * 翻译方案的工厂接口
    * @author bajdcc
    */
    class IStyleController : public Object
    {
    public:
        /**
        * 创建实例
        * @param type 实例的类型
        * @return 实例
        */
        virtual shared_ptr<IStyle> createStyle(StyleType type) = 0;
    };

    /**
    * 翻译方案的工厂模式
    * @author bajdcc
    */
    class StyleFactory : public IStyleController
    {
    private:
        static shared_ptr<StyleFactory> one;

    public:
        shared_ptr<IStyle> createStyle(StyleType type) override;

        static shared_ptr<IStyleController> singleton();
    };

    /**
    * RMB翻译方案
    * @author bajdcc
    */
    class RmbStyle : public IStyle, public enable_shared_from_this<RmbStyle>
    {
    private:
        static shared_ptr<RmbStyle> one;

        /**
        * 词法分析的状态机指令
        */
        vector<shared_ptr<ILexerStep>> lexerSteps;

    public:
        RmbStyle();

    private:
        /**
        * 词法分析的状态机指令
        */
        void initLexerSteps();

    public:
        vector<shared_ptr<ILexerStep>> getLexerStep() const override;
        shared_ptr<Lexer> createLexer(string text) override;

        static shared_ptr<IStyle> singleton();
    };
}
