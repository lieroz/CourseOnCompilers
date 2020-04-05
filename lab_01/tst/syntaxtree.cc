#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "utils.h"
#include "syntaxtree.h"

TEST(SyntaxTree, Test1)
{
    const std::string regexp = infixToPostfix("a|b");

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);

    const auto &root = syntaxTree.getRoot();

    EXPECT_EQ(root.symbol, '&');
    EXPECT_FALSE(root.nullable);

    ASSERT_THAT(syntaxTree.getFollowPos().at(0), ::testing::ElementsAre(3));
    ASSERT_THAT(syntaxTree.getFollowPos().at(1), ::testing::ElementsAre(3));

    ASSERT_THAT(root.firstPos, ::testing::ElementsAre(0, 1));
    ASSERT_THAT(root.lastPos, ::testing::ElementsAre(3));
}

TEST(SyntaxTree, Test2)
{
    const std::string regexp = infixToPostfix("(a|b)*abb");

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);

    const auto &root = syntaxTree.getRoot();

    EXPECT_EQ(root.symbol, '&');
    EXPECT_FALSE(root.nullable);

    ASSERT_THAT(syntaxTree.getFollowPos().at(0), ::testing::ElementsAre(0, 1, 4));
    ASSERT_THAT(syntaxTree.getFollowPos().at(1), ::testing::ElementsAre(0, 1, 4));
    ASSERT_THAT(syntaxTree.getFollowPos().at(4), ::testing::ElementsAre(6));
    ASSERT_THAT(syntaxTree.getFollowPos().at(6), ::testing::ElementsAre(8));
    ASSERT_THAT(syntaxTree.getFollowPos().at(8), ::testing::ElementsAre(10));

    ASSERT_THAT(root.firstPos, ::testing::ElementsAre(0, 1, 4));
    ASSERT_THAT(root.lastPos, ::testing::ElementsAre(10));
}

TEST(SyntaxTree, Test3)
{
    const std::string regexp = infixToPostfix("a(a|b)");

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);

    const auto &root = syntaxTree.getRoot();

    EXPECT_EQ(root.symbol, '&');
    EXPECT_FALSE(root.nullable);

    ASSERT_THAT(syntaxTree.getFollowPos().at(0), ::testing::ElementsAre(1, 2));
    ASSERT_THAT(syntaxTree.getFollowPos().at(1), ::testing::ElementsAre(5));
    ASSERT_THAT(syntaxTree.getFollowPos().at(2), ::testing::ElementsAre(5));

    ASSERT_THAT(root.firstPos, ::testing::ElementsAre(0));
    ASSERT_THAT(root.lastPos, ::testing::ElementsAre(5));
}

TEST(SyntaxTree, Test4)
{
    const std::string regexp = infixToPostfix("((a|b)*(a|b)b*)|a");

    SyntaxTree syntaxTree;
    syntaxTree.create(regexp);

    const auto &root = syntaxTree.getRoot();

    EXPECT_EQ(root.symbol, '&');
    EXPECT_FALSE(root.nullable);

    ASSERT_THAT(syntaxTree.getFollowPos().at(0), ::testing::ElementsAre(0, 1, 4, 5));
    ASSERT_THAT(syntaxTree.getFollowPos().at(1), ::testing::ElementsAre(0, 1, 4, 5));
    ASSERT_THAT(syntaxTree.getFollowPos().at(4), ::testing::ElementsAre(8, 13));
    ASSERT_THAT(syntaxTree.getFollowPos().at(5), ::testing::ElementsAre(8, 13));
    ASSERT_THAT(syntaxTree.getFollowPos().at(8), ::testing::ElementsAre(8, 13));
    ASSERT_THAT(syntaxTree.getFollowPos().at(11), ::testing::ElementsAre(13));

    ASSERT_THAT(root.firstPos, ::testing::ElementsAre(0, 1, 4, 5, 11));
    ASSERT_THAT(root.lastPos, ::testing::ElementsAre(13));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
