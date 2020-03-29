#include <iostream>

#include <unordered_map>
#include <set>
#include <stack>

static constexpr char regexpEndingSymbol = '#';

// Regex infix to postfix part start

enum class Operators
{
    OPEN_PARENTHESIS,  // (
    ALTERNATION,       // |
    CONCATENATION,     // & explicit concatenation operator
    ZERO_OR_MORE,      // *
    SYMBOL,
};

static const std::unordered_map<char, Operators> precedenceMap = {
    {'(', Operators::OPEN_PARENTHESIS},
    {'|', Operators::ALTERNATION},
    {'&', Operators::CONCATENATION},
    {'*', Operators::ZERO_OR_MORE},
};

auto getPrecedence(char c)
{
    auto precedence = precedenceMap.find(c);
    return precedence == std::end(precedenceMap) ? Operators::SYMBOL : precedence->second;
}

auto formatRegex(std::string_view regex)
{
    std::string result;
    result.reserve(regex.size());

    std::set<char> allOperators = {'|', '*'};
    std::set<char> binaryOperators = {'|'};

    for (size_t i = 1, size = regex.size(); i < size; ++i)
    {
        char c1 = regex[i - 1];
        char c2 = regex[i];

        result.push_back(c1);

        if (c1 != '(' && c2 != ')' && allOperators.find(c2) == std::end(allOperators) &&
            binaryOperators.find(c1) == std::end(binaryOperators))
        {
            result.push_back('&');
        }
    }

    result.push_back(regex.back());
    result.push_back('&');
    result.push_back(regexpEndingSymbol);
    return result;
}

auto infixToPostfix(std::string_view infix)
{
    std::string postfix;
    postfix.reserve(infix.size());

    std::stack<char> stack;

    for (const char &c: infix)
    {
        switch (c)
        {
            case '(':
                stack.push(c);
                break;
            case ')':
                while (stack.top() != '(')
                {
                    postfix.push_back(stack.top());
                    stack.pop();
                }

                stack.pop();
                break;
            default:
                while (!stack.empty())
                {
                    auto stackPrecedence = getPrecedence(stack.top());
                    auto precedence = getPrecedence(c);

                    if (stackPrecedence >= precedence)
                    {
                        postfix.push_back(stack.top());
                        stack.pop();
                    }
                    else
                    {
                        break;
                    }
                }

                stack.push(c);
                break;
        }
    }

    while (!stack.empty())
    {
        postfix.push_back(stack.top());
        stack.pop();
    }

    return postfix;
}

// Regex infix to postfix part end
//
// Regex to syntax tree start

struct Node
{
    char symbol;
    bool nullable = false;

    std::set<size_t> firstPos;
    std::set<size_t> lastPos;
};

class SyntaxTree
{
public:
    auto generateSyntaxTree(std::string_view regexp)
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
                    alternate(nodeId, node);
                    break;
                case '&':
                    concatenate(nodeId, node);
                    break;
                case '*':
                    star(nodeId, node);
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

    const auto &getSyntaxTree() const
    {
        return syntaxTree;
    }

    const auto &getFollowPos() const
    {
        return followPos;
    }

private:
    void alternate(size_t nodeId, Node &node)
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

    void concatenate(size_t nodeId, Node &node)
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

    void star(size_t nodeId, Node &node)
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

private:
    std::stack<Node> stack;
    std::unordered_map<size_t, Node> syntaxTree;
    std::unordered_map<size_t, std::set<size_t>> followPos;
};

// Regex to syntax tree end
//
// DFA part start
using DfaState = std::set<size_t>;
using DfaTransitionState = std::unordered_map<char, DfaState>;

auto createDfa(const Node &dfaStartState, const SyntaxTree &syntaxTree)
{
    size_t stateId = 0;
    std::unordered_map<size_t, DfaTransitionState> dfaTransitionsTmp;
    std::unordered_map<size_t, DfaState> allDfaStates;
    std::set<DfaState> dfaStates = {dfaStartState.firstPos};

    for (const auto &state: dfaStates)
    {
        DfaTransitionState newStates;
        for (const auto &i: state)
        {
            const auto &node = syntaxTree.getSyntaxTree().at(i);
            // symbol == '#' - custom regexp end symbol
            if (i != syntaxTree.getSyntaxTree().size() - 2)
            {
                const auto &followPos = syntaxTree.getFollowPos().at(i);
                newStates[node.symbol].insert(std::begin(followPos), std::end(followPos));
            }
        }

        for (const auto &[symbol, state]: newStates)
        {
            dfaStates.insert(state);
        }

        allDfaStates[stateId] = state;
        dfaTransitionsTmp[stateId] = newStates;

        ++stateId;
    }

    std::unordered_map<size_t, std::unordered_map<char, size_t>> dfaTransitions;

    for (const auto &[id, transitions]: dfaTransitionsTmp)
    {
        std::unordered_map<char, size_t> tmp;
        for (const auto &[symbol, state]: transitions)
        {
            auto it = std::find_if(std::begin(allDfaStates), std::end(allDfaStates),
                [state = std::ref(state)](auto &&x) { return x.second == state.get(); });
            tmp[symbol] = it->first;
        }
        dfaTransitions[id] = tmp;
    }

    return std::make_pair(allDfaStates, dfaTransitions);
}

// DFA part end
//
// DFA minimization part start (Hopcroft)
// ===
// DFA minimization part end

bool matchRegexp(const std::unordered_map<size_t, DfaState> &dfaStates,
    const std::unordered_map<size_t, std::unordered_map<char, size_t>> &dfaTransitions,
    std::string_view regexp)
{
    size_t currentState = 0;

    for (const char &c: regexp)
    {
        const auto transitions = dfaTransitions.at(currentState);
        if (auto it = transitions.find(c); it == std::end(transitions))
        {
            return false;
        }
        else
        {
            currentState = it->second;
        }
    }

    return currentState == dfaStates.size() - 1 ? true : false;
}

int main()
{
    auto regexp = infixToPostfix(formatRegex("(a|b)*abb"));
    /* auto regexp = infixToPostfix(formatRegex("a(a|b)")); */

    SyntaxTree syntaxTree;
    auto dfaStartState = syntaxTree.generateSyntaxTree(regexp);

    for (const auto &[pos, node]: syntaxTree.getSyntaxTree())
    {
        std::cout << "POS: " << pos;
        std::cout << "\nSYMBOL: " << node.symbol;
        std::cout << "\nNULLABLE: " << node.nullable;

        std::cout << "\nFIRST POS: ";
        std::copy(std::begin(node.firstPos), std::end(node.firstPos),
            std::ostream_iterator<size_t>(std::cout, " "));

        std::cout << "\nLAST POS: ";
        std::copy(std::begin(node.lastPos), std::end(node.lastPos),
            std::ostream_iterator<size_t>(std::cout, " "));
        std::cout << "\n\n";
    }

    std::cout << "=====================\n\n";

    for (const auto &[pos, followPos]: syntaxTree.getFollowPos())
    {
        std::cout << "POS: " << pos;
        std::cout << "\nFOLLOW POS: ";
        std::copy(std::begin(followPos), std::end(followPos),
            std::ostream_iterator<size_t>(std::cout, " "));
        std::cout << "\n\n";
    }

    std::cout << "=====================\n\n";

    std::cout << "SYMBOL: " << dfaStartState.symbol;
    std::cout << "\nNULLABLE: " << dfaStartState.nullable;

    std::cout << "\nFIRST POS: ";
    std::copy(std::begin(dfaStartState.firstPos), std::end(dfaStartState.firstPos),
        std::ostream_iterator<size_t>(std::cout, " "));

    std::cout << "\nLAST POS: ";
    std::copy(std::begin(dfaStartState.lastPos), std::end(dfaStartState.lastPos),
        std::ostream_iterator<size_t>(std::cout, " "));
    std::cout << "\n\n";

    std::cout << "=====================\n\n";

    auto [dfaStates, dfaTransitions] = createDfa(dfaStartState, syntaxTree);

    for (const auto &[id, transitions]: dfaTransitions)
    {
        const auto &dfaState = dfaStates[id];
        for (const auto &[symbol, id]: transitions)
        {
            std::copy(std::begin(dfaState), std::end(dfaState),
                std::ostream_iterator<size_t>(std::cout, " "));
            std::cout << "-> " << symbol << " -> " << id << "\n";
        }
        std::cout << "\n";
    }

    std::cout << "Regexp match result: " << std::boolalpha
              << matchRegexp(dfaStates, dfaTransitions, "abb") << std::endl;

    return 0;
}
