#include <map>
#include <list>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

template<typename T, typename U>
using IsBaseOf = std::enable_if_t<std::is_base_of_v<T, U>>;

template<typename T, typename U>
using NotIsBaseOf = std::enable_if_t<!std::is_base_of_v<T, U>>;

struct Node
{
    std::string data;
    std::list<Node> children;
};

static const std::set<std::string> identifiers = {"a", "b", "c"};
static const std::set<std::string> constants = {"const"};

using ReturnType = std::pair<std::optional<Node>, std::string_view>;

struct GrammarElement
{
    virtual ReturnType accept(std::string_view) = 0;
};

struct Block : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct OperatorsList : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Tail : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Operator : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Expression : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct SimpleExpression : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Term : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Factor : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct RelationOperation : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Sign : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct AdditionOperation : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct MultiplicationOperation : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Identifier : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Constant : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct SimpleExpression1 : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Term1 : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

struct Expression1 : GrammarElement
{
    ReturnType accept(std::string_view) override;
};

template<typename T>
ReturnType accept(std::string_view str, IsBaseOf<GrammarElement, T> * = nullptr)
{
    return T{}.accept(str);
}

template<typename T>
ReturnType accept(std::string_view, NotIsBaseOf<GrammarElement, T> * = nullptr)
{
    return {};
}

ReturnType Block::accept(std::string_view str)
{
    std::cout << "Block: " << str << std::endl;

    auto tree = Node{"Block", {}};
    if (str.front() == '{' && str.back() == '}')
    {
        if (auto &&[node, newStr] = ::accept<OperatorsList>(str.substr(1)); node)
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

ReturnType OperatorsList::accept(std::string_view str)
{
    std::cout << "OperatorsList: " << str << std::endl;

    auto tree = Node{"OperatorsList", {}};
    if (auto &&[node1, newStr1] = ::accept<Operator>(str); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Tail>(newStr1); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }
    return {std::nullopt, str};
}

ReturnType Tail::accept(std::string_view str)
{
    std::cout << "Tail: " << str << std::endl;

    auto tree = Node{"Tail", {}};
    if (str.front() == ';')
    {
        if (auto &&[node1, newStr1] = ::accept<Operator>(str.substr(1)); node1)
        {
            if (auto &&[node2, newStr2] = ::accept<Tail>(newStr1); node2)
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

ReturnType Operator::accept(std::string_view str)
{
    std::cout << "Operator: " << str << std::endl;

    auto tree = Node{"Operator", {}};
    if (auto &&[node1, newStr1] = ::accept<Identifier>(str); node1)
    {
        if (newStr1.front() == '=')
        {
            if (auto &&[node2, newStr2] = ::accept<Expression>(newStr1.substr(1)); node2)
            {
                tree.children.push_back(*node1);
                tree.children.push_back({"=", {}});
                tree.children.push_back(*node2);
                return {tree, newStr2};
            }
        }
    }

    if (auto &&[node, newStr] = ::accept<Block>(str); node)
    {
        tree.children.push_back(*node);
        return {tree, newStr};
    }
    return {std::nullopt, str};
}

ReturnType Expression::accept(std::string_view str)
{
    std::cout << "Expression: " << str << std::endl;

    auto tree = Node{"Expression", {}};
    if (auto &&[node1, newStr1] = ::accept<SimpleExpression>(str); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Expression1>(newStr1); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }
    return {std::nullopt, str};
}

ReturnType SimpleExpression::accept(std::string_view str)
{
    std::cout << "SimpleExpression: " << str << std::endl;

    auto tree = Node{"SimpleExpression", {}};
    if (auto &&[node1, newStr1] = ::accept<Term>(str); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<SimpleExpression1>(newStr1); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }

    if (auto &&[node1, newStr1] = ::accept<Sign>(str); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Term>(newStr1); node1)
        {
            if (auto &&[node3, newStr3] = ::accept<SimpleExpression1>(newStr2); node1)
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

ReturnType Term::accept(std::string_view str)
{
    std::cout << "Term: " << str << std::endl;

    auto tree = Node{"Term", {}};
    if (auto &&[node1, newStr1] = ::accept<Factor>(str); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Term1>(newStr1); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }
    return {std::nullopt, str};
}

ReturnType Factor::accept(std::string_view str)
{
    std::cout << "Factor: " << str << std::endl;

    auto tree = Node{"Factor", {}};
    if (auto &&[node, newStr] = ::accept<Identifier>(str); node)
    {
        tree.children.push_back(*node);
        return {tree, newStr};
    }
    if (auto &&[node, newStr] = ::accept<Constant>(str); node)
    {
        tree.children.push_back(*node);
        return {tree, newStr};
    }
    if (str.front() == '(')
    {
        if (auto &&[node, newStr] = ::accept<SimpleExpression>(str.substr(1)); node)
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
        if (auto &&[node, newStr] = ::accept<Factor>(str.substr(3)); node)
        {
            tree.children.push_back(*node);
            return {tree, newStr};
        }
    }

    return {std::nullopt, str};
}

ReturnType RelationOperation::accept(std::string_view str)
{
    std::cout << "RelationOperation: " << str << std::endl;

    auto tree = Node{"RelationOperation", {}};
    if (str.find("==") == 0)
    {
        tree.children.push_back({"==", {}});
        return {tree, str.substr(2)};
    }
    else if (str.find("<>") == 0)
    {
        tree.children.push_back({"<>", {}});
        return {tree, str.substr(2)};
    }
    else if (str.find("<") == 0)
    {
        tree.children.push_back({"<", {}});
        return {tree, str.substr(1)};
    }
    else if (str.find("<=") == 0)
    {
        tree.children.push_back({"<=", {}});
        return {tree, str.substr(2)};
    }
    else if (str.find(">") == 0)
    {
        tree.children.push_back({">", {}});
        return {tree, str.substr(1)};
    }
    else if (str.find(">=") == 0)
    {
        tree.children.push_back({">=", {}});
        return {tree, str.substr(2)};
    }
    return {std::nullopt, str};
}

ReturnType Sign::accept(std::string_view str)
{
    std::cout << "Sign: " << str << std::endl;

    auto tree = Node{"Sign", {}};
    if (str.front() == '+')
    {
        tree.children.push_back({"+", {}});
        return {tree, str.substr(1)};
    }
    else if (str.front() == '-')
    {
        tree.children.push_back({"-", {}});
        return {tree, str.substr(1)};
    }
    return {std::nullopt, str};
}

ReturnType AdditionOperation::accept(std::string_view str)
{
    std::cout << "AdditionOperation: " << str << std::endl;

    auto tree = Node{"AdditionOperation", {}};
    if (str.front() == '+')
    {
        tree.children.push_back({"+", {}});
        return {tree, str.substr(1)};
    }
    else if (str.front() == '-')
    {
        tree.children.push_back({"-", {}});
        return {tree, str.substr(1)};
    }
    else if (str.find("or") == 0)
    {
        tree.children.push_back({"or", {}});
        return {tree, str.substr(2)};
    }
    return {std::nullopt, str};
}

ReturnType MultiplicationOperation::accept(std::string_view str)
{
    std::cout << "MultiplicationOperation: " << str << std::endl;

    auto tree = Node{"MultiplicationOperation", {}};
    if (str.front() == '*')
    {
        tree.children.push_back({"*", {}});
        return {tree, str.substr(1)};
    }
    else if (str.front() == '/')
    {
        tree.children.push_back({"/", {}});
        return {tree, str.substr(1)};
    }
    else if (str.find("div") == 0)
    {
        tree.children.push_back({"div", {}});
        return {tree, str.substr(3)};
    }
    else if (str.find("mod") == 0)
    {
        tree.children.push_back({"mod", {}});
        return {tree, str.substr(3)};
    }
    else if (str.find("and") == 0)
    {
        tree.children.push_back({"and", {}});
        return {tree, str.substr(3)};
    }
    return {std::nullopt, str};
}

ReturnType Identifier::accept(std::string_view str)
{
    std::cout << "Identifier: " << str << std::endl;

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

ReturnType Constant::accept(std::string_view str)
{
    std::cout << "Constant: " << str << std::endl;

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

ReturnType SimpleExpression1::accept(std::string_view str)
{
    std::cout << "SimpleExpression': " << str << std::endl;

    auto tree = Node{"SimpleExpression'", {}};
    if (auto &&[node1, newStr1] = ::accept<AdditionOperation>(str); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Term>(newStr1); node2)
        {
            if (auto &&[node3, newStr3] = ::accept<SimpleExpression1>(newStr2); node3)
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

ReturnType Term1::accept(std::string_view str)
{
    std::cout << "Term': " << str << std::endl;

    auto tree = Node{"Term'", {}};
    if (auto &&[node1, newStr1] = ::accept<MultiplicationOperation>(str); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<Factor>(newStr1); node2)
        {
            if (auto &&[node3, newStr3] = ::accept<Term1>(newStr2); node3)
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

ReturnType Expression1::accept(std::string_view str)
{
    std::cout << "Expression': " << str << std::endl;

    auto tree = Node{"Expression'", {}};
    if (auto &&[node1, newStr1] = ::accept<RelationOperation>(str); node1)
    {
        if (auto &&[node2, newStr2] = ::accept<SimpleExpression>(newStr1); node2)
        {
            tree.children.push_back(*node1);
            tree.children.push_back(*node2);
            return {tree, newStr2};
        }
    }

    tree.children.push_back({"ϵ", {}});
    return {tree, str};
}

ReturnType accept(std::string str)
{
    str.erase(std::remove(std::begin(str), std::end(str), ' '), std::end(str));
    return accept<Block>(str);
}

struct MyVertex
{
    std::string label;
    int id;

    auto key() const
    {
        return std::tie(id, label);
    }
    bool operator<(const MyVertex &rhs) const
    {
        return key() < rhs.key();
    }
    bool operator==(const MyVertex &rhs) const
    {
        return key() == rhs.key();
    }
    bool operator!=(const MyVertex &rhs) const
    {
        return key() != rhs.key();
    }
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, MyVertex>;
using Grammar = std::map<std::string, std::vector<std::vector<std::string>>>;

void printTree(const Node &tree)
{
    Graph graph;

    static auto node = [&graph](std::string name, int id) {
        for (auto &&v: boost::make_iterator_range(vertices(graph)))
            if (graph[v] == MyVertex{name, id})
                return v;
        return add_vertex({name, id}, graph);
    };

    size_t count = 0;
    std::stack<std::pair<Node, size_t>> stack;
    stack.push({tree, count});

    while (!stack.empty())
    {
        auto [n, num] = stack.top();
        stack.pop();
        auto v = node(n.data + " " + std::to_string(num), num);

        for (auto &&child: n.children)
        {
            ++count;
            stack.push({child, count});

            auto v_ = node(child.data + " " + std::to_string(count), count);
            boost::add_edge(v, v_, graph);
        }
    }

    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(&MyVertex::id, graph));
    dp.property("label", boost::get(&MyVertex::label, graph));

    std::ofstream file("tree.txt");
    write_graphviz_dp(file, graph, dp);
    write_graphviz(file, graph);
}

int main()
{
    std::string s = "{a=-b+const<(nota+b)*b;{a=a<>a;{c=const}}}";
    auto &&[tree, str] = accept(s);
    std::cout << str << std::endl;

    if (tree && str.empty())
    {
        printTree(*tree);
    }

    return 0;
}
