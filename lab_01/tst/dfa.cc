#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "utils.h"
#include "syntaxtree.h"
#include "dfa.h"

TEST(Dfa, Test1)
{
    const std::string regexp = infixToPostfix("a|b");

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);

    const auto &dfaStartState = syntaxTree.getRoot();

    Dfa dfa;
    dfa.create(dfaStartState, syntaxTree);
    dfa.minimize(syntaxTree.getAlphabet());

    EXPECT_TRUE(dfa.match("a"));
    EXPECT_TRUE(dfa.matchMinimized("a"));

    EXPECT_TRUE(dfa.match("b"));
    EXPECT_TRUE(dfa.matchMinimized("b"));

    EXPECT_FALSE(dfa.match("ab"));
    EXPECT_FALSE(dfa.matchMinimized("ab"));
}

TEST(Dfa, Test2)
{
    const std::string regexp = infixToPostfix("(a|b)*abb");

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);

    const auto &dfaStartState = syntaxTree.getRoot();

    Dfa dfa;
    dfa.create(dfaStartState, syntaxTree);
    dfa.minimize(syntaxTree.getAlphabet());

    EXPECT_FALSE(dfa.match("aaaa"));
    EXPECT_FALSE(dfa.matchMinimized("aaaa"));

    EXPECT_FALSE(dfa.match("bbbb"));
    EXPECT_FALSE(dfa.matchMinimized("bbbb"));

    EXPECT_TRUE(dfa.match("ababb"));
    EXPECT_TRUE(dfa.matchMinimized("ababb"));
}

TEST(Dfa, Test3)
{
    const std::string regexp = infixToPostfix("a(a|b)");

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);

    const auto &dfaStartState = syntaxTree.getRoot();

    Dfa dfa;
    dfa.create(dfaStartState, syntaxTree);
    dfa.minimize(syntaxTree.getAlphabet());

    EXPECT_FALSE(dfa.match("a"));
    EXPECT_FALSE(dfa.matchMinimized("a"));

    EXPECT_TRUE(dfa.match("aa"));
    EXPECT_TRUE(dfa.matchMinimized("aa"));

    EXPECT_TRUE(dfa.match("ab"));
    EXPECT_TRUE(dfa.matchMinimized("ab"));
}

TEST(Dfa, Test4)
{
    const std::string regexp = infixToPostfix("((a|b)*(a|b)b*)|a");

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);

    const auto &dfaStartState = syntaxTree.getRoot();

    Dfa dfa;
    dfa.create(dfaStartState, syntaxTree);
    dfa.minimize(syntaxTree.getAlphabet());

    EXPECT_TRUE(dfa.match("a"));
    EXPECT_TRUE(dfa.matchMinimized("a"));

    EXPECT_TRUE(dfa.match("aaaaaaaa"));
    EXPECT_TRUE(dfa.matchMinimized("aaaaaaaa"));

    EXPECT_TRUE(dfa.match("bb"));
    EXPECT_TRUE(dfa.matchMinimized("bb"));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
