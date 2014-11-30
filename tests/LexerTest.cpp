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

TEST_F(LexerTest, LexComma_GivesComma)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(",");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Comma, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexColon_GivesColon)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(":");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Colon, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexSemi_GivesSemi)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(";");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Semi, lexer.TokenizeNext());
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

TEST_F(LexerTest, LexOpenBracket_GivesLBracket)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("[");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::LBracket, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexCloseBracket_GivesRBracket)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("]");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::RBracket, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexOpenBrace_GivesLBrace)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("{");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::LBrace, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexCloseBrace_GivesRBrace)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("}");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::RBrace, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexPeriod_GivesPeriod)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(".");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Period, lexer.TokenizeNext());
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

TEST_F(LexerTest, LexDiv_GivesDiv)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("/");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Div, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexEquals_GivesEquals)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("=");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Equal, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexNotEqual_GivesNotEqual)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("<>");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::NotEqual, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexLessThan_GivesLessThan)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("<");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::LessThan, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexLEquals_GivesLEquals)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("<=");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::LEqual, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexGreaterThan_GivesGreaterThan)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(">");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::GreaterThan, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexGEqual_GivesGEqual)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(">=");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::GEqual, lexer.TokenizeNext());
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

TEST_F(LexerTest, LexFor_GivesFor)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("for");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::For, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexTo_GivesTo)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("to");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::To, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexDo_GivesDo)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("do");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Do, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexLet_GivesLet)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("let");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Let, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexIn_GivesIn)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("in");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::In, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexEnd_GivesEnd)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("end");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::End, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexOf_GivesOf)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("of");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Of, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexBreak_GivesBreak)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("break");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Break, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexNil_GivesNil)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("nil");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Nil, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexFunction_GivesFunction)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("function");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Function, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexVar_GivesVar)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("var");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Var, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexType_GivesType)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("type");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Type, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexImport_GivesImport)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("import");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Import, lexer.TokenizeNext());
}

TEST_F(LexerTest, LexPrimative_GivesPrimative)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("primative");
	Lexer lexer(std::move(inStream));
	AssertEqualTokens(PrimativeToken::Primative, lexer.TokenizeNext());
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

TEST_F(LexerTest, LexString_String)
{
	std::string strVal("\"This is \t a string\"");
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(strVal);
	Lexer lexer(std::move(inStream));
	Token token = lexer.TokenizeNext();
	AssertEqualTokens(Token(PrimativeToken::StringLit, "This is \t a string"), token);
}

TEST_F(LexerTest, LexSuperSimpleComment_Success)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("/**/");
	Lexer lexer(std::move(inStream));
	Token token = lexer.TokenizeNext();
	AssertEqualTokens(PrimativeToken::EndOfFile, token);
}

TEST_F(LexerTest, LexSimpleCommentUnclosed_Throws)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("  /* foo bar");
	Lexer lexer(std::move(inStream));
	ASSERT_THROW(lexer.TokenizeNext(), LexException);
}

TEST_F(LexerTest, LexSimpleComment_Success)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("  /* foo bar */");
	Lexer lexer(std::move(inStream));
	Token token = lexer.TokenizeNext();
	AssertEqualTokens(PrimativeToken::EndOfFile, token);
}

TEST_F(LexerTest, LexNestedComment_Success)
{
	std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("/* foo /* bar */ baz */");
	Lexer lexer(std::move(inStream));
	Token token = lexer.TokenizeNext();
	AssertEqualTokens(PrimativeToken::EndOfFile, token);
}

TEST_F(LexerTest, LexAFullThing)
{
	std::string program(" if foo + 108 = x \n\rthen /* \r A comment \r\n /* that is nested */ */  foo := x  else\n bar:=b-c*10          ");
	std::vector<Token> expectedTokens;
	expectedTokens.push_back(Token(PrimativeToken::If));
	expectedTokens.push_back(Token(PrimativeToken::Identifier, "foo"));
	expectedTokens.push_back(Token(PrimativeToken::Plus));
	expectedTokens.push_back(Token(PrimativeToken::Number, "108"));
	expectedTokens.push_back(Token(PrimativeToken::Equal));
	expectedTokens.push_back(Token(PrimativeToken::Identifier, "x"));
	expectedTokens.push_back(Token(PrimativeToken::Then));
	expectedTokens.push_back(Token(PrimativeToken::Identifier, "foo"));
	expectedTokens.push_back(Token(PrimativeToken::Assign));
	expectedTokens.push_back(Token(PrimativeToken::Identifier, "x"));
	expectedTokens.push_back(Token(PrimativeToken::Else));
	expectedTokens.push_back(Token(PrimativeToken::Identifier, "bar"));
	expectedTokens.push_back(Token(PrimativeToken::Assign));
	expectedTokens.push_back(Token(PrimativeToken::Identifier, "b"));
	expectedTokens.push_back(Token(PrimativeToken::Minus));
	expectedTokens.push_back(Token(PrimativeToken::Identifier, "c"));
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