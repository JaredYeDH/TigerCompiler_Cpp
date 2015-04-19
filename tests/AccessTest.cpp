#include "gtest/gtest.h"

#include "Temps.h"
#include "Frame.h"

using namespace FrameAccess;
using namespace Temps;

class AccessTest : public ::testing::Test
{
};

TEST_F(AccessTest, AccessConstructedWithOffsetIsFrameKind)
{
    Access access{4};
    ASSERT_EQ(AccessKind::InFrame, access.GetKind());
}

TEST_F(AccessTest, AccessConstructedWithTempIsRegKind)
{
    Access access{UseTempFactory().MakeTempVar()};
    ASSERT_EQ(AccessKind::InRegister, access.GetKind());
}

