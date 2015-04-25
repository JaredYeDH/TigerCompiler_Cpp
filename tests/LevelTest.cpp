#include "gtest/gtest.h"

#include "Translate.h"

using namespace std;
using namespace Translate;

class LevelTest : public ::testing::Test
{
};

TEST_F(LevelTest, AllocateLocal_)
{
    auto label = Temps::UseTempFactory().MakeLabel();
    vector<bool> formals{false, false};
    auto level = NewLevel(nullptr, label, formals);
    level->AllocateLocal(true);
}

TEST_F(LevelTest, NewLevelAddsStaticLinkParam)
{
    auto label = Temps::UseTempFactory().MakeLabel();
    vector<bool> formals{false, false};
    auto level = NewLevel(nullptr, label, formals);
    ASSERT_EQ(3, level->UseFormals().size());
}
