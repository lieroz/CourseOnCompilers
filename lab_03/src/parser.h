#pragma once

#include <list>
#include <vector>
#include <string>
#include <string_view>
#include <optional>

namespace parser::detail
{
template<typename T, typename U>
using IsBaseOf = std::enable_if_t<std::is_base_of_v<T, U>>;

template<typename T, typename U>
using NotIsBaseOf = std::enable_if_t<!std::is_base_of_v<T, U>>;

struct Node
{
    std::string data;
    std::list<Node> children;
};

using ReturnType = std::pair<std::optional<Node>, std::string_view>;

struct GrammarElement
{
    virtual ReturnType accept(std::string_view, size_t depth) = 0;
};

struct Block : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct OperatorsList : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Tail : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Operator : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Expression : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct SimpleExpression : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Term : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Factor : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct RelationOperation : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Sign : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct AdditionOperation : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct MultiplicationOperation : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Identifier : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Constant : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct SimpleExpression1 : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Term1 : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

struct Expression1 : GrammarElement
{
    ReturnType accept(std::string_view, size_t depth) override;
};

template<typename T>
ReturnType accept(std::string_view str, size_t depth, IsBaseOf<GrammarElement, T> * = nullptr)
{
    return T{}.accept(str, depth);
}

template<typename T>
ReturnType accept(std::string_view, size_t, NotIsBaseOf<GrammarElement, T> * = nullptr)
{
    return {};
}
}  // namespace parser::detail

namespace parser
{
inline detail::ReturnType accept(std::string str)
{
    str.erase(std::remove(std::begin(str), std::end(str), ' '), std::end(str));
    return detail::accept<detail::Block>(str, 0);
}
}  // namespace parser
