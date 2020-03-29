#include <iostream>

#include <unordered_map>
#include <set>
#include <stack>

#include "utils.h"
#include "syntaxtree.h"

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
    auto regexp = infixToPostfix("(a|b)*abb");
    /* auto regexp = infixToPostfix("a(a|b)"); */

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
