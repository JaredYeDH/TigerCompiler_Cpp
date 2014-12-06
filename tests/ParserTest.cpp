#include "gtest/gtest.h"

#include "Parser.h"

class ParserTest : public ::testing::Test
{
};

TEST_F(ParserTest, Fail) 
{
    ASSERT_EQ(true, true);
}
