#include "AST.h"
#include <set>

using namespace AST;
using namespace Translate;

std::shared_ptr<CompileTimeErrorReporter> AstNode::m_errorReporter;
std::shared_ptr<WarningReporter> AstNode::m_warningReporter;
uint8_t AstNode::m_loopScope;
std::shared_ptr<IEscapeCalculator> AstNode::m_escapecalc;
std::shared_ptr<Level> AstNode::m_currentLevel = std::make_shared<Level>(); // outermost

void AstNode::SetStaticErrorReporters(const std::shared_ptr<CompileTimeErrorReporter>& errReporter, const std::shared_ptr<WarningReporter>& warningReporter)
{
    m_errorReporter = errReporter;
    m_warningReporter = warningReporter;
}

void AstNode::SetStaticEscapeCalculator(const std::shared_ptr<IEscapeCalculator>& escapeCalc)
{
    m_escapecalc = escapeCalc;
}

void AstNode::ReportTypeError(ErrorCode errorCode, const SupplementalErrorMsg& message)
{
    Error err { errorCode, UsePosition(), message };
    UseErrorReporter()->AddError(err);
}

bool AstNode::InLoopScope() const
{
    return m_loopScope > 0;
}

void AstNode::EnterLoopScope()
{
    m_loopScope++;
}

void AstNode::ExitLoopScope()
{
    if (m_loopScope == 0)
    {
        throw CompilerErrorException("Attempt to leave loop scope when already completely out of loop scope");
    }
    m_loopScope--;
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
        ty = std::make_shared<VarEntry>(TypeFactory::MakeIntType(), UseLevel()->AllocateLocal(false));
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
        symTyRaw = std::make_shared<FunEntry>(std::vector<Type>{}, TypeFactory::MakeIntType(), UseLevel(), Temps::UseTempFactory().MakeLabel());
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
        if (Types::IsNameType(argTy))
        {
            auto actual = UseTypeEnvironment()->LookUp(Types::GetSymbolFromNameType(argTy));
            if (!actual)
            {
                throw CompilerErrorException("Use of non defined nametype in call expression");
            }
            argTy = (*actual)->UseType();
        }
        
        if (!AreEqualTypes(argTy, formals[i]))
        {
            std::stringstream ss;
            ss << "Argument " << i << " should be " << Types::TypeString(formals[i]) << " but saw " << Types::TypeString(argTy);
            ReportTypeError(ErrorCode::Err8, ss.str());
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

    if (op != BinOp::Eq && op != BinOp::Neq 
        && !AreEqualTypes(leftType, TypeFactory::MakeIntType())
        && !AreEqualTypes(leftType, TypeFactory::MakeStringType()))
    {
        ReportTypeError(ErrorCode::Err10);
    }
    if (op == BinOp::Eq || op == BinOp::Neq)
    {
        return TypeFactory::MakeIntType();
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

    ErrorCode errorCode;
    std::string errorMessage;
    if (!Types::FillNameTypes(expectedType, UseTypeEnvironment(), errorCode, errorMessage))
    {
        ReportTypeError(errorCode, errorMessage);
    }

    RecordTy fieldTypes;
    for (const auto& field : fields)
    {
        field.expr->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        Type ty = field.expr->TypeCheck();
        fieldTypes.push_back({field.field, ty});
    }

    if (!Types::IsRecordTypeWithMatchingFields(expectedType, fieldTypes, UseTypeEnvironment(), errorCode, errorMessage))
    {
        ReportTypeError(errorCode, errorMessage);
    }

    return expectedType;
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

    if (var->IsImmutable())
    {
        ReportTypeError(ErrorCode::Err67);
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
    EnterLoopScope();
    auto bodyTy = body->TypeCheck();
    ExitLoopScope();
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
    EnterLoopScope();
    {
        bool shadowed;
        UseValueEnvironment()->Insert(var, std::make_shared<VarEntry>(lTy, UseLevel()->AllocateLocal(escape)), shadowed, true /*immutable*/);
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
    ExitLoopScope();
    return TypeFactory::MakeUnitType();
}

Type BreakExpression::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());
    
    if (!InLoopScope())
    {
        ReportTypeError(ErrorCode::Err74);
    }
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

    if (!Types::IsArrayType((*tyTy)->UseType()))
    {
        ReportTypeError(ErrorCode::Err71, "");
    }

    auto typeOfArray = Types::GetTypeOfArray((*tyTy)->UseType());
    if (!typeOfArray || !AreEqualTypes(*typeOfArray, initTy))
    {
        ReportTypeError(ErrorCode::Err20, "");
    }

    return (*tyTy)->UseType();
}

// Will not add the funentry to the environment. This promises to leave the
// environments as they where when you passed them in when it's done.
Type FunDec::TypeCheck()
{
    // lookup in env
    auto funEntry = UseValueEnvironment()->LookUp(name);
    if (!funEntry || !*funEntry)
    {
        throw CompilerErrorException("Possibly recursive function was not added to environment in header pass");
    }

    if (!(*funEntry)->IsFunction())
    {
        throw CompilerErrorException("Checking function decl on non function type");
    }

    UseValueEnvironment()->BeginScope();
    UseTypeEnvironment()->BeginScope();

    // bring all formals into scope
    for (uint32_t i = 0; i < (*funEntry)->UseFormals().size(); ++i)
    {
        const auto& formal = (*funEntry)->UseFormals()[i];
        bool shadowed;
        auto field = fields[i];
        UseValueEnvironment()->Insert(field.name, std::make_shared<VarEntry>(formal, UseLevel()->AllocateLocal(field.escape)), shadowed);
        if (shadowed)
        {
            std::string err("Shadowing of " + field.name.UseName());
            UseWarningReporter()->AddWarning({field.position, err, WarningLevel::Low});
        }
    }

    body->SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
    Type actualType = body->TypeCheck();
    if (resultTy)
    {
        auto actualResultTy = UseTypeEnvironment()->LookUp(*resultTy);
        if (!actualResultTy || !*actualResultTy)
        {
            Error err { ErrorCode::Err31, position, ""};
            UseErrorReporter()->AddError(err);
        }
        if (!AreEqualTypes((*actualResultTy)->UseType(), actualType))
        {
            Error err { ErrorCode::Err32, position, ""};
            UseErrorReporter()->AddError(err);
        }
    }
    else if (!AreEqualTypes(TypeFactory::MakeUnitType(), actualType))
    { 
        Error err { ErrorCode::Err70, position, ""};
        UseErrorReporter()->AddError(err);
    }

    UseValueEnvironment()->EndScope();
    UseTypeEnvironment()->EndScope();

    if (!AreEqualTypes(actualType, (*funEntry)->GetType()))
    {
        Error err {ErrorCode::Err32, position, ""};
        UseErrorReporter()->AddError(err);
        // no error correcting here, go with the listed type
    }
    
    return actualType;
}

std::shared_ptr<FunEntry> FunDec::CalculateHeader()
{
    Type ty;
    if (resultTy)
    {
        auto resTy = UseTypeEnvironment()->LookUp(*resultTy);
        if (!resTy)
        {
            Error err {ErrorCode::Err31, position, ""};
            UseErrorReporter()->AddError(err);
            // error correcting
            ty = TypeFactory::MakeIntType();
        }
        ty = (*resTy)->UseType();
    }
    else
    {
        ty = TypeFactory::MakeUnitType();
    }

    std::vector<Type> formals;
    for (const Field& field : fields)
    {
        auto formalType = UseTypeEnvironment()->LookUp(field.type);
        if (!formalType)
        {
            Error err { ErrorCode::Err30, position, field.name.UseName()};
            UseErrorReporter()->AddError(err);
            // error correcting
            formalType = std::make_shared<EnvType>(TypeFactory::MakeIntType());
        }
    
        formals.push_back((*formalType)->UseType());
    }

    // Right now, we have to assume that the given type is valid.
    return std::make_shared<FunEntry>(formals, ty, UseLevel(), Temps::UseTempFactory().MakeLabel());
}

Type FunctionDeclaration::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());

    // inital pass to get headers of name type
    for (auto& decl : decls)
    {
        decl.SetEnvironments(UseValueEnvironment(), UseTypeEnvironment());
        std::shared_ptr<FunEntry> fun = decl.CalculateHeader();

        bool shadowed;
        UseValueEnvironment()->Insert(decl.name, fun, shadowed);
        if (shadowed)
        {
            std::string err("Shadowing of " + decl.name.UseName());
            ReportTypeError(ErrorCode::Err72, err);
        }
    }

    // second pass for checking bodies
    for (auto& decl : decls)
    {
        decl.TypeCheck();
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
            ReportTypeError(ErrorCode::Err22, name.UseName());
        }
    }
    else if (Types::IsStrictlyNil(experTy))
    {
        ReportTypeError(ErrorCode::Err23, "");
    }
    
    bool shadowed;
    UseValueEnvironment()->Insert(name, std::make_shared<VarEntry>(experTy, UseLevel()->AllocateLocal(escape)), shadowed);
    if (shadowed)
    {
        std::string err("Shadowing of " + name.UseName());
        UseWarningReporter()->AddWarning({UsePosition(), err, WarningLevel::Low});
    }

    return TypeFactory::MakeUnitType();
}

bool HasTypeCycle(const TyDec& tyDec, const std::shared_ptr<TypeEnvironment>& env)
{
    auto type = (*env->LookUp(tyDec.name))->UseType();
    std::set<Symbol> seen;
    seen.insert(tyDec.name);

    while (Types::IsNameType(type))
    {
        auto name = Types::GetSymbolFromNameType(type);
        if (seen.count(name) != 0)
        {
            return true;
        }
        seen.insert(name);
        type = (*env->LookUp(name))->UseType();
    }
    return false;
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
            ReportTypeError(ErrorCode::Err72, err);
        }
    }
    
    // second pass to actually type check
    for (const TyDec& tyDec : types)
    {
        Type ty = tyDec.type->TypeCheck();
        auto envValue = UseTypeEnvironment()->LookUp(tyDec.name);
        if (!envValue)
        {
            throw CompilerErrorException("Expected type to exist in type environment. This breaks recursive types");
        }

        (*envValue)->ReplaceNameTypeWithType(ty);
    }

    for (const TyDec& tyDec : types)
    {
        if (HasTypeCycle(tyDec, UseTypeEnvironment()))
        {
            ReportTypeError(ErrorCode::Err73);
        }
    }

    // TODO: does this return something different?
    return TypeFactory::MakeUnitType();
}

Type NameType::TypeCheck()
{
    assert(UseValueEnvironment());
    assert(UseTypeEnvironment());

    auto ty = UseTypeEnvironment()->LookUp(name);
    if (!ty)
    {
        ReportTypeError(ErrorCode::Err0, name.UseName());
        // error correct
        return TypeFactory::MakeIntType();
    }
    Type actual = (*ty)->UseType();

    return actual;
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

void SimpleVar::CalculateEscapes()
{
    UseEscapeCalculator()->EscapeIfNecessary(symbol);    
}

void FieldVar::CalculateEscapes()
{
    UseEscapeCalculator()->EscapeIfNecessary(symbol);
}

void SubscriptVar::CalculateEscapes()
{
    var->CalculateEscapes();
    expression->CalculateEscapes();
}

void VarExpression::CalculateEscapes()
{
    var->CalculateEscapes();
}

void CallExpression::CalculateEscapes()
{
    for (auto& arg : args)
    {
        arg->CalculateEscapes();
    }
}

void OpExpression::CalculateEscapes()
{
    lhs->CalculateEscapes();
    rhs->CalculateEscapes();
}

void RecordExpression::CalculateEscapes()
{
    for (auto& field : fields)
    {
        field.expr->CalculateEscapes();
    }
}

void SeqExpression::CalculateEscapes()
{
    for (auto& expr : expressions)
    {
        expr->CalculateEscapes();
    }
}

void AssignmentExpression::CalculateEscapes()
{
    var->CalculateEscapes();
    expression->CalculateEscapes();
}

void IfExpression::CalculateEscapes()
{
    test->CalculateEscapes();
    thenBranch->CalculateEscapes();
    if (elseBranch)
    {
        elseBranch->CalculateEscapes();
    }
}

void WhileExpression::CalculateEscapes()
{
    test->CalculateEscapes();
    body->CalculateEscapes();
}

void ForExpression::CalculateEscapes()
{
    // TODO: how does a forexpression escape?
    low->CalculateEscapes();
    high->CalculateEscapes();
    body->CalculateEscapes();
}

void LetExpression::CalculateEscapes()
{
    for (auto& decl : decls)
    {
        decl->CalculateEscapes();
    }
    body->CalculateEscapes();
}

void ArrayExpression::CalculateEscapes()
{
    size->CalculateEscapes();
    init->CalculateEscapes();
}

void FunDec::CalculateEscapes()
{
    for (auto& field : fields)
    {
        UseEscapeCalculator()->TrackDecl(field.name, &field.escape);
    }
    body->CalculateEscapes();
}

void FunctionDeclaration::CalculateEscapes()
{
    UseEscapeCalculator()->IncreaseDepth();
    for (auto& decl : decls)
    {
        decl.CalculateEscapes();
    }
    UseEscapeCalculator()->DecreaseDepth();
}

void VarDeclaration::CalculateEscapes()
{
    UseEscapeCalculator()->TrackDecl(name, &escape);
    init->CalculateEscapes();
}
