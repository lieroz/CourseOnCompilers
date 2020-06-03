#pragma once

#include <string>
#include <vector>

namespace lisp
{
enum class CellType
{
    Symbol,
    Number,
    List,
    Proc,
    Lambda
};

struct Environment;

// a variant that can hold any kind of lisp value
struct Cell
{
    CellType type;

    std::string val;
    std::vector<Cell> list;
    std::function<Cell(const std::vector<Cell> &)> proc;

    std::shared_ptr<Environment> env;

    Cell(CellType type = CellType::Symbol) : type(type) {}
    Cell(CellType type, const std::string &val) : type(type), val(val) {}
    Cell(std::function<Cell(const std::vector<Cell> &)> proc) : type(CellType::Proc), proc(proc) {}
};

// initialize Lisp environment
std::shared_ptr<Environment> init();

// return the Lisp expression represented by the given string
Cell read(std::string_view str);

// convert given Cell to a Lisp-readable string
std::string toString(Cell exp);

// evaluate Lisp expressions into C++ functions
Cell eval(Cell x, std::shared_ptr<Environment> env);

// the default read-eval-print-loop
void repl(std::istream &input,
    std::ostream &output,
    std::string_view prompt,
    std::shared_ptr<Environment> env);
}  // namespace lisp
