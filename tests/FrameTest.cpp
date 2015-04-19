#include "gtest/gtest.h"

#include "Temps.h"
#include "Frame.h"

using namespace Temps;
using namespace FrameAccess;

class FrameTest : public ::testing::Test
{
};

TEST_F(FrameTest, GetNameUsesNameGiven)
{
    Label l = UseTempFactory().MakeLabel();
    auto frame = FrameFactory::MakeFrame(l, {});
    ASSERT_EQ(l, frame->GetName());
}

TEST_F(FrameTest, FormalsAsAccessIsSameSizeAsFormals)
{
    Label l = UseTempFactory().MakeLabel();
    std::vector<bool> formals {true, false, true};
    auto frame = FrameFactory::MakeFrame(l, formals);
    ASSERT_EQ(formals.size(), frame->UseFormals().size());
}
