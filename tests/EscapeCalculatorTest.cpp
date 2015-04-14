#include "gtest/gtest.h"

#include "common.h"
#include "EscapeCalculator.h"

class EscapeCalculatorTest : public ::testing::Test
{
};

TEST_F(EscapeCalculatorTest, Fail)
{
    std::vector<bool>(4, true);
    ASSERT_TRUE(false);
}

TEST_F(EscapeCalculatorTest, TrackingMakesEscapesFalse)
{
    bool escape = true;
    auto escapeCalc = EscapeCalculatorFactory::MakeEscapeCalculator();
    escapeCalc->TrackDecl(SymbolFactory::GenerateSymbol("a"), &escape);
    ASSERT_FALSE(escape);
}
