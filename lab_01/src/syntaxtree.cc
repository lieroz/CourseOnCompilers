#include "syntaxtree.h"

Node SyntaxTree::generateSyntaxTree(std::string_view regexp)
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
                break;
        }

        syntaxTree[nodeId++] = node;
    }

    auto tmp = stack.top();
    stack.pop();
    return tmp;
}

const SyntaxTree::Tree &SyntaxTree::getSyntaxTree() const
{
    return syntaxTree;
}

const SyntaxTree::FollowPos &SyntaxTree::getFollowPos() const
{
    return followPos;
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
