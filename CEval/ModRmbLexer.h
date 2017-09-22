#pragma once

#include <iostream>
#include <string>
#include "RefStringBase.h"
#include "RefStringIterator.h"

using namespace std;
using namespace cc_ref_string_base;
using namespace cc_ref_string_iterator;

namespace cc_mod_rmb_style
{
    class IStyle;
}

namespace cc_mod_rmb_lexer
{
    /**
      * 词法分析的指令
      *
      * @author bajdcc
      */
    enum LexerInstType
    {
        // 基于字符串
        IS_RECORDING = 100,
        IS_END,
        BEGIN_RECORD,
        END_RECORD,
        JUMP_STATE,
        MATCH,
        PASS,
        EXEC_PASS,
        STOP,

        // 基于指令
        IF = 200,
        JMP,
        EXIT,
        EXIT_STEP,
        MOV,
        NEG,
        LOAD,
        STORE,
        PANIC,

        // 基于错误
        MISSING = 300,
    };

    /**
    * 词法分析指令（零地址或一地址）
    * @author bajdcc
    */
    class ILexerInst : public Object
    {
    public:
        /**
        * @return 指令类型
        */
        virtual LexerInstType getType() const = 0;

        /**
        * @return 指令数据
        */
        virtual int getData() const = 0;
    };

    /**
    * 零地址
    * @author bajdcc
    */
    class ZeroInst : public ILexerInst
    {
    private:
        LexerInstType type;

    public:
        ZeroInst(LexerInstType type);

        LexerInstType getType() const override;
        int getData() const override;

        string toString() override;
    };

    /**
    * 一地址
    * @author bajdcc
    */
    class OneInst : public ZeroInst
    {
    private:
        int data;

    public:
        OneInst(LexerInstType type, int data);

        int getData() const override;

        string toString() override;
    };

    class InstFactory
    {
    public:
        /**
        * 创建零地址指令
        * @param type
        * @return
        */
        static shared_ptr<ILexerInst> createInst(LexerInstType type);

        /**
        * 创建一地址指令
        * @param type
        * @param data
        * @return
        */
        static shared_ptr<ILexerInst> createInst(LexerInstType type, int data);
    };

    /**
    * 词法分析的步骤类型
    * @author bajdcc
    */
    enum LexerStepType
    {
        BEGIN,
        CURRENT,
        NEXT,
        END,
    };

    /**
    * 词法分析的步骤
    * @author bajdcc
    */
    class ILexerStep : Object
    {
    public:
        /**
        * 设置步骤的指令
        * @param type 步骤类型
        * @param inst 指令集
        */
        virtual void setStep(LexerStepType type, vector<shared_ptr<ILexerInst>> inst) = 0;

        /**
        * 返回步骤的指令
        * @param type 第几个步骤
        * @return 指令集
        */
        virtual vector<shared_ptr<ILexerInst>> getStep(LexerStepType type) const = 0;

        /**
        * 判断步骤的指令是否存在
        * @param type 第几个步骤
        * @return 指令集
        */
        virtual bool hasStep(LexerStepType type) const = 0;
    };

    /**
    * 词法分析的步骤实现
    * @author bajdcc
    */
    class LexerStep : public ILexerStep
    {
    private:
        map<LexerStepType, vector<shared_ptr<ILexerInst>>> steps;

    public:
        void setStep(LexerStepType type, vector<shared_ptr<ILexerInst>> inst) override;
        vector<shared_ptr<ILexerInst>> getStep(LexerStepType type) const override;
        bool hasStep(LexerStepType type) const override;
    };


    /**
    * 词法分析器的状态机环境
    * @author bajdcc
    */
    class Env
    {
    public:
        /**
        * 当前状态
        */
        int state{0};

        /**
        * 当前字符串位置
        */
        int index{0};

        /**
        * 当前指令位置
        */
        int addr{0};

        /**
        * 当前寄存器
        */
        int reg{-1};

        /**
        * 当前字符
        */
        char ch{0};

        /**
        * 当前步骤
        */
        shared_ptr<ILexerStep> step;

        /**
        * 当前指令
        */
        shared_ptr<ILexerInst> inst;

        /**
        * 当前捕获字串
        */
        shared_ptr<RefString> ref;

        /**
        * 是否刚执行过跳转指令
        */
        bool jmp{false};

        /**
        * 是否需要执行PASS指令
        */
        bool pass{false};

        /**
        * 是否需要退出步骤
        */
        bool exitStep{false};

        /**
        * 是否出现错误
        */
        bool panic{false};

        /**
        * 全局存储空间
        */
        map<int, int> scope;
    };

    using namespace cc_mod_rmb_style;

    /**
    * RMB词法分析器
    *
    * @author bajdcc
    */
    class Lexer : Object
    {
    protected:
        string text;
        shared_ptr<IRefStringIterator> itText;
        vector<shared_ptr<RefString>> group;
        shared_ptr<IStyle> style;
        vector<shared_ptr<ILexerMatcher>> matchers;

    public:
        Lexer(string text, shared_ptr<IStyle> style);

        string getText() const;
        shared_ptr<IStyle> getStyle() const;
        vector<shared_ptr<RefString>> getGroup() const;

        bool match();

    private:
        /**
        * 运行步骤
        *
        * @param env
        *            环境
        * @return 是否终止
        * @throws OperationNotSupportedException
        */
        bool runStep(Env& env);

        /**
        * 运行指令
        *
        * @param env
        *            环境
        * @throws Exception
        */
        bool runInst(Env& env);

        /**
        * 切换当前环境
        *
        * @param type
        *            步骤类型
        * @param env
        *            环境
        */
        void swapEnvironment(LexerStepType type, Env& env);

    public:
        string toString() override;
    };

    /**
    * RMB词法分析
    * @author bajdcc
    */
    class RmbLexer : public Lexer
    {
    public:
        RmbLexer(string string, shared_ptr<IStyle> style);

    private:
        static shared_ptr<IRefStringIterator> decorator(string text);
    };
}
