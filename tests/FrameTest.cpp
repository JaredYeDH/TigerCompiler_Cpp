#include "gtest/gtest.h"

#include "Temps.h"
#include "Frame.h"

using namespace Temps;

class FrameTest : public ::testing::Test
{
};

TEST_F(FrameTest, GetNameUsesNameGiven)
{
    Label l = UseTempFactory().MakeLabel();
    auto frame = FrameFactory::MakeFrame(l, {});
    ASSERT_EQ(l, frame->GetName());
}

TEST_F(FrameTest, FAIL)
{
    FAIL();
}
