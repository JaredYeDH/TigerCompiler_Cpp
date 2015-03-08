#include "AST.h"

using namespace AST;

std::shared_ptr<CompileTimeErrorReporter> AstNode::m_errorReporter;
std::shared_ptr<WarningReporter> AstNode::m_warningReporter;

void AstNode::SetStaticErrorReporters(const std::shared_ptr<CompileTimeErrorReporter>& errReporter, const std::shared_ptr<WarningReporter>& warningReporter)
{
    m_errorReporter = errReporter;
    m_warningReporter = warningReporter;
}

void AstNode::ReportTypeError(const std::string message)
{
    Error err { ErrorType::TypeError, UsePosition(), message };
    UseErrorReporter()->AddError(err);
}

std::shared_ptr<CompileTimeErrorReporter>& AstNode::UseErrorReporter()
{
    return m_errorReporter;
}

std::shared_ptr<WarningReporter>& AstNode::UseWarningReporter()
{
    return m_warningReporter;
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
        ReportTypeError(error);
        // error correct
        ty = std::make_shared<VarEntry>(TypeFactory::MakeIntType());
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
        ReportTypeError(msg);
        // error correct
        ty = TypeFactory::MakeIntType();
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
        ReportTypeError(msg);
    }
    else if (!AreEqualTypes(TypeFactory::MakeIntType(), expTy))
    {
        std::string msg = "Attempt to subscript with non-int";
        ReportTypeError(msg);
    }

    auto ty = Types::GetTypeOfArray(varTy);
    if (!ty)
    {
        std::string msg = "Attempt to get type of array of non-array type";
        ReportTypeError(msg);
        // error correct
        ty = TypeFactory::MakeArrayType(TypeFactory::MakeIntType());
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
        ReportTypeError(msg);
        // error correct
        symTyRaw = std::make_shared<FunEntry>(std::vector<Type>{}, TypeFactory::MakeIntType());
    }

    if (!(*symTyRaw)->IsFunction())
    {
        std::string msg = "attempt to call non function";
        ReportTypeError(msg);
    }

    const auto& formals = (*symTyRaw)->UseFormals();
    Type ty = (*symTyRaw)->GetType();
    if (formals.size() != args.size())
    {
        std::string msg = "Type of arguments do not match";
        ReportTypeError(msg);
        // error correct
        return ty;
    }

    for (unsigned int i = 0 ; i < args.size(); ++i)
    {
        args[i]->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());

        Type argTy = args[i]->TypeCheck();
        if (!AreEqualTypes(argTy, formals[i]))
        {
            std::string msg = "Type of argument does not match";
            ReportTypeError(msg);
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
        ReportTypeError(message);
    }

    if (!AreEqualTypes(leftType, TypeFactory::MakeIntType())
        && !AreEqualTypes(leftType, TypeFactory::MakeStringType()))
    {
        std::string message = "Attempt to use binary op on non int or string";
        ReportTypeError(message);
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
        ReportTypeError(msg);
        // error correct
        expectTyWrapper = EnvType(TypeFactory::MakeIntType());
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
        ReportTypeError(errorMessage);
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
        ReportTypeError(message);
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
        ReportTypeError(message);
    }

    Type thenTy = thenBranch->TypeCheck();
    if (elseBranch)
    {
        elseBranch->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        if (!AreEqualTypes(thenTy, elseBranch->TypeCheck()))
        {
            std::string msg = "If-then-else where then and else branch types do not match";
            ReportTypeError(msg);
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
        ReportTypeError(message);
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
        ReportTypeError(msg);
    }

    UseValueEnvironment()->BeginScope();
    {
        bool shadowed;
        UseValueEnvironment()->Insert(var, std::make_shared<VarEntry>(lTy), shadowed);
        if (shadowed)
        {
            ErrorMsg err("Shadowing of " + var.UseName());
            UseWarningReporter()->AddWarning({UsePosition(), err, WarningLevel::Low});
        }
        body->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        Type unit = TypeFactory::MakeUnitType();
        if (!AreEqualTypes(unit, body->TypeCheck()))
        {
            std::string msg = "for expression with body of non unit type";
            ReportTypeError(msg);
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
        ReportTypeError(msg);
    }
    
    auto tyTy = UseTypeEnvironment()->LookUp(type);
    if (!tyTy)
    {
        std::string msg = "Attempt to use non-existant type for type of array";
        ReportTypeError(msg);
    }

    if (!AreEqualTypes(tyTy->UseType(), initTy))
    {
        std::string msg = "Init for array does not match type of array";
        ReportTypeError(msg);
    }

    // TODO: what type is an array expression?
    return TypeFactory::MakeNilType();
}

// Will not add the funentry to the environment. This promises to leave the
// environments as they where when you passed them in when it's done.
std::shared_ptr<FunEntry> CheckFunctionDecl(const std::shared_ptr<CompileTimeErrorReporter>& errorReporter, const std::shared_ptr<WarningReporter>& warningReporter, std::shared_ptr<ValueEnvironment>& valEnv, std::shared_ptr<TypeEnvironment>& tyEnv, const FunDec& decl)
{
    // vector<Field> fields
    // optional<Symbol> resultTy
    // unique_ptr<Expression> body

    valEnv->BeginScope();
    tyEnv->BeginScope();

    std::vector<Type> formals;

    for (const Field& field : decl.fields)
    {
        auto formalType = tyEnv->LookUp(field.type);
        if (!formalType)
        {
            std::stringstream msg;
            msg << "Formal function argument " <<  field.name.UseName() << " is of non-existant type";
            Error err { ErrorType::TypeError, decl.position, msg.str()};
            errorReporter->AddError(err);
            // error correcting
            formalType = EnvType{TypeFactory::MakeIntType()};
        }
    
        bool shadowed;
        valEnv->Insert(field.name, std::make_shared<VarEntry>(formalType->UseType()), shadowed);
        if (shadowed)
        {
            ErrorMsg err("Shadowing of " + field.name.UseName());
            warningReporter->AddWarning({field.position, err, WarningLevel::Low});
        }

        formals.push_back(formalType->UseType());
    }

    decl.body->SetEnvironments(valEnv, tyEnv);
    Type actualType = decl.body->TypeCheck();
    if (decl.resultTy)
    {
        auto resultTy = tyEnv->LookUp(*decl.resultTy);
        if (!resultTy)
        {
            Error err { ErrorType::TypeError, decl.position, "Listed result type of function is non-existant"};
            errorReporter->AddError(err);
 
        }
        if (!AreEqualTypes(resultTy->UseType(), actualType))
        {
            Error err { ErrorType::TypeError, decl.position, "Actual type of function does not match listed"};
            errorReporter->AddError(err);
 

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
        std::shared_ptr<FunEntry> fun = CheckFunctionDecl(UseErrorReporter(), UseWarningReporter(), UseValueEnvironment(), UseTypeEnvironment(), decl);

        bool shadowed;
        UseValueEnvironment()->Insert(decl.name, fun, shadowed);
        if (shadowed)
        {
            ErrorMsg err("Shadowing of " + decl.name.UseName());
            UseWarningReporter()->AddWarning({decl.position, err, WarningLevel::Low});
        }

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
            ReportTypeError(message);
        }
        else if (!AreEqualTypes(experTy, ty->UseType()))
        {
            std::string message = "Un-matched types in var declaration";
            ReportTypeError(message);
        }
    }
    else if (AreEqualTypes(experTy, TypeFactory::MakeNilType()))
    {
        std::string message = "Must use long form of var dec if init expression is nil";
        ReportTypeError(message);
    }
    
    bool shadowed;
    UseValueEnvironment()->Insert(name, std::make_shared<VarEntry>(experTy), shadowed);
    if (shadowed)
    {
        ErrorMsg err("Shadowing of " + name.UseName());
        UseWarningReporter()->AddWarning({UsePosition(), err, WarningLevel::Low});
    }

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

        bool shadowed;
        UseTypeEnvironment()->Insert(tyDec.name, ty, shadowed);
        if (shadowed)
        {
            ErrorMsg err("Shadowing of " + tyDec.name.UseName());
            UseWarningReporter()->AddWarning({tyDec.position, err, WarningLevel::Low});
        }
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
            ReportTypeError(msg);
            // error correct
            ty = TypeFactory::MakeIntType();
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
        ReportTypeError(msg);
    }
    Type type = ty->UseType();
    return TypeFactory::MakeArrayType(type);
}
