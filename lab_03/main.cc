#include <map>
#include <list>
#include <vector>

#include <iostream>

using Grammar = std::map<std::string, std::vector<std::vector<std::string>>>;

Grammar globalGrammar = {
    {"expression",
        {{"simple expression"}, {"simple expression", "relation operation", "simple expression"}}},
    {"simple expression",
        {{"term"}, {"sign", "term"}, {"simple expression", "addition type operation", "term"}}},
    {"term", {{"factor"}, {"factor", "multiplication type operation", "term"}}},
    {"factor", {{"identifier"}, {"constant"}, {"(", "simple expression", ")"}, {"not", "factor"}}},
    {"relation operation", {{"=="}, {"!"}, {"<"}, {"<="}, {">"}, {">="}}},
    {"sign", {{"+"}, {"-"}}},
    {"addition type operation", {{"+"}, {"-"}, {"or"}}},
    {"multiplication type operation", {{"*"}, {"/"}, {"div"}, {"mod"}, {"and"}}},
    {"identifier", {{"i"}}},
    {"constant", {{"c"}}},
    {"block", {{"{", "operators list", "}"}}},
    {"operators list", {{"operator", "tail"}}},
    {"tail", {{";", "operator", "tail"}, {";"}}},
    {"operator", {{"block"}, {"identifier", "=", "expression"}}},
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
            std::cout << prefix << "IF" << std::endl;
            return {Node{currentSymbol, {}}, stringToRead};
        }
        else if (currentSymbol == stringToRead.substr(0, currentSymbol.size()))
        {
            std::cout << prefix << "ELSE IF" << std::endl;
            return {Node{currentSymbol, {}}, stringToRead.substr(currentSymbol.size())};
        }
        else
        {
            std::cout << prefix << "ELSE" << std::endl;
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
                std::cout << prefix << "FOUND" << std::endl;
                return {retNode, newStr};
            }
        }
    }

    std::cout << prefix << "NOT FOUND" << std::endl;
    return {{}, stringToRead};
}

int main()
{
    auto &&[tree, str] = buildTree(globalGrammar, grammarStart, "{i=+c;i=c;}");
    if (tree)
    {
        std::cout << "TREEEEE" << std::endl;
    }
    return 0;
}
