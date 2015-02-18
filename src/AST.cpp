#include "AST.h"

using namespace AST;

void AstNode::ReportError(std::string& message)
{
    throw SemanticAnalysisException(message.c_str());
}

Type Program::TypeCheck()
{
    m_expression->SetEnvironments(m_valueEnvironment, m_typeEnvironment);
    return m_expression->TypeCheck();
}

Type SimpleVar::TypeCheck()
{
/*    auto ty = UseValueEnvironment().LookUp(symbol);
    if (!ty)
    {
        std::string error = "Unidentified variable " + symbol.UseName();
        ReportError(error);
    }
    Type t = (*ty)->GetType();
    return Types::StripLeadingNameTypes(t);
*/
    throw CompilerErrorException("Not Implemented");
}

Type FieldVar::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type SubscriptVar::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type VarExpression::TypeCheck()
{ 
    return var->TypeCheck();
}

Type NilExpression::TypeCheck()
{
    return TypeFactory::MakeNilType();
}

Type IntExpression::TypeCheck()
{
    return TypeFactory::MakeIntType();
}

Type StringExpression::TypeCheck()
{
    return TypeFactory::MakeStringType();
}

Type CallExpression::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type OpExpression::TypeCheck()
{
    lhs->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    rhs->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    auto leftType = lhs->TypeCheck();
    auto rightType = rhs->TypeCheck();
    if (!AreEqualTypes(leftType, rightType))
    {
        std::string message = "Left and right types do not match";
        ReportError(message);
    }

    if (!AreEqualTypes(leftType, TypeFactory::MakeIntType())
        && !AreEqualTypes(leftType, TypeFactory::MakeStringType()))
    {
        std::string message = "Attempt to use binary op on non int or string";
        ReportError(message);
    }
    return leftType;
}

Type RecordExpression::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type SeqExpression::TypeCheck()
{
    Type t;
    for (auto& expr : expressions)
    {
        expr->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        t = expr->TypeCheck();
    }
    return t;
}

Type AssignmentExpression::TypeCheck()
{
    var->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    expression->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());

    Type varType = var->TypeCheck();
    Type expType = expression->TypeCheck();
    
    if (!AreEqualTypes(varType, expType))
    {
        std::string message = "Type mismatch in AssignmentExpression";
        ReportError(message);
    }

    return TypeFactory::MakeUnitType();
}

Type IfExpression::TypeCheck()
{
    test->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    thenBranch->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    Type testTy = test->TypeCheck();
    if (!AreEqualTypes(testTy, TypeFactory::MakeIntType()))
    {
        std::string message = "If condition of non-int type";
        ReportError(message);
    }

    Type thenTy = thenBranch->TypeCheck();
    if (elseBranch)
    {
        elseBranch->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        if (!AreEqualTypes(thenTy, elseBranch->TypeCheck()))
        {
            std::string msg = "If-then-else where then and else branch types do not match";
            ReportError(msg);
        }
        return thenTy;
    }
    else
    {
        return TypeFactory::MakeUnitType();
    }

}

Type WhileExpression::TypeCheck()
{
    test->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    body->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    if (!AreEqualTypes(TypeFactory::MakeIntType(), test->TypeCheck()))
    {
        std::string message = "while expression with non-integer test";
        ReportError(message);
    }
    body->TypeCheck();
    return TypeFactory::MakeUnitType();
}

Type ForExpression::TypeCheck()
{
    low->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    high->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    Type lTy = low->TypeCheck();
    Type hTy = high->TypeCheck();
    if (!AreEqualTypes(lTy, hTy) && !AreEqualTypes(lTy, TypeFactory::MakeIntType()))
    {
        std::string msg = "for expression with ranges not of int type";
        ReportError(msg);
    }

    UseValueEnvironment()->BeginScope();
    {
        UseValueEnvironment()->Insert(var, std::make_shared<VarEntry>(lTy));
        body->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        Type unit = TypeFactory::MakeUnitType();
        if (!AreEqualTypes(unit, body->TypeCheck()))
        {
            std::string msg = "for expression with body of non unit type";
            ReportError(msg);
        }
    }
    UseValueEnvironment()->EndScope();
    return TypeFactory::MakeUnitType();
}

Type BreakExpression::TypeCheck()
{
    return TypeFactory::MakeUnitType();
}

Type LetExpression::TypeCheck()
{
    UseTypeEnvironment()->BeginScope();
    UseValueEnvironment()->BeginScope();
    
    for (auto& decl : decls)
    {
        decl->TypeCheck();
    }
    Type t = body->TypeCheck();

    UseValueEnvironment()->EndScope();
    UseTypeEnvironment()->EndScope();
    return t;
}

Type ArrayExpression::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type FunctionDeclaration::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type VarDeclaration::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type TypeDeclaration::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type NameType::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type RecordType::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}

Type ArrayType::TypeCheck()
{
    throw CompilerErrorException("Not Implemented");
}
