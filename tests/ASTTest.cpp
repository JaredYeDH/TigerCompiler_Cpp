#include "gtest/gtest.h"

#include "AST.h"

using namespace std;
using namespace AST;

class ASTTest : public ::testing::Test
{
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
