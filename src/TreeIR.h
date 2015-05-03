#pragma once

#include "common.h"
#include "Temps.h"

namespace TreeIR {

enum class BinaryOp : uint8_t
{
    Plus,
    Minus,
    Mul,
    Div,
    And,
    Or,
    LShift,
    RShift,
    ArithShift,
    Xor
};

enum class RelationalOp : uint8_t
{
    Eq,
    Ne,
    Lt,
    Gt,
    Le,
    Ge,
    ULt,
    ULe,
    UGt,
    UGe
};

struct TreeExpression { virtual ~TreeExpression() {} };

struct TreeStatement { virtual ~TreeStatement() {} };

struct ConstExp : public TreeExpression
{
    const int64_t m_int;
    ConstExp() = delete;
};

struct NameExp : public TreeExpression 
{
    const Temps::Label m_label;
    NameExp() = delete;
};

struct TempExp : public TreeExpression
{
    const Temps::LocalTemp m_temp;
    TempExp() = delete;
};

struct BinOpExp : public TreeExpression
{
    const BinaryOp m_op;
    const std::unique_ptr<TreeExpression> m_lhs;
    const std::unique_ptr<TreeExpression> m_rhs;
    BinOpExp() = delete;
};

struct MemExp : public TreeExpression
{
    const std::unique_ptr<TreeExpression> m_exp;
    MemExp() = delete;
};

struct CallExp : public TreeExpression
{
    const std::unique_ptr<TreeExpression> m_fun;
    const std::vector<std::unique_ptr<TreeExpression>> m_args;
    CallExp(std::unique_ptr<TreeExpression> fun, std::vector<std::unique_ptr<TreeExpression>>&& args)
        : m_fun(std::move(fun))
        , m_args(std::move(args))
    {}
    CallExp() = delete;
};

struct SeqExp : public TreeExpression
{
    const std::unique_ptr<TreeStatement> m_stm;
    const std::unique_ptr<TreeExpression> m_exp;
    SeqExp() = delete;
};

struct MoveStm : public TreeStatement
{
    const std::unique_ptr<TreeExpression> m_target;
    const std::unique_ptr<TreeExpression> m_toMove;
    MoveStm() = delete;
};

struct ExpStm : public TreeStatement
{
    const std::unique_ptr<TreeExpression> m_exp;
    ExpStm() = delete;
};

struct JumpStm : public TreeStatement
{
    const std::unique_ptr<TreeExpression> m_exp;
    const std::vector<Temps::Label> m_labels;
    JumpStm() = delete;
};

struct ConditionalJumpStm : public TreeStatement
{
    const RelationalOp m_op;
    const std::unique_ptr<TreeExpression> m_lhs;
    const std::unique_ptr<TreeExpression> m_rhs;
    const Temps::Label m_trueBranch;
    const Temps::Label m_falseBranch;
    ConditionalJumpStm() = delete;
};

struct SeqStm : public TreeStatement
{
    const std::unique_ptr<TreeStatement> m_first;
    const std::unique_ptr<TreeStatement> m_next;
    SeqStm() = delete;
};

struct LabelStm : public TreeStatement
{
    const Temps::Label m_label;
    LabelStm() = delete;
};

}
