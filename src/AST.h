#pragma once
#include <memory>
#include <string>
using namespace std;

namespace AST
{
typedef std::string Location;

struct ArithmeticExpression {};

struct NumericExpression
    : public ArithmeticExpression
{
    int value;
};

struct LocationExpession
    : public ArithmeticExpression
{
    std::unique_ptr<Location> location;
};

struct AdditionExpression
    : public ArithmeticExpression
{
    std::unique_ptr<ArithmeticExpression> lhs;
    std::unique_ptr<ArithmeticExpression> rhs;
};

struct SubtractionExpression
    : public ArithmeticExpression
{
    std::unique_ptr<ArithmeticExpression> lhs;
    std::unique_ptr<ArithmeticExpression> rhs;
};

struct MultiplicationExpression
    : public ArithmeticExpression
{
    std::unique_ptr<ArithmeticExpression> lhs;
    std::unique_ptr<ArithmeticExpression> rhs;
};

struct BooleanExpression {};

struct TruthPrimativeExpression
    : public BooleanExpression
{
    bool value;
};

struct EqualExpression
    : BooleanExpression
{
    std::unique_ptr<ArithmeticExpression> lhs;
    std::unique_ptr<ArithmeticExpression> rhs;
};

struct LessThanOrEqualExpression
    : BooleanExpression
{
    std::unique_ptr<ArithmeticExpression> lhs;
    std::unique_ptr<ArithmeticExpression> rhs;
};

struct NotExpression
    : BooleanExpression
{
    std::unique_ptr<BooleanExpression> lhs;
};

struct AndExpression
    : BooleanExpression
{
    std::unique_ptr<BooleanExpression> lhs;
    std::unique_ptr<BooleanExpression> rhs;
};

struct OrThanOrEqualExpression
    : BooleanExpression
{
    std::unique_ptr<BooleanExpression> lhs;
    std::unique_ptr<BooleanExpression> rhs;
};

struct Command {};

struct SkipCommand
    : Command
{
};

struct AssignCommand
    : Command
{
    std::unique_ptr<Location> location;
    std::unique_ptr<ArithmeticExpression> rhs;
};

struct SequenceCommand
    : Command
{
    std::unique_ptr<Command> lhs;
    std::unique_ptr<Command> rhs;
};

struct IfCommand
    : Command
{
    std::unique_ptr<BooleanExpression> condition;
    std::unique_ptr<Command> lhs;
    std::unique_ptr<Command> rhs;
};

struct WhileCommand
    : Command
{
    std::unique_ptr<BooleanExpression> condition;
    std::unique_ptr<Command> command;
};
}
