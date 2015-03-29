#include "AST.h"

using namespace AST;

std::shared_ptr<CompileTimeErrorReporter> AstNode::m_errorReporter;
std::shared_ptr<WarningReporter> AstNode::m_warningReporter;

void AstNode::SetStaticErrorReporters(const std::shared_ptr<CompileTimeErrorReporter>& errReporter, const std::shared_ptr<WarningReporter>& warningReporter)
{
    m_errorReporter = errReporter;
    m_warningReporter = warningReporter;
}

void AstNode::ReportTypeError(ErrorCode errorCode, const SupplementalErrorMsg& message)
{
    Error err { errorCode, UsePosition(), message };
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
        ReportTypeError(ErrorCode::Err0, symbol.UseName());
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
        ReportTypeError(ErrorCode::Err1, "");
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
        ReportTypeError(ErrorCode::Err2, "");
    }
    else if (!AreEqualTypes(TypeFactory::MakeIntType(), expTy))
    {
        ReportTypeError(ErrorCode::Err3, "");
    }

    auto ty = Types::GetTypeOfArray(varTy);
    if (!ty)
    {
        ReportTypeError(ErrorCode::Err4);
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
        ReportTypeError(ErrorCode::Err5);
        // error correct
        symTyRaw = std::make_shared<FunEntry>(std::vector<Type>{}, TypeFactory::MakeIntType());
    }

    if (!(*symTyRaw)->IsFunction())
    {
        ReportTypeError(ErrorCode::Err6);
    }

    const auto& formals = (*symTyRaw)->UseFormals();
    Type ty = (*symTyRaw)->GetType();
    if (formals.size() != args.size())
    {
        ReportTypeError(ErrorCode::Err7);
        // error correct
        return ty;
    }

    for (unsigned int i = 0 ; i < args.size(); ++i)
    {
        args[i]->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());

        Type argTy = args[i]->TypeCheck();
        if (!AreEqualTypes(argTy, formals[i]))
        {
            ReportTypeError(ErrorCode::Err8);
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
        ReportTypeError(ErrorCode::Err9);
    }

    if (!AreEqualTypes(leftType, TypeFactory::MakeIntType())
        && !AreEqualTypes(leftType, TypeFactory::MakeStringType()))
    {
        ReportTypeError(ErrorCode::Err10);
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
        ReportTypeError(ErrorCode::Err11);
        // error correct
        expectTyWrapper = std::make_shared<EnvType>(TypeFactory::MakeIntType());
    }
    Type expectedType = (*expectTyWrapper)->UseType();

    RecordTy fieldTypes;
    for (const auto& field : fields)
    {
        field.expr->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        Type ty = field.expr->TypeCheck();
        fieldTypes.push_back({field.field, ty});
    }

    ErrorCode errorCode;
    std::string errorMessage;
    if (!Types::IsRecordTypeWithMatchingFields(expectedType, fieldTypes, errorCode, errorMessage))
    {
        ReportTypeError(errorCode, errorMessage);
    }

    // TODO: Do we need to do ID matching here?
    return TypeFactory::MakeRecordType(fieldTypes);
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
        ReportTypeError(ErrorCode::Err12);
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
        ReportTypeError(ErrorCode::Err13);
    }

    Type thenTy = thenBranch->TypeCheck();
    if (elseBranch)
    {
        elseBranch->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        if (!AreEqualTypes(thenTy, elseBranch->TypeCheck()))
        {
            ReportTypeError(ErrorCode::Err14);
        }
        return thenTy;
    }
    else
    {
        if (!AreEqualTypes(TypeFactory::MakeUnitType(), thenTy))
        {
            ReportTypeError(ErrorCode::Err68);
        }
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
        ReportTypeError(ErrorCode::Err15);
    }
    auto bodyTy = body->TypeCheck();
    if (!AreEqualTypes(TypeFactory::MakeUnitType(), bodyTy))
    {
        ReportTypeError(ErrorCode::Err66);
    }
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
    if (!AreEqualTypes(lTy, hTy) || !AreEqualTypes(lTy, TypeFactory::MakeIntType()) || !AreEqualTypes(hTy, TypeFactory::MakeIntType()))
    {
        ReportTypeError(ErrorCode::Err16, "");
    }

    UseValueEnvironment()->BeginScope();
    {
        bool shadowed;
        UseValueEnvironment()->Insert(var, std::make_shared<VarEntry>(lTy), shadowed);
        if (shadowed)
        {
            std::string err("Shadowing of " + var.UseName());
            UseWarningReporter()->AddWarning({UsePosition(), err, WarningLevel::Low});
        }
        body->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        Type unit = TypeFactory::MakeUnitType();
        if (!AreEqualTypes(unit, body->TypeCheck()))
        {
            ReportTypeError(ErrorCode::Err17, "");
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
        ReportTypeError(ErrorCode::Err18, "");
    }
    
    auto tyTy = UseTypeEnvironment()->LookUp(type);
    if (!tyTy)
    {
        ReportTypeError(ErrorCode::Err19, "");
    }

    if (!AreEqualTypes((*tyTy)->UseType(), initTy))
    {
        ReportTypeError(ErrorCode::Err20, "");
    }

    // TODO: Do we need to do anythin with id matching?
    return TypeFactory::MakeArrayType(initTy);
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
            Error err { ErrorCode::Err30, decl.position, field.name.UseName()};
            errorReporter->AddError(err);
            // error correcting
            formalType = std::make_shared<EnvType>(TypeFactory::MakeIntType());
        }
    
        bool shadowed;
        valEnv->Insert(field.name, std::make_shared<VarEntry>((*formalType)->UseType()), shadowed);
        if (shadowed)
        {
            std::string err("Shadowing of " + field.name.UseName());
            warningReporter->AddWarning({field.position, err, WarningLevel::Low});
        }

        formals.push_back((*formalType)->UseType());
    }

    decl.body->SetEnvironments(valEnv, tyEnv);
    Type actualType = decl.body->TypeCheck();
    if (decl.resultTy)
    {
        auto resultTy = tyEnv->LookUp(*decl.resultTy);
        if (!resultTy)
        {
            Error err { ErrorCode::Err31, decl.position, ""};
            errorReporter->AddError(err);
 
        }
        if (!AreEqualTypes((*resultTy)->UseType(), actualType))
        {
            Error err { ErrorCode::Err32, decl.position, ""};
            errorReporter->AddError(err);
        }
    }
    else if (!AreEqualTypes(TypeFactory::MakeUnitType(), actualType))
    { 
        Error err { ErrorCode::Err70, decl.position, ""};
        errorReporter->AddError(err);
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
            std::string err("Shadowing of " + decl.name.UseName());
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
            ReportTypeError(ErrorCode::Err21, "");
        }
        else if (!AreEqualTypes(experTy, (*ty)->UseType()))
        {
            ReportTypeError(ErrorCode::Err22, "");
        }
    }
    else if (AreEqualTypes(experTy, TypeFactory::MakeNilType()))
    {
        ReportTypeError(ErrorCode::Err23, "");
    }
    
    bool shadowed;
    UseValueEnvironment()->Insert(name, std::make_shared<VarEntry>(experTy), shadowed);
    if (shadowed)
    {
        std::string err("Shadowing of " + name.UseName());
        UseWarningReporter()->AddWarning({UsePosition(), err, WarningLevel::Low});
    }

    return TypeFactory::MakeUnitType();
}

Type TypeDeclaration::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
    
    // initial pass for headers. This handles recursive types
    for (const TyDec& tyDec : types)
    {
        tyDec.type->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        
        bool shadowed;
        UseTypeEnvironment()->Insert(tyDec.name, std::make_shared<EnvType>(TypeFactory::MakeEmptyNameType(tyDec.name)), shadowed);
        if (shadowed)
        {
            std::string err("Shadowing of " + tyDec.name.UseName());
            UseWarningReporter()->AddWarning({tyDec.position, err, WarningLevel::Low});
        }
    }
    
    // second pass to actually type check
    for (const TyDec& tyDec : types)
    {
        Type ty = tyDec.type->TypeCheck();
        auto envValue = UseTypeEnvironment()->LookUp(tyDec.name);
        if (!envValue || !Types::IsNameType((*envValue)->UseType()))
        {
            throw CompilerErrorException("Expected type to exist in type environment. This breaks recursive types");
        }
        (*envValue)->AddTypeToNameType(ty);
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
            ReportTypeError(ErrorCode::Err24, "");
            // error correct
            ty = std::make_shared<EnvType>(TypeFactory::MakeIntType());
        }
        record.push_back({field.name, (*ty)->UseType()});
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
        ReportTypeError(ErrorCode::Err25, "");
    }
    Type type = (*ty)->UseType();
    return TypeFactory::MakeArrayType(type);
}
