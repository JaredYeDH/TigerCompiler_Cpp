#include "gtest/gtest.h"

#include "Temps.h"

using namespace Temps;

class TempsTest : public ::testing::Test
{
};

TEST_F(TempsTest, TempIsEqualToItself)
{
    LocalTemp t = UseTempFactory().MakeTempVar();
    ASSERT_EQ(t, t);
}

TEST_F(TempsTest, TwoTempsAreNotEqual)
{
    LocalTemp t1 = UseTempFactory().MakeTempVar();
    LocalTemp t2 = UseTempFactory().MakeTempVar();
    ASSERT_NE(t1, t2);
}

TEST_F(TempsTest, LabelIsEqualToItself)
{
    Label l = UseTempFactory().MakeLabel();
    ASSERT_EQ(l, l);
}

TEST_F(TempsTest, LabelIsNotEqualToNewLabel)
{
    Label l1 = UseTempFactory().MakeLabel();
    Label l2 = UseTempFactory().MakeLabel();
    ASSERT_NE(l1, l2);
}

TEST_F(TempsTest, NamedLabelEqualToItself)
{
    Label nl = UseTempFactory().MakeNamedLabel("Moooop");
    ASSERT_EQ(nl, nl);
}

TEST_F(TempsTest, NamedLabelNotEqualToGenerated)
{
    Label nl = UseTempFactory().MakeNamedLabel("0");
    Label gl = UseTempFactory().MakeLabel();
    ASSERT_NE(nl, gl);
}
