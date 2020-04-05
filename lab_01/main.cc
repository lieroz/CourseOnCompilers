#include <iostream>

#include "utils.h"
#include "syntaxtree.h"
#include "dfa.h"

int main()
{
    std::string regexp;
    std::cout << "Input regexp: ";
    std::cin >> regexp;

    std::string matcher;
    std::cout << "Input string to match: ";
    std::cin >> matcher;

    SyntaxTree syntaxTree;
    syntaxTree.create(infixToPostfix(regexp));
    const auto &dfaStartState = syntaxTree.getRoot();

    std::cout << syntaxTree.toString() << std::endl;

    Dfa dfa;
    dfa.create(dfaStartState, syntaxTree);

    std::cout << dfa.toString(dfa.getTransitions()) << std::endl;
    std::cout << "Regexp match result dfa: " << std::boolalpha << dfa.match(matcher) << std::endl;

    dfa.minimize(syntaxTree.getAlphabet());
    std::cout << dfa.toString(dfa.getMinimizedTransitions()) << std::endl;
    std::cout << "Regexp match result min dfa: " << std::boolalpha << dfa.matchMinimized(matcher)
              << std::endl;

    return 0;
}
