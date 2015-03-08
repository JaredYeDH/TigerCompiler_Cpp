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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    auto ty = UseValueEnvironment()->LookUp(symbol);
    if (!ty)
    {
        std::string error = "Unidentified variable " + symbol.UseName();
        ReportError(error);
    }
    if ((*ty)->IsFunction())
    {
        // TODO: check params
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
    
    var->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    return var->TypeCheck();
}

Type NilExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    return TypeFactory::MakeNilType();
}

Type IntExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    return TypeFactory::MakeIntType();
}

Type StringExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    return TypeFactory::MakeStringType();
}

Type CallExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    auto expectTyWrapper = UseTypeEnvironment()->LookUp(type);
    if (!expectTyWrapper)
    {
        std::string msg("attempt to create instance of record of non existing type");
        ReportError(msg);
    }
    Type expectedType = expectTyWrapper->UseType();

    RecordTy fieldTypes;
    for (const auto& field : fields)
    {
        field.expr->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        Type ty = field.expr->TypeCheck();
        fieldTypes.push_back({field.field, ty});
    }

    std::string errorMessage;
    if (!Types::IsRecordTypeWithMatchingFields(expectedType, fieldTypes, errorMessage))
    {
        ReportError(errorMessage);
    }

    // TODO: what type should this return?
    return TypeFactory::MakeUnitType();
}

Type SeqExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    Type t = TypeFactory::MakeUnitType();
    for (auto& expr : expressions)
    {
        expr->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        t = expr->TypeCheck();
    }
    return t;
}

Type AssignmentExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    return TypeFactory::MakeUnitType();
}

Type LetExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    UseTypeEnvironment()->BeginScope();
    UseValueEnvironment()->BeginScope();
    
    for (auto& decl : decls)
    {
        decl->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        decl->TypeCheck();
    }
    body->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    Type t = body->TypeCheck();

    UseValueEnvironment()->EndScope();
    UseTypeEnvironment()->EndScope();
    return t;
}

Type ArrayExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    // the typeof init must match type of type
    // size must be of type int
    // ??? return a new array type ???

    size->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    init->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());

    Type sizeTy = size->TypeCheck();
    Type initTy = init->TypeCheck();

    if (!AreEqualTypes(TypeFactory::MakeIntType(), sizeTy))
    {
        std::string msg = "Can not use non-init type for size of array";
        ReportError(msg);
    }
    
    auto tyTy = UseTypeEnvironment()->LookUp(type);
    if (!tyTy)
    {
        std::string msg = "Attempt to use non-existant type for type of array";
        ReportError(msg);
    }

    if (!AreEqualTypes(tyTy->UseType(), initTy))
    {
        std::string msg = "Init for array does not match type of array";
        ReportError(msg);
    }

    // TODO: what type is an array expression?
    return TypeFactory::MakeNilType();
}

// Will not add the funentry to the environment. This promises to leave the
// environments as they where when you passed them in when it's done.
std::shared_ptr<FunEntry> CheckFunctionDecl(std::shared_ptr<ValueEnvironment>& valEnv, std::shared_ptr<TypeEnvironment>& tyEnv, const FunDec& decl)
{
    // vector<Field> fields
    // optional<Symbol> resultTy
    // unique_ptr<Expression> body

    valEnv->BeginScope();
    tyEnv->BeginScope();

    std::vector<Type> formals;

    for (const Field& field : decl.fields)
    {
        auto formalType = tyEnv->LookUp(field.name);
        if (!formalType)
        {
            throw SemanticAnalysisException("Formal function argument is of non-existant type");
        }

        valEnv->Insert(field.name, std::make_shared<VarEntry>(formalType->UseType()));
        formals.push_back(formalType->UseType());
    }

    decl.body->SetEnvironments(valEnv, tyEnv);
    Type actualType = decl.body->TypeCheck();
    if (decl.resultTy)
    {
        auto resultTy = tyEnv->LookUp(*decl.resultTy);
        if (!resultTy)
        {
            throw SemanticAnalysisException("Listed result type of function is non-existant");
        }
        if (!AreEqualTypes(resultTy->UseType(), actualType))
        {
            throw SemanticAnalysisException("Actual type of function does not match listed");
        }
    }

    valEnv->EndScope();
    tyEnv->EndScope();
    
    return std::make_shared<FunEntry>(formals, actualType);
}

Type FunctionDeclaration::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());

    // TODO: Recursive decls
    for (const auto& decl : decls)
    {
        std::shared_ptr<FunEntry> fun = CheckFunctionDecl(UseValueEnvironment(), UseTypeEnvironment(), decl);
        UseValueEnvironment()->Insert(decl.name, fun);
    }
    return TypeFactory::MakeUnitType();
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
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
    
    for (const TyDec& tyDec : types)
    {
        tyDec.type->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        Type ty = tyDec.type->TypeCheck();
        UseTypeEnvironment()->Insert(tyDec.name, ty);
    }

    // TODO: does this return something different?
    return TypeFactory::MakeUnitType();
}

Type NameType::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    return TypeFactory::MakeEmptyNameType(name);
}

Type RecordType::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    RecordTy record;
    for (const auto& field: fields)
    {
        auto ty = UseTypeEnvironment()->LookUp(field.type);
        if (!ty)
        {
            std::string msg = "Attempt to make record with non existant type";
            ReportError(msg);
        }
        record.push_back({field.name, ty->UseType()});
    }
    return TypeFactory::MakeRecordType(record);
}

Type ArrayType::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
 
    auto ty = UseTypeEnvironment()->LookUp(name);
    if (!ty)
    {
        std::string msg = "Type checking arraytype with non bound name for type";
        ReportError(msg);
    }
    Type type = ty->UseType();
    return TypeFactory::MakeArrayType(type);
}
