#include "utils.h"

#include <string>
#include <unordered_map>
#include <set>
#include <stack>

inline constexpr char regexpEndingSymbol = '#';

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

auto formatRegexp(std::string_view regex)
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
    return result;
}

std::string infixToPostfix(std::string_view infix)
{
    auto formatted = formatRegexp(infix);

    std::string postfix;
    postfix.reserve(formatted.size());

    std::stack<char> stack;

    for (const char &c: formatted)
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

    postfix.push_back(regexpEndingSymbol);
    postfix.push_back('&');
    return postfix;
}
