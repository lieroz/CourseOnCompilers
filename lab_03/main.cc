#include <map>
#include <list>
#include <vector>
#include <stack>

#include <iostream>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

struct MyVertex;

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, MyVertex>;
using Grammar = std::map<std::string, std::vector<std::vector<std::string>>>;

Grammar grammar = {
    {"program", {{"block"}}},
    {"block", {{"{", "operators list", "}"}}},
    {"operators list", {{"operator", "tail"}}},
    {"tail", {{";", "operator", "tail"}, {";"}, {"e"}}},
    {"operator", {{"identifier", "=", "expression"}, {"{", "operators list", "}"}}},
    {"expression", {{"simple expression", "expression'"}}},
    {"simple expression", {{"term", "simple expression'"}, {"sign", "term", "simple expression'"}}},
    {"term", {{"factor", "term'"}}},
    {"factor", {{"identifier"}, {"constant"}, {"(", "simple expression", ")"}, {"not", "factor"}}},
    {"relation operation", {{"=="}, {"!"}, {"<"}, {"<="}, {">"}, {">="}}},
    {"sign", {{"+"}, {"-"}}},
    {"addition type operation", {{"+"}, {"-"}, {"or"}}},
    {"multiplication type operation", {{"*"}, {"/"}, {"div"}, {"mod"}, {"and"}}},
    {"identifier", {{"i"}}},
    {"constant", {{"c"}}},
    {"simple expression'", {{"addition type operation", "term", "simple expression'"}, {"e"}}},
    {"term'", {{"multiplication type operation", "factor", "term'"}, {"e"}}},
    {"expression'", {{"relation operation", "simple expression"}, {"e"}}},
};

static const std::string grammarStart = "block";
static const std::string epsilon = "e";

struct Node
{
    std::string data;
    std::list<Node> children;
};

std::pair<std::optional<Node>, std::string> buildTree(
    const Grammar &grammar, std::string currentSymbol, std::string stringToRead, size_t depth = 0)
{
    std::string prefix(depth, '\t');
    std::cout << prefix << currentSymbol << ": " << stringToRead << std::endl;
    if (grammar.find(currentSymbol) == std::end(grammar))
    {
        if (currentSymbol == epsilon)
        {
            std::cout << prefix << "EPSILON" << std::endl;
            return {Node{currentSymbol, {}}, stringToRead};
        }
        else if (currentSymbol == stringToRead.substr(0, currentSymbol.size()))
        {
            std::cout << prefix << "FOUND TERMINAL AT STR START" << std::endl;
            return {Node{currentSymbol, {}}, stringToRead.substr(currentSymbol.size())};
        }
        else
        {
            std::cout << prefix << "ERROR" << std::endl;
            return {{}, stringToRead};
        }
    }
    else
    {
        for (auto &&rule: grammar.at(currentSymbol))
        {
            auto newStr = stringToRead;
            auto retNode = Node{currentSymbol, {}};
            bool flag = false;

            for (auto &&symbol: rule)
            {
                std::cout << prefix << currentSymbol << " -> " << symbol << std::endl;
                auto &&[symbolChildren, str] = buildTree(grammar, symbol, newStr, depth + 1);
                newStr = str;

                if (!symbolChildren)
                {
                    flag = true;
                    break;
                }

                retNode.children.push_back(*symbolChildren);
            }

            if (!flag)
            {
                std::cout << prefix << "FOUND SUBTREE" << std::endl;
                return {retNode, newStr};
            }
        }
    }

    std::cout << prefix << "NOT FOUND SUBTREE" << std::endl;
    return {{}, stringToRead};
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
    auto &&[tree, _] = buildTree(grammar, grammarStart, "{i=-c;{i=c<(noti+c*c);}}");
    if (tree)
    {
        printTree(*tree);
    }

    return 0;
}
