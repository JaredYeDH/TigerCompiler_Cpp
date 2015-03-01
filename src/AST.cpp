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
    auto ty = UseValueEnvironment()->LookUp(symbol);
    if (!ty)
    {
        std::string error = "Unidentified variable " + symbol.UseName();
        ReportError(error);
    }
    if ((*ty)->IsFunction())
    {
        // TODO: check params
        //const auto& formals = (*ty)->UseFormals();
        throw CompilerErrorException("type checking simplevar function");
        return (*ty)->GetType();
    }
    else
    {
        Type t = (*ty)->GetType();
        return Types::StripLeadingNameTypes(t);
    }
}

Type FieldVar::TypeCheck()
{
    // var must be of type record
    // symbol must be in record
    var->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    Type varTy = var->TypeCheck();
    auto ty = Types::GetFieldFromRecord(varTy, symbol);
    if (!ty)
    {
        std::string msg = "attempt to get field of non-record or use of non-existant field";
        ReportError(msg);
    }
    return *ty;
}

Type SubscriptVar::TypeCheck()
{
    // var must be of type array
    // expression must be of type int
    var->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    expression->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());

    Type varTy = var->TypeCheck();
    Type expTy = expression->TypeCheck();

    if (!Types::IsArrayType(varTy))
    {
        std::string msg = "Attempt to subscript non-array type";
        ReportError(msg);
    }
    else if (!AreEqualTypes(TypeFactory::MakeIntType(), expTy))
    {
        std::string msg = "Attempt to subscript with non-int";
        ReportError(msg);
    }

    auto ty = Types::GetTypeOfArray(varTy);
    if (!ty)
    {
        std::string msg = "Attempt to get type of array of non-array type";
        ReportError(msg);
    }
    return *ty;
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
    auto symTyRaw = UseValueEnvironment()->LookUp(function);

    if (!symTyRaw)
    {
        std::string msg = "attempt to call non-existant function";
        ReportError(msg);
    }

    if (!(*symTyRaw)->IsFunction())
    {
        std::string msg = "attempt to call non function";
        ReportError(msg);
    }

    const auto& formals = (*symTyRaw)->UseFormals();
    Type ty = (*symTyRaw)->GetType();
    if (formals.size() != args.size())
    {
        std::string msg = "Type of arguments do not match";
        ReportError(msg);
    }

    for (unsigned int i = 0 ; i < args.size(); ++i)
    {
        args[i]->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());

        Type argTy = args[i]->TypeCheck();
        if (!AreEqualTypes(argTy, formals[i]))
        {
            std::string msg = "Type of argument does not match";
            ReportError(msg);
        }
    }

    return ty;
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
    // TODO: implement
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
        decl->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        decl->TypeCheck();
    }
    Type t = body->TypeCheck();

    UseValueEnvironment()->EndScope();
    UseTypeEnvironment()->EndScope();
    return t;
}

Type ArrayExpression::TypeCheck()
{
    // TODO: implement
    throw CompilerErrorException("Not Implemented");
}

Type FunctionDeclaration::TypeCheck()
{
    // TODO: implement
    throw CompilerErrorException("Not Implemented");
}

Type VarDeclaration::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
   
    init->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    Type experTy = init->TypeCheck();
    
    if (type)
    {
        auto ty = UseTypeEnvironment()->LookUp(*type);
        if (!ty)
        {
            std::string message = "Use of non-existant type in type annotation in var declaration";
            ReportError(message);
        }
        else if (!AreEqualTypes(experTy, ty->UseType()))
        {
            std::string message = "Un-matched types in var declaration";
            ReportError(message);
        }
    }
    else if (AreEqualTypes(experTy, TypeFactory::MakeNilType()))
    {
        std::string message = "Must use long form of var dec if init expression is nil";
        ReportError(message);
    }
    
    UseValueEnvironment()->Insert(name, std::make_shared<VarEntry>(experTy));

    return TypeFactory::MakeUnitType();
}

Type TypeDeclaration::TypeCheck()
{
    // TODO: implement
    throw CompilerErrorException("Not Implemented");
}

Type NameType::TypeCheck()
{
    // TODO: implement
    throw CompilerErrorException("Not Implemented");
}

Type RecordType::TypeCheck()
{
    // TODO: implement
    throw CompilerErrorException("Not Implemented");
}

Type ArrayType::TypeCheck()
{
    // TODO: implement
    throw CompilerErrorException("Not Implemented");
}
