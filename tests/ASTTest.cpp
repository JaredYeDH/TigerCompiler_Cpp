#include "gtest/gtest.h"

#include "AST.h"
#include "Parser.h"

using namespace std;
using namespace AST;

class ASTTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        errors = nullptr;
        warnings = nullptr;
        ast = nullptr;
    }

    void CreateFromString(const std::string& program)
    {
        unique_ptr<istream> stream = make_unique<stringstream>(program);
        TokenStream tokenStream(move(stream));
        errors = make_shared<CompileTimeErrorReporter>();
        warnings = make_shared<WarningReporter>();
        auto parser = make_unique<Parser>(move(tokenStream), errors, warnings);
        ast = parser->Parse();
    }

    uint8_t CountNumberOfEscapes()
    {
        return 0;
    }

    std::shared_ptr<CompileTimeErrorReporter> errors;
    std::shared_ptr<WarningReporter> warnings;
    std::unique_ptr<Program> ast;
};

extern unique_ptr<Program> MakeTestProgram(unique_ptr<Expression>&& expr);

TEST_F(ASTTest, TypeCheck_OpExpressionMatchingTypes) 
{
    unique_ptr<Program> prog = MakeTestProgram(make_unique<OpExpression>(
        make_unique<IntExpression>(0, Position(0,0)),
        make_unique<IntExpression>(6, Position(0,0)),
        BinOp::Minus, Position{0,0}
    ));
    
    ASSERT_TRUE(AreEqualTypes(TypeFactory::MakeIntType(), prog->TypeCheck()));
}

TEST_F(ASTTest, TypeCheck_NilExprresion) 
{
    auto prog = MakeTestProgram(make_unique<NilExpression>(Position{0,0}));
    
    ASSERT_TRUE(AreEqualTypes(TypeFactory::MakeNilType(), prog->TypeCheck()));
}

TEST_F(ASTTest, TypeCheck_IntExprresion) 
{
    auto prog = MakeTestProgram(make_unique<IntExpression>(5, Position(0,0)));
    
    ASSERT_TRUE(AreEqualTypes(TypeFactory::MakeIntType(), prog->TypeCheck()));
}

TEST_F(ASTTest, TypeCheck_StringExprresion) 
{
    auto prog = MakeTestProgram(make_unique<StringExpression>("moop", Position(0,0)));
    ASSERT_TRUE(AreEqualTypes(TypeFactory::MakeStringType(), prog->TypeCheck()));
}

TEST_F(ASTTest, TypeCheckSimpleRecursiveTypeDecl)
{
    vector<TyDec> types;
    Symbol intlist = SymbolFactory::GenerateSymbol("intlist");
    vector<Field> fields;
    fields.push_back({SymbolFactory::GenerateSymbol("tl"), intlist, {0,0}});
    
    std::unique_ptr<RecordType> recTy = make_unique<RecordType>(std::move(fields), Position{0,0});

    types.push_back(TyDec (intlist, std::move(recTy), Position{0,0}));

    std::shared_ptr<TypeDeclaration> decl = std::make_shared<TypeDeclaration>(std::move(types), Position{0,0});
    
    auto valEnv = ValueEnvironment::GenerateBaseValueEnvironment();
    auto tyEnv  = TypeEnvironment::GenerateBaseTypeEnvironment();
    decl->SetEnvironments(valEnv, tyEnv);
   
    decl->TypeCheck();

    auto tyWrapper = tyEnv->LookUp(intlist);
    ASSERT_TRUE(tyWrapper);
    auto tyUnWrapped = (*tyWrapper)->UseType();
}

TEST_F(ASTTest, CalculateEscapes_NoEscapableValues_NothingMarkedEscaping)
{
    CreateFromString("let function foo(x : int) = print(chr(x)) in foo(1) end");
    ast->CalculateEscapes();
}
