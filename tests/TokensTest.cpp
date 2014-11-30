#include "gtest/gtest.h"
#include "Tokens.h"

class TokensTest : public ::testing::Test
{
};

TEST_F(TokensTest, MakeFromPrimThatNeedsValue_ThrowsNeedsValue_ExhaustiveFail) 
{
	ASSERT_THROW(Token{ PrimativeToken::Number }, TokenException);
	ASSERT_THROW(Token{ PrimativeToken::Location }, TokenException);
}

TEST_F(TokensTest, MakeFromPrimThatNeedsValue_Value_ExhaustivePass) 
{
	ASSERT_NO_THROW( Token(PrimativeToken::Number, "1") );
	ASSERT_NO_THROW( Token(PrimativeToken::Location, "a") );
} 

TEST_F(TokensTest, MakeFromPrimThatNeedsNoValue_NoValue_ExhaustivePass) 
{
	ASSERT_NO_THROW( Token{ PrimativeToken::EndOfFile } );
    ASSERT_NO_THROW( Token{ PrimativeToken::LParen } );
    ASSERT_NO_THROW( Token{ PrimativeToken::RParen } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Plus } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Minus } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Times } );
    ASSERT_NO_THROW( Token{ PrimativeToken::True } );
    ASSERT_NO_THROW( Token{ PrimativeToken::False } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Equal } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Not } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Skip } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Assign } );
    ASSERT_NO_THROW( Token{ PrimativeToken::If } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Then } );
    ASSERT_NO_THROW( Token{ PrimativeToken::Else } );
    ASSERT_NO_THROW( Token{ PrimativeToken::While } );
}

TEST_F(TokensTest, MakeFromPrimThatNeedsNoValue_GivenValue_ExhaustiveFail) 
{
	ASSERT_THROW( Token(PrimativeToken::EndOfFile, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::LParen, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::RParen, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Plus, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Minus, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Times, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::True, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::False, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Equal, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Not, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Skip, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Assign, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::If, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Then, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::Else, "1"), TokenException );
    ASSERT_THROW( Token(PrimativeToken::While, "1"), TokenException );
}

TEST_F(TokensTest, NoValueToken_HasNoValue)
{
	Token t(PrimativeToken::Not);
	ASSERT_FALSE(t.HasValue());
	ASSERT_STREQ("", t.UseValue().c_str());
}


TEST_F(TokensTest,ValueToken_HasValue)
{
	Token t(PrimativeToken::Location, "foo");
	ASSERT_TRUE(t.HasValue());
	ASSERT_STREQ("foo", t.UseValue().c_str());
}