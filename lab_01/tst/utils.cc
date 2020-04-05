#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "utils.h"

TEST(Utils, InfixToPostfix)
{
    const std::string infix = "(a|b)*baa*(aa|b)";
    const auto postfix = infixToPostfix(infix);

    EXPECT_EQ(postfix, "ab|*b&a&a*&aa&b|&#&");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
