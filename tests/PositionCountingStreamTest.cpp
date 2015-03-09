#include "gtest/gtest.h"

#include "PositionCountingStream.h"

class PCSTest : public ::testing::Test
{};

TEST_F(PCSTest, PositionAtStartIsZeroZero) 
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("a program");
    auto stream = PositionCountingStream(std::move(inStream));

    ASSERT_EQ(Position(1,1), stream.GetCurrentPosition());
}

TEST_F(PCSTest, GetAdvancesColumn)
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("a program");
    auto stream = PositionCountingStream(std::move(inStream));

    ASSERT_EQ('a', stream.get());
    ASSERT_EQ(Position(1,2), stream.GetCurrentPosition());
}

TEST_F(PCSTest, PeekDoesNotAdvanceColumn)
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("a program");
    auto stream = PositionCountingStream(std::move(inStream));

    ASSERT_EQ('a', stream.peek());
    ASSERT_EQ(Position(1,1), stream.GetCurrentPosition());
}

TEST_F(PCSTest, PeekTwoDoesNotAdvanceColumn)
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("abs");
    auto stream = PositionCountingStream(std::move(inStream));

    ASSERT_EQ('b', stream.peekTwo());
    ASSERT_EQ(Position(1,1), stream.GetCurrentPosition());
}

TEST_F(PCSTest, MultipleGets)
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("a program");
    auto stream = PositionCountingStream(std::move(inStream));

    ASSERT_EQ('a', stream.get());
    ASSERT_EQ(Position(1,2), stream.GetCurrentPosition());

    ASSERT_EQ(' ', stream.get());
    ASSERT_EQ(Position(1,3), stream.GetCurrentPosition());

    ASSERT_EQ('p', stream.get());
    ASSERT_EQ(Position(1,4), stream.GetCurrentPosition());
}

TEST_F(PCSTest, MultipleColumnAdvanceSimple)
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("\n\n\n");
    auto stream = PositionCountingStream(std::move(inStream));

    ASSERT_EQ(Position(1,1), stream.GetCurrentPosition());
    
    stream.get();
    ASSERT_EQ(Position(2,1), stream.GetCurrentPosition());

    stream.get();
    ASSERT_EQ(Position(3,1), stream.GetCurrentPosition());

    stream.get();
    ASSERT_EQ(Position(4,1), stream.GetCurrentPosition());
}

TEST_F(PCSTest, MultipleColumnAdvanceComplex)
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("abs\n\n\nx");
    auto stream = PositionCountingStream(std::move(inStream));

    ASSERT_EQ(Position(1,1), stream.GetCurrentPosition());

    stream.get();
    ASSERT_EQ(Position(1,2), stream.GetCurrentPosition());
    stream.get();
    ASSERT_EQ(Position(1,3), stream.GetCurrentPosition());
    stream.get();
    ASSERT_EQ(Position(1,4), stream.GetCurrentPosition());
    
    stream.get();
    ASSERT_EQ(Position(2,1), stream.GetCurrentPosition());

    stream.get();
    ASSERT_EQ(Position(3,1), stream.GetCurrentPosition());

    stream.get();
    ASSERT_EQ(Position(4,1), stream.GetCurrentPosition());

    stream.get();
    ASSERT_EQ(Position(4,2), stream.GetCurrentPosition());
}
