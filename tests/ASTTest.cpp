#include "gtest/gtest.h"

#include "AST.h"

using namespace std;
using namespace AST;

class ASTTest : public ::testing::Test
{
};

TEST_F(ASTTest, TypeCheck_OpExpressionMatchingTypes) 
{
    unique_ptr<Program> prog = make_unique<Program>(make_unique<OpExpression>(
        make_unique<IntExpression>(0),
        make_unique<IntExpression>(6),
        BinOp::Minus
    ));
    
    ASSERT_TRUE(AreEqualTypes(TypeFactory::MakeIntType(), prog->TypeCheck()));
}

TEST_F(ASTTest, TypeCheck_NilExprresion) 
{
    auto prog = make_unique<NilExpression>();
    
    ASSERT_TRUE(AreEqualTypes(TypeFactory::MakeNilType(), prog->TypeCheck()));
}

TEST_F(ASTTest, TypeCheck_IntExprresion) 
{
    auto prog = make_unique<IntExpression>(5);
    
    ASSERT_TRUE(AreEqualTypes(TypeFactory::MakeIntType(), prog->TypeCheck()));
}

TEST_F(ASTTest, TypeCheck_StringExprresion) 
{
    auto prog = make_unique<StringExpression>("moop");
    
    ASSERT_TRUE(AreEqualTypes(TypeFactory::MakeStringType(), prog->TypeCheck()));
}
