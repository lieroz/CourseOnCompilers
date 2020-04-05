#include "dfa.h"

#include <deque>
#include <sstream>

#include "syntaxtree.h"

void Dfa::create(const Node &dfaStartState, const SyntaxTree &syntaxTree)
{
    size_t stateId = 0;
    size_t transitionId = 0;

    std::unordered_map<size_t, DfaTransitionState> dfaTransitionsTmp;
    DfaStates allDfaStates = {{stateId++, dfaStartState.firstPos}};
    std::set<DfaState> dfaStates = {dfaStartState.firstPos};
    std::deque<DfaState> q = {dfaStartState.firstPos};

    while (!q.empty())
    {
        auto state = q.front();
        q.pop_front();

        DfaTransitionState newStates;
        for (const auto &i: state)
        {
            // symbol == '#' - custom regexp end symbol
            if (i != syntaxTree.getSyntaxTree().size() - 2)
            {
                const auto &node = syntaxTree.getSyntaxTree().at(i);
                const auto &followPos = syntaxTree.getFollowPos().at(i);
                newStates[node.symbol].insert(std::begin(followPos), std::end(followPos));
            }
        }

        for (const auto &[symbol, newState]: newStates)
        {
            if (dfaStates.find(newState) == std::end(dfaStates))
            {
                allDfaStates[stateId++] = newState;
                dfaStates.insert(newState);
                q.push_back(newState);
            }
        }

        dfaTransitionsTmp[transitionId++] = newStates;
    }

    DfaTransitions dfaTransitions;

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

    std::swap(allDfaStates, states);
    std::swap(dfaTransitions, transitions);
}

void Dfa::minimize(const std::set<char> &alphabet)
{
    std::unordered_map<size_t, std::unordered_map<char, size_t>> newDfaTransitions;

    std::set<size_t> S1;
    std::set<size_t> S2;

    for (const auto &[id, state]: states)
    {
        if (id != states.size() - 1)
        {
            S1.insert(id);
        }
        else
        {
            S2.insert(id);
        }
    }

    std::set<std::set<size_t>> P = {S1, S2};
    std::deque<std::pair<std::set<size_t>, char>> S;

    for (const char &c: alphabet)
    {
        for (const auto &p: P)
        {
            S.push_back({p, c});
        }
    }

    while (!S.empty())
    {
        auto [C, a] = S.front();
        S.pop_front();

        auto newP = P;
        for (const auto &R: P)
        {
            std::set<size_t> R1;
            std::set<size_t> R2;

            for (const auto &r: R)
            {
                const auto tr = transitions[r];
                if (auto it = tr.find(a); it != std::end(tr))
                {
                    if (C.find(it->second) != std::end(C))
                    {
                        R1.insert(r);
                    }
                    else
                    {
                        R2.insert(r);
                    }
                }
            }

            if (!R1.empty() && !R2.empty())
            {
                newP.erase(R);  // replace with R1 and R2
                newP.insert(R1);
                newP.insert(R2);

                bool contains = false;

                for (auto it = std::begin(S); it != std::end(S) && !contains; ++it)
                {
                    if (it->first == R && it->second == a)
                    {
                        S.erase(it);
                        S.push_back({R1, a});
                        S.push_back({R2, a});

                        contains = true;
                    }
                }

                if (!contains)
                {
                    if (R1 <= R2)
                    {
                        S.push_back({R1, a});
                    }
                    else
                    {
                        S.push_back({R2, a});
                    }
                }
            }
        }

        std::swap(P, newP);
    }

    for (const auto &states: P)
    {
        auto i = *std::begin(states);
        newDfaTransitions[i] = transitions[i];
    }

    std::swap(minimizedTransitions, newDfaTransitions);
}

bool Dfa::match(std::string_view regexp) const
{
    size_t currentState = 0;

    for (const char &c: regexp)
    {
        const auto transitions = getTransitions().at(currentState);
        if (auto it = transitions.find(c); it == std::end(transitions))
        {
            return false;
        }
        else
        {
            currentState = it->second;
        }
    }

    return currentState == getStates().size() - 1 ? true : false;
}

bool Dfa::matchMinimized(std::string_view regexp) const
{
    size_t currentState = 0;

    for (const char &c: regexp)
    {
        const auto transitions = getMinimizedTransitions().at(currentState);
        if (auto it = transitions.find(c); it == std::end(transitions))
        {
            return false;
        }
        else
        {
            currentState = it->second;
        }
    }

    return currentState == getStates().size() - 1 ? true : false;
}

const Dfa::DfaStates &Dfa::getStates() const
{
    return states;
}

const Dfa::DfaTransitions &Dfa::getTransitions() const
{
    return transitions;
}

const Dfa::DfaTransitions &Dfa::getMinimizedTransitions() const
{
    return minimizedTransitions;
}

std::string Dfa::toString(const DfaTransitions &dfaTransitions) const
{
    std::stringstream ss;

    ss << "=====================\n";
    ss << "DFA:\n";
    ss << "=====================\n\n";

    for (const auto &[id, transitions]: dfaTransitions)
    {
        ss << "ID: " << id << std::endl;
        const auto &dfaState = getStates().at(id);
        for (const auto &[symbol, id]: transitions)
        {
            std::copy(
                std::begin(dfaState), std::end(dfaState), std::ostream_iterator<size_t>(ss, " "));
            ss << "-> " << symbol << " -> " << id << "\n";
        }
        ss << "\n";
    }

    ss << "<<<<<<<<<<<<<<<<<<<<<\n";
    return ss.str();
}
