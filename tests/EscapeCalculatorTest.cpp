#include "gtest/gtest.h"

#include "common.h"
#include "EscapeCalculator.h"

class EscapeCalculatorTest : public ::testing::Test
{
};

TEST_F(EscapeCalculatorTest, TrackingMakesEscapesFalse)
{
    bool escape = true;
    auto escapeCalc = EscapeCalculatorFactory::MakeEscapeCalculator();
    escapeCalc->TrackDecl(SymbolFactory::GenerateSymbol("a"), &escape);
    ASSERT_FALSE(escape);
}

TEST_F(EscapeCalculatorTest, WhenUsedOutOfCurrentDepth_Escapes)
{
    bool escape = false;
    auto escapeCalc = EscapeCalculatorFactory::MakeEscapeCalculator();
    auto symbol = SymbolFactory::GenerateSymbol("a");
    escapeCalc->TrackDecl(symbol, &escape);

    escapeCalc->IncreaseDepth();
    escapeCalc->EscapeIfNecessary(symbol);
    ASSERT_TRUE(escape);
    escapeCalc->DecreaseDepth();
}

TEST_F(EscapeCalculatorTest, WhenUsedInCurrentDepth_NoEscapes)
{
    bool escape = false;
    auto escapeCalc = EscapeCalculatorFactory::MakeEscapeCalculator();
    auto symbol = SymbolFactory::GenerateSymbol("a");
    escapeCalc->TrackDecl(symbol, &escape);

    escapeCalc->IncreaseDepth();
    escapeCalc->DecreaseDepth();

    escapeCalc->EscapeIfNecessary(symbol);
    ASSERT_FALSE(escape);
}


