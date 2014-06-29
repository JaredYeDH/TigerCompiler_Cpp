#include "gtest/gtest.h"

#include "Calc.h"

class CalcTest : public ::testing::Test
{
};

TEST_F(CalcTest, Calc_Add_OnePlusOneIsTwo) 
{
    Calc calc;
    int augend = 1;
    int addend = 1;
    int expected_sum = 2;

    int actual_sum = calc.Add(augend, addend);

    ASSERT_EQ( expected_sum, actual_sum );
}
