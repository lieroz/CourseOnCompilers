#include "syntaxtree.h"

#include <sstream>

inline constexpr char regexpEndingSymbol = '#';

void SyntaxTree::create(std::string_view regexp)
{
    syntaxTree.clear();
    followPos.clear();

    size_t nodeId = 0;

    for (const char &c: regexp)
    {
        Node node{c};

        switch (c)
        {
            case '|':
                alternate(node);
                break;
            case '&':
                concatenate(node);
                break;
            case '*':
                star(node);
                break;
            default:
                node.firstPos.insert(nodeId);
                node.lastPos.insert(nodeId);
                stack.push(node);

                if (c != regexpEndingSymbol)
                {
                    alphabet.insert(c);
                }
                break;
        }

        syntaxTree[nodeId++] = node;
    }

    root = stack.top();
    stack.pop();
}

const Node &SyntaxTree::getRoot() const
{
    return root;
}

const SyntaxTree::Tree &SyntaxTree::getSyntaxTree() const
{
    return syntaxTree;
}

const SyntaxTree::FollowPos &SyntaxTree::getFollowPos() const
{
    return followPos;
}

const std::set<char> &SyntaxTree::getAlphabet() const
{
    return alphabet;
}

std::string SyntaxTree::toString() const
{
    const auto &dfaStartState = getRoot();
    std::stringstream ss;

    ss << "=====================\n";
    ss << "SYNTAX TREE\n";
    ss << "=====================\n\n";

    for (const auto &[pos, node]: getSyntaxTree())
    {
        ss << "POS: " << pos;
        ss << "\nSYMBOL: " << node.symbol;
        ss << "\nNULLABLE: " << node.nullable;

        ss << "\nFIRST POS: ";
        std::copy(std::begin(node.firstPos), std::end(node.firstPos),
            std::ostream_iterator<size_t>(ss, " "));

        ss << "\nLAST POS: ";
        std::copy(std::begin(node.lastPos), std::end(node.lastPos),
            std::ostream_iterator<size_t>(ss, " "));
        ss << "\n\n";
    }

    ss << "=====================\n";
    ss << "FOLLOW POSITIONS\n";
    ss << "=====================\n\n";

    for (const auto &[pos, followPos]: getFollowPos())
    {
        ss << "POS: " << pos;
        ss << "\nFOLLOW POS: ";
        std::copy(
            std::begin(followPos), std::end(followPos), std::ostream_iterator<size_t>(ss, " "));
        ss << "\n\n";
    }

    ss << "=====================\n";
    ss << "ROOT\n";
    ss << "=====================\n\n";

    ss << "SYMBOL: " << dfaStartState.symbol;
    ss << "\nNULLABLE: " << dfaStartState.nullable;

    ss << "\nFIRST POS: ";
    std::copy(std::begin(dfaStartState.firstPos), std::end(dfaStartState.firstPos),
        std::ostream_iterator<size_t>(ss, " "));

    ss << "\nLAST POS: ";
    std::copy(std::begin(dfaStartState.lastPos), std::end(dfaStartState.lastPos),
        std::ostream_iterator<size_t>(ss, " "));

    ss << "\n\n<<<<<<<<<<<<<<<<<<<<<\n";
    return ss.str();
}

void SyntaxTree::alternate(Node &node)
{
    auto c2 = stack.top();
    stack.pop();

    auto c1 = stack.top();
    stack.pop();

    node.nullable = c1.nullable || c2.nullable;

    std::merge(std::begin(c1.firstPos), std::end(c1.firstPos), std::begin(c2.firstPos),
        std::end(c2.firstPos), std::inserter(node.firstPos, std::end(node.firstPos)));
    std::merge(std::begin(c1.lastPos), std::end(c1.lastPos), std::begin(c2.lastPos),
        std::end(c2.lastPos), std::inserter(node.lastPos, std::end(node.lastPos)));

    stack.push(node);
}

void SyntaxTree::concatenate(Node &node)
{
    auto c2 = stack.top();
    stack.pop();

    auto c1 = stack.top();
    stack.pop();

    node.nullable = (c1.nullable && c2.nullable);

    if (c1.nullable)
    {
        std::merge(std::begin(c1.firstPos), std::end(c1.firstPos), std::begin(c2.firstPos),
            std::end(c2.firstPos), std::inserter(node.firstPos, std::end(node.firstPos)));
    }
    else
    {
        node.firstPos.insert(std::begin(c1.firstPos), std::end(c1.firstPos));
    }

    if (c2.nullable)
    {
        std::merge(std::begin(c1.lastPos), std::end(c1.lastPos), std::begin(c2.lastPos),
            std::end(c2.lastPos), std::inserter(node.lastPos, std::end(node.lastPos)));
    }
    else
    {
        node.lastPos.insert(std::begin(c2.lastPos), std::end(c2.lastPos));
    }

    stack.push(node);

    for (const auto &i: c1.lastPos)
    {
        followPos[i].insert(std::begin(c2.firstPos), std::end(c2.firstPos));
    }
}

void SyntaxTree::star(Node &node)
{
    auto c1 = stack.top();
    stack.pop();

    node.nullable = true;

    node.firstPos.insert(std::begin(c1.firstPos), std::end(c1.firstPos));
    node.lastPos.insert(std::begin(c1.lastPos), std::end(c1.lastPos));

    stack.push(node);

    for (const auto &i: c1.lastPos)
    {
        followPos[i].insert(std::begin(c1.firstPos), std::end(c1.firstPos));
    }
}
