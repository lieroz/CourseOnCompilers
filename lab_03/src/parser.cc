#include "parser.h"

#include <iostream>

/*
 * First add operators which are longer if they may conflict
 * For example: '<', '<>' and '<=' are in conflict, so '<' must go last
 */
static const std::vector<std::string> relationalOperators = {"==", "<>", "<=", ">=", "<", ">"};
static const std::vector<std::string> signs = {"+", "-"};
static const std::vector<std::string> additionOperators = {"+", "-", "or"};
static const std::vector<std::string> multiplicationOperators = {"*", "/", "div", "mod", "and"};
static const std::vector<std::string> identifiers = {"a", "b", "c"};
static const std::vector<std::string> constants = {"const"};

using namespace parser::detail;

ReturnType Block::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Block: " << str << std::endl;

    auto tree = Node{"Block", {}};
    if (str.front() == '{' && str.back() == '}')
    {
        if (auto &&[node, newStr] = ::accept<OperatorsList>(str.substr(1), depth + 1); node)
        {
            if (newStr.front() == '}')
            {
                tree.children.push_back({"{", {}});
                tree.children.push_back(*node);
                tree.children.push_back({"}", {}});
                return {tree, newStr.substr(1)};
            }
        }
    }
    return {std::nullopt, str};
}

ReturnType OperatorsList::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "OperatorsList: " << str << std::endl;

    auto tree = Node{"OperatorsList", {}};
    if (auto &&[node1, newStr1] = ::accept<Operator>(str, depth + 1); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Tail>(newStr1, depth + 2); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }
    return {std::nullopt, str};
}

ReturnType Tail::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Tail: " << str << std::endl;

    auto tree = Node{"Tail", {}};
    if (str.front() == ';')
    {
        if (auto &&[node1, newStr1] = ::accept<Operator>(str.substr(1), depth + 1); node1)
        {
            if (auto &&[node2, newStr2] = ::accept<Tail>(newStr1, depth + 2); node2)
            {
                tree.children.push_back({";", {}});
                tree.children.push_back(*node1);
                tree.children.push_back(*node2);
                return {tree, newStr2};
            }
        }
    }

    tree.children.push_back({"ϵ", {}});
    return {tree, str};
}

ReturnType Operator::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Operator: " << str << std::endl;

    auto tree = Node{"Operator", {}};
    if (auto &&[node1, newStr1] = ::accept<Identifier>(str, depth + 1); node1)
    {
        if (newStr1.front() == '=')
        {
            if (auto &&[node2, newStr2] = ::accept<Expression>(newStr1.substr(1), depth + 2); node2)
            {
                tree.children.push_back(*node1);
                tree.children.push_back({"=", {}});
                tree.children.push_back(*node2);
                return {tree, newStr2};
            }
        }
    }

    if (auto &&[node, newStr] = ::accept<Block>(str, depth + 1); node)
    {
        tree.children.push_back(*node);
        return {tree, newStr};
    }
    return {std::nullopt, str};
}

ReturnType Expression::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Expression: " << str << std::endl;

    auto tree = Node{"Expression", {}};
    if (auto &&[node1, newStr1] = ::accept<SimpleExpression>(str, depth + 1); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Expression1>(newStr1, depth + 2); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }
    return {std::nullopt, str};
}

ReturnType SimpleExpression::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "SimpleExpression: " << str << std::endl;

    auto tree = Node{"SimpleExpression", {}};
    if (auto &&[node1, newStr1] = ::accept<Term>(str, depth + 1); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<SimpleExpression1>(newStr1, depth + 2); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }

    if (auto &&[node1, newStr1] = ::accept<Sign>(str, depth + 1); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Term>(newStr1, depth + 2); node1)
        {
            if (auto &&[node3, newStr3] = ::accept<SimpleExpression1>(newStr2, depth + 3); node1)
            {
                tree.children.push_back(*node1);
                tree.children.push_back(*node2);
                tree.children.push_back(*node3);
                return {tree, newStr3};
            }
        }
    }
    return {std::nullopt, str};
}

ReturnType Term::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Term: " << str << std::endl;

    auto tree = Node{"Term", {}};
    if (auto &&[node1, newStr1] = ::accept<Factor>(str, depth + 1); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Term1>(newStr1, depth + 2); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }
    return {std::nullopt, str};
}

ReturnType Factor::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Factor: " << str << std::endl;

    auto tree = Node{"Factor", {}};
    if (auto &&[node, newStr] = ::accept<Identifier>(str, depth + 1); node)
    {
        tree.children.push_back(*node);
        return {tree, newStr};
    }
    if (auto &&[node, newStr] = ::accept<Constant>(str, depth + 1); node)
    {
        tree.children.push_back(*node);
        return {tree, newStr};
    }
    if (str.front() == '(')
    {
        if (auto &&[node, newStr] = ::accept<SimpleExpression>(str.substr(1), depth + 1); node)
        {
            if (newStr.front() == ')')
            {
                tree.children.push_back({"(", {}});
                tree.children.push_back(*node);
                tree.children.push_back({")", {}});
                return {tree, newStr.substr(1)};
            }
        }
    }
    if (str.find("not") == 0)
    {
        if (auto &&[node, newStr] = ::accept<Factor>(str.substr(3), depth + 1); node)
        {
            tree.children.push_back(*node);
            return {tree, newStr};
        }
    }

    return {std::nullopt, str};
}

ReturnType RelationOperation::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "RelationOperation: " << str << std::endl;

    auto tree = Node{"RelationOperation", {}};
    for (auto &&op: relationalOperators)
    {
        if (str.find(op) == 0)
        {
            tree.children.push_back({op, {}});
            return {tree, str.substr(op.size())};
        }
    }
    return {std::nullopt, str};
}

ReturnType Sign::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Sign: " << str << std::endl;

    auto tree = Node{"Sign", {}};
    for (auto &&sign: signs)
    {
        if (str.find(sign) == 0)
        {
            tree.children.push_back({sign, {}});
            return {tree, str.substr(sign.size())};
        }
    }
    return {std::nullopt, str};
}

ReturnType AdditionOperation::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "AdditionOperation: " << str << std::endl;

    auto tree = Node{"AdditionOperation", {}};
    for (auto &&op: additionOperators)
    {
        if (str.find(op) == 0)
        {
            tree.children.push_back({op, {}});
            return {tree, str.substr(op.size())};
        }
    }
    return {std::nullopt, str};
}

ReturnType MultiplicationOperation::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "MultiplicationOperation: " << str << std::endl;

    auto tree = Node{"MultiplicationOperation", {}};
    for (auto &&op: multiplicationOperators)
    {
        if (str.find(op) == 0)
        {
            tree.children.push_back({op, {}});
            return {tree, str.substr(op.size())};
        }
    }
    return {std::nullopt, str};
}

ReturnType Identifier::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Identifier: " << str << std::endl;

    auto tree = Node{"Identifier", {}};
    for (auto &&identifier: identifiers)
    {
        if (str.find(identifier) == 0)
        {
            bool canBeConstant = false;
            for (auto it = std::begin(constants); it != std::end(constants) && !canBeConstant; ++it)
            {
                if (str.find(*it) == 0)
                {
                    tree.data = "Constant";
                    tree.children.push_back({*it, {}});
                    return {tree, str.substr(it->size())};
                }
            }

            if (!canBeConstant)
            {
                tree.children.push_back({identifier, {}});
                return {tree, str.substr(identifier.size())};
            }
        }
    }
    return {std::nullopt, str};
}

ReturnType Constant::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Constant: " << str << std::endl;

    auto tree = Node{"Constant", {}};
    for (auto &&constant: constants)
    {
        if (str.find(constant) == 0)
        {
            bool canBeIdentifier = false;
            for (auto it = std::begin(identifiers); it != std::end(identifiers) && !canBeIdentifier;
                 ++it)
            {
                if (str.find(*it) == 0)
                {
                    tree.data = "Identifier";
                    tree.children.push_back({*it, {}});
                    return {tree, str.substr(it->size())};
                }
            }

            if (!canBeIdentifier)
            {
                tree.children.push_back({constant, {}});
                return {tree, str.substr(constant.size())};
            }
        }
    }
    return {std::nullopt, str};
}

ReturnType SimpleExpression1::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "SimpleExpression': " << str << std::endl;

    auto tree = Node{"SimpleExpression'", {}};
    if (auto &&[node1, newStr1] = ::accept<AdditionOperation>(str, depth + 1); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Term>(newStr1, depth + 2); node2)
        {
            if (auto &&[node3, newStr3] = ::accept<SimpleExpression1>(newStr2, depth + 3); node3)
            {
                tree.children.push_back(*node1);
                tree.children.push_back(*node2);
                tree.children.push_back(*node3);
                return {tree, newStr3};
            }
        }
    }

    tree.children.push_back({"ϵ", {}});
    return {tree, str};
}

ReturnType Term1::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Term': " << str << std::endl;

    auto tree = Node{"Term'", {}};
    if (auto &&[node1, newStr1] = ::accept<MultiplicationOperation>(str, depth + 1); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Factor>(newStr1, depth + 2); node2)
        {
            if (auto &&[node3, newStr3] = ::accept<Term1>(newStr2, depth + 3); node3)
            {
                tree.children.push_back(*node1);
                tree.children.push_back(*node2);
                tree.children.push_back(*node3);
                return {tree, newStr3};
            }
        }
    }

    tree.children.push_back({"ϵ", {}});
    return {tree, str};
}

ReturnType Expression1::accept(std::string_view str, size_t depth)
{
    std::cout << std::string(depth, '\t') << "Expression': " << str << std::endl;

    auto tree = Node{"Expression'", {}};
    if (auto &&[node1, newStr1] = ::accept<RelationOperation>(str, depth + 1); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<SimpleExpression>(newStr1, depth + 2); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }

    tree.children.push_back({"ϵ", {}});
    return {tree, str};
}
