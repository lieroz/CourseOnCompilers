#pragma once

#include <string_view>
#include <set>
#include <stack>
#include <unordered_map>

struct Node
{
    char symbol;
    bool nullable = false;

    std::set<size_t> firstPos;
    std::set<size_t> lastPos;
};

class SyntaxTree
{
    using Tree = std::unordered_map<size_t, Node>;
    using FollowPos = std::unordered_map<size_t, std::set<size_t>>;

public:
    Node generateSyntaxTree(std::string_view regexp);
    const Tree &getSyntaxTree() const;
    const FollowPos &getFollowPos() const;

private:
    void alternate(Node &node);
    void concatenate(Node &node);
    void star(Node &node);

private:
    std::stack<Node> stack;
    Tree syntaxTree;
    FollowPos followPos;
};
