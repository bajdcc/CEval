#pragma once
#include "RefStringBase.h"
#include "ModRmbLexer.h"
#include "ModRmbStyle.h"

using namespace cc_ref_string_base;
using namespace cc_mod_rmb_lexer;
using namespace cc_mod_rmb_style;

namespace cc_mod_rmb_parser
{
    class ITreeNode;
    class ITreeNodeVisitor;

    class IntegerNode;
    class DotNode;
    class DecimalNode;
    class DecimalAtomNode;
    class IntegerUnitNode;
    class IntegerAtomNode;

    enum NodeDataType
    {
        DATA,
        VALUE,
        LEVEL,
    };

    /**
    * 结点接口
    * @author bajdcc
    */
    class ITreeNode : public Object
    {
    public:
        /**
        * 被访问
        * @param visitor 访问者
        */
        virtual void visit(shared_ptr<ITreeNodeVisitor> visitor) = 0;

        /**
        * 添加孩子
        * @param node 结点
        * @param end 是否添加到最后
        */
        virtual void addNode(shared_ptr<ITreeNode> node, bool end) = 0;

        /**
        * 孩子结点是否为空
        * @return 结点是否为空
        */
        virtual bool isEmptyNodes() const = 0;

        /**
        * 孩子结点数
        * @return 结点数
        */
        virtual int size() const = 0;

        /**
        * 设置数据
        * @param key 键
        * @param value  值
        */
        virtual void set(NodeDataType key, shared_ptr<Object> value) = 0;

        /**
        * 取得数据
        * @param key 键
        * @return 数据
        */
        virtual shared_ptr<Object> get(NodeDataType key) const = 0;
    };

    /**
    * 遍历参数
    * @author bajdcc
    */
    class VisitBag
    {
    public:
        bool visitChilren{true};
        bool visitEnd{true};
        bool visitReverse{false};
    };

    /**
    * 遍历结点的访问者
    * @author bajdcc
    */
    class ITreeNodeVisitor : Object
    {
    public:
        /**
        * 遍历开始
        * @param node 整数
        */
        virtual void visitBegin(shared_ptr<IntegerNode> node, VisitBag& bag) = 0;

        /**
        * 遍历开始
        * @param node 小数点
        */
        virtual void visitBegin(shared_ptr<DotNode> node, VisitBag& bag) = 0;

        /**
        * 遍历开始
        * @param node 小数
        */
        virtual void visitBegin(shared_ptr<DecimalNode> node, VisitBag& bag) = 0;

        /**
        * 遍历开始
        * @param node 整数单元
        */
        virtual void visitBegin(shared_ptr<IntegerUnitNode> node, VisitBag& bag) = 0;

        /**
        * 遍历开始
        * @param node 整数原子
        */
        virtual void visitBegin(shared_ptr<IntegerAtomNode> node, VisitBag& bag) = 0;

        /**
        * 遍历开始
        * @param node 小数原子
        */
        virtual void visitBegin(shared_ptr<DecimalAtomNode> node, VisitBag& bag) = 0;

        /////////////////////////////////////////////////

        /**
        * 遍历结束
        * @param node 整数
        */
        virtual void visitEnd(shared_ptr<IntegerNode> node) = 0;

        /**
        * 遍历结束
        * @param node 小数点
        */
        virtual void visitEnd(shared_ptr<DotNode> node) = 0;

        /**
        * 遍历结束
        * @param node 小数
        */
        virtual void visitEnd(shared_ptr<DecimalNode> node) = 0;

        /**
        * 遍历结束
        * @param node 整数单元
        */
        virtual void visitEnd(shared_ptr<IntegerUnitNode> node) = 0;

        /**
        * 遍历结束
        * @param node 整数原子
        */
        virtual void visitEnd(shared_ptr<IntegerAtomNode> node) = 0;

        /**
        * 遍历开始
        * @param node 小数原子
        */
        virtual void visitEnd(shared_ptr<DecimalAtomNode> node) = 0;
    };

    /**
    * 可存储数据的结点
    * @author bajdcc
    */
    class StoreableNode : public ITreeNode
    {
    private:
        vector<shared_ptr<ITreeNode>> nodes;
        map<int, shared_ptr<Object>> stores;

    protected:
        void visitReverse(shared_ptr<ITreeNodeVisitor> visitor);

    public:
        void visit(shared_ptr<ITreeNodeVisitor> visitor) override;
        void addNode(shared_ptr<ITreeNode> node, bool end) override;
        bool isEmptyNodes() const override;
        int size() const override;
        void set(NodeDataType key, shared_ptr<Object> value) override;
        shared_ptr<Object> get(NodeDataType key) const override;
    };

    /**
    * 整数结点
    * @author bajdcc
    */
    class IntegerNode : public StoreableNode, public enable_shared_from_this<IntegerNode>
    {
    public:
        void visit(shared_ptr<ITreeNodeVisitor> visitor) override;
    };

    /**
    * 小数点结点
    * @author bajdcc
    */
    class DotNode : public StoreableNode, public enable_shared_from_this<DotNode>
    {
    public:
        void visit(shared_ptr<ITreeNodeVisitor> visitor) override;
    };

    /**
    * 小数结点
    * @author bajdcc
    */
    class DecimalNode : public StoreableNode, public enable_shared_from_this<DecimalNode>
    {
    public:
        void visit(shared_ptr<ITreeNodeVisitor> visitor) override
        {
            VisitBag bag;
            visitor->visitBegin(shared_from_this(), bag);
            if (bag.visitChilren)
            {
                if (bag.visitReverse)
                {
                    visitReverse(visitor);
                }
                else
                {
                    visit(visitor);
                }
            }
            if (bag.visitEnd)
            {
                visitor->visitEnd(shared_from_this());
            }
        }
    };

    /**
    * 小数原子
    * @author bajdcc
    */
    class DecimalAtomNode : public StoreableNode, public enable_shared_from_this<DecimalAtomNode>
    {
    public:
        void visit(shared_ptr<ITreeNodeVisitor> visitor) override;
    };

    /**
    * 整数单位
    * @author bajdcc
    */
    class IntegerUnitNode : public StoreableNode, public enable_shared_from_this<IntegerUnitNode>
    {
    public:
        void visit(shared_ptr<ITreeNodeVisitor> visitor) override;
    };

    /**
    * 整数原子
    * @author bajdcc
    */
    class IntegerAtomNode : public StoreableNode, public enable_shared_from_this<IntegerAtomNode>
    {
    public:
        void visit(shared_ptr<ITreeNodeVisitor> visitor) override;
    };

    /**
    * 从树结点生成字符串（数字）
    * @author bajdcc
    */
    class TreeNodeToString : public ITreeNodeVisitor
    {
    private:
        ostringstream oss;
    public:
        void visitBegin(shared_ptr<IntegerNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DotNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DecimalNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<IntegerUnitNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<IntegerAtomNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DecimalAtomNode> node, VisitBag& bag) override;
        void visitEnd(shared_ptr<IntegerNode> node) override;
        void visitEnd(shared_ptr<DotNode> node) override;
        void visitEnd(shared_ptr<DecimalNode> node) override;
        void visitEnd(shared_ptr<IntegerUnitNode> node) override;
        void visitEnd(shared_ptr<IntegerAtomNode> node) override;
        void visitEnd(shared_ptr<DecimalAtomNode> node) override;
        string toString() override;
    };

    /**
    * 计算结点层次
    * @author bajdcc
    */
    class RmbTreeLevelVisitor : public ITreeNodeVisitor
    {
    private:
        int unitLevel{0};
        int atomLevel{0};
    public:
        void visitBegin(shared_ptr<IntegerNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DotNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DecimalNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<IntegerUnitNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<IntegerAtomNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DecimalAtomNode> node, VisitBag& bag) override;
        void visitEnd(shared_ptr<IntegerNode> node) override;
        void visitEnd(shared_ptr<DotNode> node) override;
        void visitEnd(shared_ptr<DecimalNode> node) override;
        void visitEnd(shared_ptr<IntegerUnitNode> node) override;
        void visitEnd(shared_ptr<IntegerAtomNode> node) override;
        void visitEnd(shared_ptr<DecimalAtomNode> node) override;
    };

    /**
    * 语法分析器
    * @author bajdcc
    */
    class Parser : public Object
    {
    protected:
        shared_ptr<Lexer> lexer;
        vector<shared_ptr<RefString>> group;
        shared_ptr<ITreeNode> root;

        enum PartType
        {
            INTEGER,
            DOT,
            DECIMAL,
        };

    public:
        Parser(string text, shared_ptr<IStyle> style);

    protected:
        /**
        * 设置默认值
        * @param id 索引
        * @return
        */
        virtual string getDefaultValue(int id) = 0;

        /**
        * 获取迭代器
        * @param type 类型
        * @param text 文本
        * @return
        */
        virtual shared_ptr<IRefStringIterator> iterator(PartType type, shared_ptr<RefString> text) = 0;

        /**
        * 内部解析
        * @param type 类型
        * @param iterator 迭代器
        * @param node 结点
        * @return 是否可以继续
        * @throws Exception
        */
        virtual bool parseInternal(PartType type, shared_ptr<IRefStringIterator> iterator,
                                   shared_ptr<ITreeNode> node) = 0;

    private:
        /**
        * 标准化结果数组
        * @param count 
        */
        void normalizeGroup(int count);

        /**
        * 生成树
        * @return
        */
        shared_ptr<ITreeNode> parse();

        /**
        * 结点创建工厂
        * @param type
        * @return
        */
        static shared_ptr<ITreeNode> createNode(PartType type);

        /**
        * 解析整数部分
        * @param type 类型
        * @param text 文本
        * @return
        * @throws Exception
        */
        shared_ptr<ITreeNode> parseNode(PartType type, shared_ptr<RefString> text);

        string toString() override;
    };

    /**
    * 人民币翻译（结点序列化）
    * @author bajdcc
    */
    class RmbTreeToString : public ITreeNodeVisitor
    {
    private:
        vector<string> CN_ATOMS{"零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖",};
        vector<string> CN_UNITS{"", "拾", "佰", "仟",};
        vector<string> CN_DECIMALS{"", "角", "分",};
        string CN_TEN_THOUSAND{"万"};
        string CN_HUNDRED_MILLION{"亿"};
        string CN_SYMBOL{"人民币"};
        string CN_DOLLAR{"元"};
        string CN_INTEGER{"整"};

        ostringstream oss;

        int unitZero{0};
        int atomZero{0};

    private:
        /**
        * 添加整数原子
        * @param data 数字
        * @param level 等级
        */
        void appendIntegerAtom(int data, int level);

        /**
        * 添加小数原子
        * @param data 数字
        * @param level 等级
        */
        void appendDecimalAtom(int data, int level);

        void appendZero();

    public:
        RmbTreeToString();
        void visitBegin(shared_ptr<IntegerNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DotNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DecimalNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<IntegerUnitNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<IntegerAtomNode> node, VisitBag& bag) override;
        void visitBegin(shared_ptr<DecimalAtomNode> node, VisitBag& bag) override;
        void visitEnd(shared_ptr<IntegerNode> node) override;
        void visitEnd(shared_ptr<DotNode> node) override;
        void visitEnd(shared_ptr<DecimalNode> node) override;
        void visitEnd(shared_ptr<IntegerUnitNode> node) override;
        void visitEnd(shared_ptr<IntegerAtomNode> node) override;
        void visitEnd(shared_ptr<DecimalAtomNode> node) override;
        string toString() override;
    };

    /**
    * 人民币翻译方案
    *
    * @author bajdcc
    */
    class RmbParser : public Parser
    {
    public:
        explicit RmbParser(string text);

    protected:
        string getDefaultValue(int id) override;

        shared_ptr<IRefStringIterator> iterator(PartType type, shared_ptr<RefString> text) override;

        bool parseInternal(PartType type, shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node) override;

    private:
        /**
        * 返回数字
        * @param ch 字符
        * @param message 错误信息
        * @return 数字
        * @throws Exception
        */
        int getIntegerWithCheck(char ch, string message);

        /**
        * 解析小数
        * @param iterator 迭代器
        * @param node 结点
        * @return 是否可以继续
        * @throws Exception 解析错误
        */
        bool parseDecimal(shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node);

        /**
        * 解析小数点
        * @param iterator 迭代器
        * @param node 结点
        * @return 是否可以继续
        * @throws Exception 解析错误
        */
        bool parseDot(shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node);

        /**
        * 解析整数
        * @param iterator 迭代器
        * @param node 结点
        * @return 是否可以继续
        * @throws Exception 解析错误
        */
        bool parseInteger(shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node);

        /**
        * 解析整数单元
        * @param iterator 迭代器
        * @param node 结点
        * @return 是否可以继续
        * @throws Exception 解析错误
        */
        bool parseIntegerUnit(shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node);

    public:
        string toNumberString() const;
        string toString() override;
    };
}
