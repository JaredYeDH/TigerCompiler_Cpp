#include "gtest/gtest.h"

#include "TokenStream.h"

using namespace std;

class TokenStreamTest : public ::testing::Test
{
};

void AssertNextPrimMatches(PrimativeToken prim, TokenStream& stream)
{
    ASSERT_EQ(prim, stream.GetNextToken().GetTokenType());
}

void AssertNextPositionAndPrimMatch(const Position& pos, PrimativeToken prim, TokenStream& stream)
{
    auto token = stream.GetNextToken();
    ASSERT_EQ(pos, token.UsePosition());
    ASSERT_EQ(prim, token.GetTokenType());
}

TEST_F(TokenStreamTest, SingleElm_GetNext_GetsThatElm) 
{
    vector<Token> tokens{ Token{ PrimativeToken::While} };
    TokenStream stream(tokens);
    AssertNextPrimMatches(PrimativeToken::While, stream);
}

TEST_F(TokenStreamTest, NoElm_GetNext_ReturnsEOF)
{
    TokenStream stream {vector<Token>()};
    AssertNextPrimMatches(PrimativeToken::EndOfFile, stream);
}

TEST_F(TokenStreamTest, NoElm_GetNextManyTimes_ReturnsEOF)
{
    TokenStream stream {vector<Token>()};
    for (int i = 0; i < 10; ++i)
    {
        AssertNextPrimMatches(PrimativeToken::EndOfFile, stream);
    }
}

TEST_F(TokenStreamTest, ManyElm_GetNext_GoesInOrder)
{
    TokenStream stream {vector<Token>{
         Token{PrimativeToken::If}
        ,Token{PrimativeToken::Then}
        ,Token{PrimativeToken::Else}
        }
    };

    AssertNextPrimMatches(PrimativeToken::If, stream);
    AssertNextPrimMatches(PrimativeToken::Then, stream);
    AssertNextPrimMatches(PrimativeToken::Else, stream);
}

TEST_F(TokenStreamTest, Peek_DoesNotAdvance)
{
    TokenStream stream {vector<Token> {Token{PrimativeToken::If}}};
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_EQ(PrimativeToken::If,
                stream.PeekNextToken().GetTokenType());
    }
}

TEST_F(TokenStreamTest, CreateFromStream_Tokenizes)
{
    unique_ptr<istream> inStream = make_unique<stringstream>("for");
    TokenStream stream(move(inStream));
    AssertNextPrimMatches(PrimativeToken::For, stream);
}

TEST_F(TokenStreamTest, CreateFromStream_TokenizesMany)
{
    unique_ptr<istream> inStream = make_unique<stringstream>("if then for");
    TokenStream stream(move(inStream));
    AssertNextPrimMatches(PrimativeToken::If, stream);
    AssertNextPrimMatches(PrimativeToken::Then, stream);
    AssertNextPrimMatches(PrimativeToken::For, stream);
}

TEST_F(TokenStreamTest, CreateFromStream_TokenizesTon)
{
    unique_ptr<istream> inStream = make_unique<stringstream>("in rec1.nam := \"asd\" end");
    TokenStream stream(move(inStream));
    AssertNextPrimMatches(PrimativeToken::In, stream);
    AssertNextPrimMatches(PrimativeToken::Identifier, stream);
    AssertNextPrimMatches(PrimativeToken::Period, stream);
    AssertNextPrimMatches(PrimativeToken::Identifier, stream);
    AssertNextPrimMatches(PrimativeToken::Assign, stream);
    AssertNextPrimMatches(PrimativeToken::StringLit, stream);
    AssertNextPrimMatches(PrimativeToken::End, stream);
}

TEST_F(TokenStreamTest, ComplexWithCommentsGetsPositionCorrect)
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>(
    "/* error : procedure returns value */\n"
    "if 1\n"
    "    then 2\n"
    "    else g\n");
    TokenStream stream(move(inStream));

    AssertNextPositionAndPrimMatch({2,1}, PrimativeToken::If, stream);
    AssertNextPositionAndPrimMatch({2,4}, PrimativeToken::Number, stream);
    AssertNextPositionAndPrimMatch({3,5}, PrimativeToken::Then, stream);
    AssertNextPositionAndPrimMatch({3,10},PrimativeToken::Number, stream);
    AssertNextPositionAndPrimMatch({4,5}, PrimativeToken::Else, stream);
    AssertNextPositionAndPrimMatch({4,10},PrimativeToken::Identifier, stream);
} 
