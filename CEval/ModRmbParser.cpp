#include "stdafx.h"
#include "ModRmbParser.h"


namespace cc_mod_rmb_parser
{
    void StoreableNode::visitReverse(shared_ptr<ITreeNodeVisitor> visitor)
    {
        for (int i = nodes.size() - 1; i >= 0; i--)
        {
            nodes.at(i)->visit(visitor);
        }
    }

    void StoreableNode::visit(shared_ptr<ITreeNodeVisitor> visitor)
    {
        for (auto& node : nodes)
        {
            node->visit(visitor);
        }
    }

    void StoreableNode::addNode(shared_ptr<ITreeNode> node, bool end)
    {
        if (end)
        {
            nodes.push_back(node);
        }
        else
        {
            nodes.insert(nodes.begin(), node);
        }
    }

    bool StoreableNode::isEmptyNodes() const
    {
        return nodes.empty();
    }

    int StoreableNode::size() const
    {
        return nodes.size();
    }

    void StoreableNode::set(NodeDataType key, shared_ptr<Object> value)
    {
        stores.insert(make_pair(key, value));
    }

    shared_ptr<Object> StoreableNode::get(NodeDataType key) const
    {
        return stores.at(key);
    }

    void IntegerNode::visit(shared_ptr<ITreeNodeVisitor> visitor)
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

    void DotNode::visit(shared_ptr<ITreeNodeVisitor> visitor)
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

    void DecimalAtomNode::visit(shared_ptr<ITreeNodeVisitor> visitor)
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

    void IntegerUnitNode::visit(shared_ptr<ITreeNodeVisitor> visitor)
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

    void IntegerAtomNode::visit(shared_ptr<ITreeNodeVisitor> visitor)
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

    void TreeNodeToString::visitBegin(shared_ptr<IntegerNode> node, VisitBag& bag)
    {
    }

    void TreeNodeToString::visitBegin(shared_ptr<DotNode> node, VisitBag& bag)
    {
        oss << node->get(DATA)->toString();
    }

    void TreeNodeToString::visitBegin(shared_ptr<DecimalNode> node, VisitBag& bag)
    {
    }

    void TreeNodeToString::visitBegin(shared_ptr<IntegerUnitNode> node, VisitBag& bag)
    {
        oss << node->get(DATA)->toString();
    }

    void TreeNodeToString::visitBegin(shared_ptr<IntegerAtomNode> node, VisitBag& bag)
    {
        oss << node->get(DATA)->toString();
    }

    void TreeNodeToString::visitBegin(shared_ptr<DecimalAtomNode> node, VisitBag& bag)
    {
    }

    void TreeNodeToString::visitEnd(shared_ptr<IntegerNode> node)
    {
    }

    void TreeNodeToString::visitEnd(shared_ptr<DotNode> node)
    {
    }

    void TreeNodeToString::visitEnd(shared_ptr<DecimalNode> node)
    {
    }

    void TreeNodeToString::visitEnd(shared_ptr<IntegerUnitNode> node)
    {
    }

    void TreeNodeToString::visitEnd(shared_ptr<IntegerAtomNode> node)
    {
    }

    void TreeNodeToString::visitEnd(shared_ptr<DecimalAtomNode> node)
    {
    }

    string TreeNodeToString::toString()
    {
        return oss.str();
    }

    void RmbTreeLevelVisitor::visitBegin(shared_ptr<IntegerNode> node, VisitBag& bag)
    {
        bag.visitReverse = true;
    }

    void RmbTreeLevelVisitor::visitBegin(shared_ptr<DotNode> node, VisitBag& bag)
    {
        node->set(LEVEL, make_shared<Integer>(0));
        bag.visitChilren = false;
        bag.visitEnd = false;
    }

    void RmbTreeLevelVisitor::visitBegin(shared_ptr<DecimalNode> node, VisitBag& bag)
    {
        atomLevel = 1;
    }

    void RmbTreeLevelVisitor::visitBegin(shared_ptr<IntegerUnitNode> node, VisitBag& bag)
    {
        node->set(LEVEL, make_shared<Integer>(unitLevel));
        bag.visitReverse = true;
    }

    void RmbTreeLevelVisitor::visitBegin(shared_ptr<IntegerAtomNode> node, VisitBag& bag)
    {
        node->set(LEVEL, make_shared<Integer>(atomLevel));
        bag.visitChilren = false;
    }

    void RmbTreeLevelVisitor::visitBegin(shared_ptr<DecimalAtomNode> node, VisitBag& bag)
    {
        node->set(LEVEL, make_shared<Integer>(atomLevel));
        bag.visitChilren = false;
    }

    void RmbTreeLevelVisitor::visitEnd(shared_ptr<IntegerNode> node)
    {
        unitLevel = 0;
    }

    void RmbTreeLevelVisitor::visitEnd(shared_ptr<DotNode> node)
    {
    }

    void RmbTreeLevelVisitor::visitEnd(shared_ptr<DecimalNode> node)
    {
    }

    void RmbTreeLevelVisitor::visitEnd(shared_ptr<IntegerUnitNode> node)
    {
        atomLevel = 0;
        unitLevel++;
    }

    void RmbTreeLevelVisitor::visitEnd(shared_ptr<IntegerAtomNode> node)
    {
        atomLevel++;
    }

    void RmbTreeLevelVisitor::visitEnd(shared_ptr<DecimalAtomNode> node)
    {
        atomLevel++;
    }

    Parser::Parser(string text, shared_ptr<IStyle> style)
    {
        lexer = style->createLexer(text);
        lexer->match();
        group = lexer->getGroup();
        normalizeGroup(3);
        try
        {
            root = parse();
        }
        catch (cc_exception& e)
        {
            cerr << e.toString() << endl;
        }
    }

    void Parser::normalizeGroup(int count)
    {
        auto results = group;
        for (int i = 0; i < count; i++)
        {
            if (i >= int(results.size()))
            {
                results.push_back(make_shared<RefString>(getDefaultValue(i)));
            }
        }
    }

    shared_ptr<ITreeNode> Parser::parse()
    {
        auto node = make_shared<StoreableNode>();
        node->addNode(parseNode(INTEGER, group.at(INTEGER)), true);
        node->addNode(parseNode(DOT, group.at(DOT)), true);
        node->addNode(parseNode(DECIMAL, group.at(DECIMAL)), true);
        return node;
    }

    shared_ptr<ITreeNode> Parser::createNode(PartType type)
    {
        switch (type)
        {
        case DECIMAL:
            return make_shared<DecimalNode>();
        case DOT:
            return make_shared<DotNode>();
        case INTEGER:
            return make_shared<IntegerNode>();
        default:
            return nullptr;
        }
    }

    shared_ptr<ITreeNode> Parser::parseNode(PartType type, shared_ptr<RefString> text)
    {
        auto node = createNode(type);
        auto it = iterator(type, text);
        while (parseInternal(type, it, node));
        return node;
    }

    string Parser::toString()
    {
        ostringstream oss;
        oss << "Parser, " << lexer->toString();
        return oss.str();
    }

    void RmbTreeToString::appendIntegerAtom(int data, int level)
    {
        oss << CN_ATOMS[data];
        oss << CN_UNITS[level];
    }

    void RmbTreeToString::appendDecimalAtom(int data, int level)
    {
        oss << CN_ATOMS[data];
        oss << CN_DECIMALS[level];
    }

    void RmbTreeToString::appendZero()
    {
        oss << CN_ATOMS[0];
    }

    RmbTreeToString::RmbTreeToString()
    {
        oss << CN_SYMBOL;
    }

    void RmbTreeToString::visitBegin(shared_ptr<IntegerNode> node, VisitBag& bag)
    {
    }

    void RmbTreeToString::visitBegin(shared_ptr<DotNode> node, VisitBag& bag)
    {
        bag.visitChilren = false;
        bag.visitEnd = false;
        oss << CN_DOLLAR;
    }

    void RmbTreeToString::visitBegin(shared_ptr<DecimalNode> node, VisitBag& bag)
    {
        if (node->isEmptyNodes())
        {
            bag.visitChilren = false;
            bag.visitEnd = false;
            oss << CN_INTEGER;
        }
    }

    void RmbTreeToString::visitBegin(shared_ptr<IntegerUnitNode> node, VisitBag& bag)
    {
    }

    void RmbTreeToString::visitBegin(shared_ptr<IntegerAtomNode> node, VisitBag& bag)
    {
        bag.visitChilren = false;
        bag.visitEnd = false;
        auto data = dynamic_pointer_cast<Integer>(node->get(VALUE))->value();
        if (data != 0)
        {
            if (atomZero > 0)
            {
                atomZero = 0;
                appendZero();
            }
            auto level = dynamic_pointer_cast<Integer>(node->get(LEVEL))->value();
            appendIntegerAtom(data, level);
        }
        else
        {
            atomZero++;
        }
    }

    void RmbTreeToString::visitBegin(shared_ptr<DecimalAtomNode> node, VisitBag& bag)
    {
        bag.visitChilren = false;
        bag.visitEnd = false;
        auto data = dynamic_pointer_cast<Integer>(node->get(VALUE))->value();
        if (data != 0)
        {
            auto level = dynamic_pointer_cast<Integer>(node->get(LEVEL))->value();
            appendDecimalAtom(data, level);
        }
    }

    void RmbTreeToString::visitEnd(shared_ptr<IntegerNode> node)
    {
    }

    void RmbTreeToString::visitEnd(shared_ptr<DotNode> node)
    {
    }

    void RmbTreeToString::visitEnd(shared_ptr<DecimalNode> node)
    {
    }

    void RmbTreeToString::visitEnd(shared_ptr<IntegerUnitNode> node)
    {
        if (atomZero == node->size())
        {
            unitZero++;
        }
        auto level = dynamic_pointer_cast<Integer>(node->get(LEVEL))->value();
        if (level != 0)
        {
            bool zero = unitZero > 0;
            if (level % 2 == 0)
            {
                unitZero = 0;
                oss << CN_HUNDRED_MILLION;
            }
            else if (!zero)
            {
                oss << CN_TEN_THOUSAND;
            }
            else
            {
                appendZero();
            }
        }
        atomZero = 0;
    }

    void RmbTreeToString::visitEnd(shared_ptr<IntegerAtomNode> node)
    {
    }

    void RmbTreeToString::visitEnd(shared_ptr<DecimalAtomNode> node)
    {
    }

    string RmbTreeToString::toString()
    {
        return oss.str();
    }

    RmbParser::RmbParser(string text): Parser(text, StyleFactory::singleton()->createStyle(RMB))
    {
        root->visit(make_shared<RmbTreeLevelVisitor>());
    }

    string RmbParser::getDefaultValue(int id)
    {
        switch (id)
        {
        case INTEGER:
            return "0";
        case DOT:
            return "";
        case DECIMAL:
            return "";
        default:
            break;
        }
        throw cc_exception("Invalid part_type id");
    }

    shared_ptr<IRefStringIterator> RmbParser::iterator(PartType type, shared_ptr<RefString> text)
    {
        switch (type)
        {
        case DECIMAL:
            return text->iterator();
        case DOT:
            return text->iterator();
        case INTEGER:
            return text->reverse();
        default:
            return nullptr;
        }
    }

    bool RmbParser::parseInternal(PartType type, shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node)
    {
        switch (type)
        {
        case DECIMAL:
            return parseDecimal(iterator, node);
        case DOT:
            return parseDot(iterator, node);
        case INTEGER:
            return parseInteger(iterator, node);
        default:
            return false;
        }
    }

    int RmbParser::getIntegerWithCheck(char ch, string message)
    {
        if (!isdigit(ch))
        {
            throw cc_exception(message);
        }
        return ch - '0';
    }

    bool RmbParser::parseDecimal(shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node)
    {
        if (iterator->available())
        {
            auto decimal = make_shared<DecimalAtomNode>();
            node->addNode(decimal, true);
            decimal->set(DATA, make_shared<Integer>(iterator->current()));
            decimal->set(VALUE, make_shared<Integer>(getIntegerWithCheck(iterator->current(), "Decimal")));
            iterator->next();
            return iterator->available();
        }
        return false;
    }

    bool RmbParser::parseDot(shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node)
    {
        if (iterator->current() == '.')
        {
            node->set(DATA, make_shared<Integer>(iterator->current()));
            iterator->next();
            return iterator->available();
        }
        throw cc_exception("Dot");
    }

    bool RmbParser::parseInteger(shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node)
    {
        while (parseIntegerUnit(iterator, node));
        return iterator->available();
    }

    bool RmbParser::parseIntegerUnit(shared_ptr<IRefStringIterator> iterator, shared_ptr<ITreeNode> node)
    {
        auto unit = make_shared<IntegerUnitNode>();
        unit->set(DATA, make_shared<String>(" "));
        node->addNode(unit, false);
        for (int i = 0; i < 4 && iterator->available(); i++)
        {
            auto atom = make_shared<IntegerAtomNode>();
            atom->set(DATA, make_shared<Integer>(iterator->current()));
            atom->set(VALUE, make_shared<Integer>(getIntegerWithCheck(iterator->current(), "Integer")));
            iterator->next();
            unit->addNode(atom, false);
        }
        return iterator->available();
    }

    string RmbParser::toNumberString() const
    {
        auto visitor = make_shared<TreeNodeToString>();
        root->visit(visitor);
        return visitor->toString();
    }

    string RmbParser::toString()
    {
        auto visitor = make_shared<RmbTreeToString>();
        root->visit(visitor);
        return visitor->toString();
    }
}
