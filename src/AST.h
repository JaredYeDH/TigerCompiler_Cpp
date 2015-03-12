#pragma once
#include "Environments.h"
#include "Position.h"
#include "Symbol.h"
#include "CompileTimeErrorHandler.h"

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <boost/optional.hpp>

namespace AST
{

enum class BinOp : unsigned int
{
   Plus,
   Minus,
   Times,
   Div,
   Eq,
   Neq,
   Lt,
   Le,
   Gt,
   Ge 
};

struct Field
{
    Symbol name;
    bool escape;
    Symbol type;
    Position position;

    Field(const Symbol& nm, const Symbol& ty, const Position& pos)
        : name(nm)
        , escape(true)
        , type(ty)
        , position(pos)
    {
    }
};

struct AstNode
{
    virtual const Position& UsePosition()
    {
        return m_position;
    }

    virtual Type TypeCheck() = 0;
    
    virtual ~AstNode() {}
    
    AstNode()
        : m_position({0,0})
    {}

    void SetEnvironments(const std::shared_ptr<ValueEnvironment>& valEnv, const std::shared_ptr<TypeEnvironment>& tyEnv)
    {
        if (!valEnv || !tyEnv)
        {
            throw CompilerErrorException("Attempt to set internal environment to invalid value");
        }

        m_valueEnvironment = valEnv;
        m_typeEnvironment = tyEnv;
    }

    virtual std::shared_ptr<ValueEnvironment>& UseValueEnvironment()
    {
        return m_valueEnvironment;
    }

    virtual std::shared_ptr<TypeEnvironment>& UseTypeEnvironment()
    {
        return m_typeEnvironment;
    }

    virtual std::shared_ptr<CompileTimeErrorReporter>& UseErrorReporter();

    virtual std::shared_ptr<WarningReporter>& UseWarningReporter();

    void SetStaticErrorReporters(const std::shared_ptr<CompileTimeErrorReporter>& errReporter, const std::shared_ptr<WarningReporter>& warningReporter);

    virtual void ReportTypeError(ErrorCode errorCode, const SupplementalErrorMsg& message = "");

protected:
    virtual void SetPosition(const Position& position)
    {
        m_position = position;
    }

private:
    Position m_position;

    std::shared_ptr<ValueEnvironment> m_valueEnvironment;
    std::shared_ptr<TypeEnvironment> m_typeEnvironment;

    // TODO: this is hacky. These should be wired up correctly, not static.
    static std::shared_ptr<CompileTimeErrorReporter> m_errorReporter;
    static std::shared_ptr<WarningReporter> m_warningReporter;
};

struct Expression : public AstNode {};

struct Var : public AstNode {};

struct Declaration : public AstNode {};

struct TypeNode : public AstNode {};

class Program
{
public:
    Program(
            std::unique_ptr<Expression>&& expr,
            const std::shared_ptr<ValueEnvironment>& valEnv,
            const std::shared_ptr<TypeEnvironment>& typeEnv,
            const std::shared_ptr<CompileTimeErrorReporter>& errorReporter,
            const std::shared_ptr<WarningReporter>& warningReporter)
        : m_expression(std::move(expr))
        , m_valueEnvironment(valEnv)
        , m_typeEnvironment(typeEnv)
        , m_errorReporter(errorReporter)
        , m_warningReporter(warningReporter)
    {
        m_expression->SetStaticErrorReporters(errorReporter, warningReporter);
    }

    Program(
            std::unique_ptr<Expression>&& expr,
            const std::shared_ptr<CompileTimeErrorReporter>& errorReporter,
            const std::shared_ptr<WarningReporter>& warningReporter)
        : Program(std::move(expr)
        , ValueEnvironment::GenerateBaseValueEnvironment()
        , TypeEnvironment::GenerateBaseTypeEnvironment()
        , errorReporter
        , warningReporter)
    {
    }

    const Expression& UseExpression()
    {
        return *m_expression;
    }

    Type TypeCheck();

private:
    std::unique_ptr<Expression> m_expression;
    std::shared_ptr<ValueEnvironment> m_valueEnvironment;
    std::shared_ptr<TypeEnvironment> m_typeEnvironment;
    std::shared_ptr<CompileTimeErrorReporter> m_errorReporter;
    std::shared_ptr<WarningReporter> m_warningReporter;
};

struct SimpleVar
    : public Var
{
    Symbol symbol;
    SimpleVar(const Symbol& sym, const Position& pos)
        : symbol(sym) 
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct FieldVar
    : public Var
{
    Symbol symbol;
    std::unique_ptr<Var> var;
    FieldVar(const Symbol& sym, std::unique_ptr<Var>&& invar, const Position& pos)
        : symbol(sym)
        , var(std::move(invar))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct SubscriptVar
    : public Var
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression; 
    SubscriptVar(std::unique_ptr<Var>&& inVar, std::unique_ptr<Expression>&& exp, const Position& pos)
        : var(std::move(inVar))
        , expression(std::move(exp)) 
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct VarExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    VarExpression(std::unique_ptr<Var>&& inVar, const Position& pos)
        : var(std::move(inVar)) 
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct NilExpression
    : public Expression
{ 
    NilExpression(const Position& pos)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct IntExpression
    : public Expression
{
    int value;
    IntExpression(int val, const Position& pos)
        : value(val)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct StringExpression
    : public Expression
{
    std::string value;
    StringExpression(const std::string& val, const Position& pos)
        : value(val)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct CallExpression
    : public Expression
{
    Symbol function;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpression(const Symbol& fn, std::vector<std::unique_ptr<Expression>>&& ars, const Position& pos)
        : function(fn)
        , args(std::move(ars))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct OpExpression
    : public Expression
{
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    BinOp op;
    OpExpression(std::unique_ptr<Expression>&& ls, std::unique_ptr<Expression>&& rs, BinOp oper, const Position& pos)
        : lhs(std::move(ls))
        , rhs(std::move(rs))
        , op(oper)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct FieldExp
{
    Symbol field;
    std::unique_ptr<Expression> expr;
    Position position;

    FieldExp(const Symbol& fld, std::unique_ptr<Expression>&& ex, const Position& pos)
        : field(fld)
        , expr(std::move(ex))
        , position(pos)
    {}
};

struct RecordExpression
    : public Expression
{
    Symbol type;
    std::vector<FieldExp> fields;
    RecordExpression(const Symbol& ty, std::vector<FieldExp>&& flds, const Position& pos)
        : type(ty)
        , fields(std::move(flds))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct SeqExpression
    : public Expression
{
    std::vector<std::unique_ptr<Expression>> expressions;
    SeqExpression(std::vector<std::unique_ptr<Expression>>&& expr, const Position& pos)
        : expressions(std::move(expr))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct AssignmentExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression;
    AssignmentExpression(std::unique_ptr<Var>&& v, std::unique_ptr<Expression> expr, const Position& pos)
        : var(std::move(v))
        , expression(std::move(expr))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct IfExpression
    : public Expression
{
    std::unique_ptr<Expression> test;
    std::unique_ptr<Expression> thenBranch;
    std::unique_ptr<Expression> elseBranch;

    IfExpression(
            std::unique_ptr<Expression>&& t,
            std::unique_ptr<Expression>&& thn,
            std::unique_ptr<Expression>&& els,
            const Position& pos)
        : test(std::move(t))
        , thenBranch(std::move(thn))
        , elseBranch(std::move(els))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct WhileExpression
    : public Expression
{
    std::unique_ptr<Expression> test;
    std::unique_ptr<Expression> body;

    WhileExpression(std::unique_ptr<Expression>&& t, std::unique_ptr<Expression>&& b, const Position& pos)
        : test(std::move(t))
        , body(std::move(b))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct ForExpression
    : public Expression
{
    Symbol var;
    bool escape;
    std::unique_ptr<Expression> low;
    std::unique_ptr<Expression> high;
    std::unique_ptr<Expression> body;

    ForExpression(const Symbol& v, std::unique_ptr<Expression>&& l, std::unique_ptr<Expression>&& h, std::unique_ptr<Expression>&& b, const Position& pos)
        : var(v)
        , escape(true)
        , low(move(l))
        , high(move(h))
        , body(move(b))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct BreakExpression
    : public Expression
{
    BreakExpression(const Position& pos)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct LetExpression
    : public Expression
{
    std::vector<std::unique_ptr<Declaration>> decls;
    std::unique_ptr<Expression> body;

    LetExpression(std::vector<std::unique_ptr<Declaration>>&& decs, std::unique_ptr<Expression>&& bdy, const Position& pos)
        : decls(std::move(decs))
        , body(std::move(bdy))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct ArrayExpression
    : public Expression
{
    Symbol type;
    std::unique_ptr<Expression> size;
    std::unique_ptr<Expression> init;

    ArrayExpression(const Symbol& id, std::unique_ptr<Expression>&& sz, std::unique_ptr<Expression>&& val, const Position& pos)
        : type(id)
        , size(std::move(sz))
        , init(std::move(val))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct FunDec
{
    Symbol name;
    std::vector<Field> fields;
    boost::optional<Symbol> resultTy;
    std::unique_ptr<Expression> body;
    Position position;

    FunDec(const Symbol& nam, std::vector<Field>&& flds, boost::optional<Symbol> ty, std::unique_ptr<Expression>&& bdy, const Position& pos)
        : name(nam)
        , fields(std::move(flds))
        , resultTy(ty)
        , body(std::move(bdy))
        , position(pos)
    {
    }
};

struct FunctionDeclaration
    : public Declaration
{
    std::vector<FunDec> decls;

    FunctionDeclaration(std::vector<FunDec>&& decs, const Position& pos)
        : decls(std::move(decs))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct VarDeclaration
    : public Declaration
{
   Symbol name;
   bool escape;
   boost::optional<Symbol> type;
   std::unique_ptr<Expression> init;

   VarDeclaration(const Symbol& id, boost::optional<Symbol> ty, std::unique_ptr<Expression>&& it, const Position& pos)
    : name(id)
    , escape(true)
    , type(ty)
    , init(std::move(it))
    {
        SetPosition(pos);
    }

    Type TypeCheck() override;
};

struct TyDec
{
    Symbol name;
    std::unique_ptr<TypeNode> type;
    Position position;
    TyDec(const Symbol& id, std::unique_ptr<TypeNode>&& ty, const Position& pos)
        : name(id)
        , type(std::move(ty))
        , position(pos)
    {}
};

struct TypeDeclaration
    : public Declaration
{
    std::vector<TyDec> types;

    TypeDeclaration(std::vector<TyDec>&& ty, const Position& pos)
        : types(std::move(ty))
    {
        SetPosition(pos);
    }

    Type TypeCheck() override;
};

struct NameType
    : public TypeNode
{
    Symbol name;

    NameType(const Symbol& id, const Position& pos)
        : name(id)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct RecordType
    : public TypeNode
{
    std::vector<Field> fields;

    RecordType(std::vector<Field>&& flds, const Position& pos)
        : fields(std::move(flds))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

struct ArrayType
    : public TypeNode
{
    Symbol name;

    ArrayType(const Symbol& id, const Position& pos)
        : name(id)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
};

}
