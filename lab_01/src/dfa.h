#pragma once

#include <set>
#include <unordered_map>
#include <string_view>

class Node;
class SyntaxTree;

class Dfa
{
    using DfaState = std::set<size_t>;
    using DfaTransitionState = std::unordered_map<char, DfaState>;
    using DfaStates = std::unordered_map<size_t, DfaState>;
    using DfaTransitions = std::unordered_map<size_t, std::unordered_map<char, size_t>>;

public:
    void create(const Node &dfaStartState, const SyntaxTree &syntaxTree);
    void minimize(const std::set<char> &alphabet);

    bool match(std::string_view regexp) const;
    bool matchMinimized(std::string_view regexp) const;

    const DfaStates &getStates() const;
    const DfaTransitions &getTransitions() const;
    const DfaTransitions &getMinimizedTransitions() const;

    std::string toString(const DfaTransitions &dfaTransitions) const;

private:
    DfaStates states;
    DfaTransitions transitions;
    DfaTransitions minimizedTransitions;
};
