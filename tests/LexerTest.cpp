#include "gtest/gtest.h"
#include "Lexer.h"

class LexerTest : public ::testing::Test
{
};

void AssertEqualTokens(PrimativeToken lhs, const Token& rhs)
{
	ASSERT_EQ(static_cast<unsigned int>(lhs), static_cast<unsigned int>(rhs.GetTokenType()));
}

void AssertEqualTokens(const Token& lhs, const Token& rhs)
{
	ASSERT_EQ(static_cast<unsigned int>(lhs.GetTokenType()), static_cast<unsigned int>(rhs.GetTokenType()));
	ASSERT_STREQ(lhs.UseValue().c_str(), rhs.UseValue().c_str());
}

TEST_F(LexerTest, LexEmpty_GivesEOF)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::EndOfFile, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexWhite_GivesEOF)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("  \t \n");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::EndOfFile, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexOpenParen_GivesLParen)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("(");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::LParen, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexCloseParen_GivesRParen)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(")");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::RParen, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexPlus_GivesPlus)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("+");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Plus, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexMinus_GivesMinus)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("-");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Minus, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexTimes_GivesTimes)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("*");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Times, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexEquals_GivesEquals)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("=");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Equal, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexLEquals_GivesLEquals)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("<=");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::LEqual, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexNot_GivesNot)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("!");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Not, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexAnd_GivesAnd)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("&");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::And, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexOr_GivesOr)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("|");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Or, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexAssign_GivesAssign)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(":=");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Assign, lexer.TokenizeNext());
}

TEST_F(LexerTest, ChainSingleCharOps_NoSpace)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("&|");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::And, lexer.TokenizeNext());
	AssertEqualTokens(PrimativeToken::Or, lexer.TokenizeNext());
	AssertEqualTokens(PrimativeToken::EndOfFile, lexer.TokenizeNext());
}

TEST_F(LexerTest, ChainSingleCharOps_ManySpace)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("  &    |");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::And, lexer.TokenizeNext());
	AssertEqualTokens(PrimativeToken::Or, lexer.TokenizeNext());
	AssertEqualTokens(PrimativeToken::EndOfFile, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexSkip_GivesSkip)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("skip");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Skip, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexTrue_GivesTrue)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("true");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::True, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexFalse_GivesFalse)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("false");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::False, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexIf_GivesIf)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("if");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::If, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexThen_GivesThen)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("then");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Then, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexElse_GivesElse)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("else");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Else, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexWhile_GivesWhile)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("while");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::While, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexLongNumber_GivesNumber)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("1592");
	Lexer lexer(std::move(inStream));
	Token token = lexer.TokenizeNext();
	AssertEqualTokens(PrimativeToken::Number, token);
	ASSERT_STREQ("1592", token.UseValue().c_str());
}

TEST_F(LexerTest, LexNumber_GivesNumber)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("8");
	Lexer lexer(std::move(inStream));
	Token token = lexer.TokenizeNext();
	AssertEqualTokens(PrimativeToken::Number, token);
	ASSERT_STREQ("8", token.UseValue().c_str());
}

TEST_F(LexerTest, LexNegNumber_GivesNumber)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("-108");
	Lexer lexer(std::move(inStream));
	Token token = lexer.TokenizeNext();
	AssertEqualTokens(PrimativeToken::Number, token);
	ASSERT_STREQ("-108", token.UseValue().c_str());
}

TEST_F(LexerTest, LexAFullThing)
{
	std::string program("if foo + 108 = x \nthen foo := x  else\n bar:=b-c*10          ");
	std::vector<Token> expectedTokens;
	expectedTokens.push_back(Token(PrimativeToken::If));
	expectedTokens.push_back(Token(PrimativeToken::Location, "foo"));
	expectedTokens.push_back(Token(PrimativeToken::Plus));
	expectedTokens.push_back(Token(PrimativeToken::Number, "108"));
	expectedTokens.push_back(Token(PrimativeToken::Equal));
	expectedTokens.push_back(Token(PrimativeToken::Location, "x"));
	expectedTokens.push_back(Token(PrimativeToken::Then));
	expectedTokens.push_back(Token(PrimativeToken::Location, "foo"));
	expectedTokens.push_back(Token(PrimativeToken::Assign));
	expectedTokens.push_back(Token(PrimativeToken::Location, "x"));
	expectedTokens.push_back(Token(PrimativeToken::Else));
	expectedTokens.push_back(Token(PrimativeToken::Location, "bar"));
	expectedTokens.push_back(Token(PrimativeToken::Assign));
	expectedTokens.push_back(Token(PrimativeToken::Location, "b"));
	expectedTokens.push_back(Token(PrimativeToken::Minus));
	expectedTokens.push_back(Token(PrimativeToken::Location, "c"));
	expectedTokens.push_back(Token(PrimativeToken::Times));
	expectedTokens.push_back(Token(PrimativeToken::Number, "10"));
	expectedTokens.push_back(Token(PrimativeToken::EndOfFile));

	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(program);
	Lexer lexer(std::move(inStream));

	for (const Token& token : expectedTokens)
	{
		AssertEqualTokens(token, lexer.TokenizeNext());
	}
}

TEST_F(LexerTest, TokenizeGarbageFails)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("@");
	Lexer lexer(std::move(inStream));
	ASSERT_THROW(lexer.TokenizeNext(), LexException);
}