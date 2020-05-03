#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include "parser.h"

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

void printTree(const parser::detail::Node &tree)
{
    Graph graph;

    static auto node = [&graph](std::string name, int id) {
        for (auto &&v: boost::make_iterator_range(vertices(graph)))
            if (graph[v] == MyVertex{name, id})
                return v;
        return add_vertex({name, id}, graph);
    };

    size_t count = 0;
    std::stack<std::pair<parser::detail::Node, size_t>> stack;
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
    std::string s = "{a=-b+const<(nota+b)*bdiva;{a=a<>a;{c=const}}}";
    auto &&[tree, str] = parser::accept(s);
    std::cout << str << std::endl;

    if (tree && str.empty())
    {
        printTree(*tree);
    }

    return 0;
}
