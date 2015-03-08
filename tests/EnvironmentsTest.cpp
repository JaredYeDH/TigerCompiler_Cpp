#include "gtest/gtest.h"

#include "Environments.h"

class EnvTest : public ::testing::Test
{
};

TEST_F(EnvTest, BaseTypeEnvironment_ContainsInt)
{
    auto tyTest = TypeEnvironment::GenerateBaseTypeEnvironment();
    ASSERT_TRUE(tyTest->LookUp(SymbolFactory::GenerateSymbol("int")));
}

TEST_F(EnvTest, BaseTypeEnvironment_ContainsString)
{
    auto tyTest = TypeEnvironment::GenerateBaseTypeEnvironment();
    ASSERT_TRUE(tyTest->LookUp(SymbolFactory::GenerateSymbol("string")));
}

TEST_F(EnvTest, BaseTypeEnvironment_AddWorksAsExpected)
{
    auto tyTest = TypeEnvironment::GenerateBaseTypeEnvironment();
    bool shadowed;
    tyTest->Insert(SymbolFactory::GenerateSymbol("foo"), TypeFactory::MakeIntType(), shadowed);
    ASSERT_TRUE(tyTest->LookUp(SymbolFactory::GenerateSymbol("foo")));
}

TEST_F(EnvTest, BaseValueEnvironment_ContainsPrint)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    auto print = valTest->LookUp(SymbolFactory::GenerateSymbol("print"));
    ASSERT_TRUE(print);
}

TEST_F(EnvTest, BaseValueEnvironment_ContainsFlush)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("flush")));
}

TEST_F(EnvTest, BaseValueEnvironment_ContainsGetChar)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("getchar")));
}

TEST_F(EnvTest, BaseValueEnvironment_Containsord)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("ord")));
}

TEST_F(EnvTest, BaseValueEnvironment_Containschr)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("chr")));
}

TEST_F(EnvTest, BaseValueEnvironment_Containssize)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("size")));
}

TEST_F(EnvTest, BaseValueEnvironment_Containssubstring)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("substring")));
}

TEST_F(EnvTest, BaseValueEnvironment_Containsconcat)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("concat")));
}

TEST_F(EnvTest, BaseValueEnvironment_Containsnot)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("not")));
}

TEST_F(EnvTest, BaseValueEnvironment_Containsexit)
{
    auto valTest = ValueEnvironment::GenerateBaseValueEnvironment();
    ASSERT_TRUE(valTest->LookUp(SymbolFactory::GenerateSymbol("exit")));
}
