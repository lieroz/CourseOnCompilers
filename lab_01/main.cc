#include <iostream>

#include <unordered_map>

#include "utils.h"
#include "syntaxtree.h"
#include "dfa.h"

int main()
{
    /* auto regexp = infixToPostfix("(a|b)*abb"); */
    /* auto regexp = infixToPostfix("a(a|b)"); */
    auto regexp = infixToPostfix("((a|b)*(a|b)b*)|a");
    /* auto regexp = infixToPostfix("a|b"); */

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);
    const auto &dfaStartState = syntaxTree.getRoot();

    std::cout << syntaxTree.toString() << std::endl;

    Dfa dfa;
    dfa.create(dfaStartState, syntaxTree);

    std::cout << dfa.toString(dfa.getTransitions()) << std::endl;
    std::cout << "Regexp match result dfa: " << std::boolalpha << dfa.match("abb") << std::endl;

    dfa.minimize(syntaxTree.getAlphabet());
    std::cout << dfa.toString(dfa.getMinimizedTransitions()) << std::endl;
    std::cout << "Regexp match result min dfa: " << std::boolalpha << dfa.matchMinimized("abb")
              << std::endl;

    return 0;
}
