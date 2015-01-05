#include "gtest/gtest.h"

#include "Symbol.h"

class SymbolTest : public ::testing::Test
{
};

TEST_F(SymbolTest, SameSymbolInstanceEqual) 
{
    Symbol sym = SymbolFactory::GenerateSymbol("ab");
    ASSERT_EQ(sym, sym);
}

TEST_F(SymbolTest, SameSymbolTwoTimes_Equals)
{
    Symbol sym1 = SymbolFactory::GenerateSymbol("a");
    Symbol sym2 = SymbolFactory::GenerateSymbol("a");
    ASSERT_EQ(sym1, sym2);
}

TEST_F(SymbolTest, DifferentSymbols_NoEqual)
{
    Symbol sym1 = SymbolFactory::GenerateSymbol("a");
    Symbol sym2 = SymbolFactory::GenerateSymbol("b");
    ASSERT_NE(sym1, sym2);
}

TEST_F(SymbolTest, EqualCaseInsensitive)
{
    Symbol sym1 = SymbolFactory::GenerateSymbol("a");
    Symbol sym2 = SymbolFactory::GenerateSymbol("A");
    ASSERT_NE(sym1, sym2);
}

TEST_F(SymbolTest, UseNameWorksAsExpected)
{
    Symbol sym = SymbolFactory::GenerateSymbol("a");
    ASSERT_STREQ(sym.UseName().c_str(), "a");
}


TEST_F(SymbolTest, SameSymbolInstanceHashEqual) 
{
    Symbol sym = SymbolFactory::GenerateSymbol("ab");
    ASSERT_EQ(sym.Hash(), sym.Hash());
}

TEST_F(SymbolTest, SameSymbolTwoTimesHash_Equals)
{
    Symbol sym1 = SymbolFactory::GenerateSymbol("a");
    Symbol sym2 = SymbolFactory::GenerateSymbol("a");
    ASSERT_EQ(sym1.Hash(), sym2.Hash());
}

TEST_F(SymbolTest, DifferentSymbolsHash_NoEqual)
{
    Symbol sym1 = SymbolFactory::GenerateSymbol("a");
    Symbol sym2 = SymbolFactory::GenerateSymbol("b");
    ASSERT_NE(sym1.Hash(), sym2.Hash());
}

TEST_F(SymbolTest, EmptySymbol_ExpectException)
{
    EXPECT_THROW(SymbolFactory::GenerateSymbol(""), CompilerErrorException);
}
