#include "gtest/gtest.h"

#include "Symbol.h"
#include "SymbolTable.h"

class SymbolTableTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        table = SymbolTableFactory::CreateTable<int>();
    } 

    std::shared_ptr<ISymbolTable<int>> table;
};

TEST_F(SymbolTableTest, NewSymTable_DoesNotContainAbitrarySymbol) 
{
    table->BeginScope();
    ASSERT_FALSE(table->LookUp(SymbolFactory::GenerateSymbol("a")));
}


TEST_F(SymbolTableTest, NewSymTable_AddingContainsAbitrarySymbol) 
{
    table->BeginScope();
    bool shadowed;
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4, shadowed);
    ASSERT_FALSE(shadowed);
    ASSERT_TRUE(table->LookUp(SymbolFactory::GenerateSymbol("a")));
}

TEST_F(SymbolTableTest, SymbolTableErrorWhenOverwrittingExistingValue) 
{
    table->BeginScope();
    bool shadowed;
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4, shadowed);
    ASSERT_FALSE(shadowed);
    // overwrites original mapping
    table->Insert(SymbolFactory::GenerateSymbol("a"), 5, shadowed);
    ASSERT_TRUE(shadowed);
    ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 5);
}

TEST_F(SymbolTableTest, SymbolTableRemovedFromTableWhenOutOfScope)
{
    table->BeginScope();
    bool shadowed;
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4, shadowed);
    table->EndScope();
    ASSERT_FALSE(table->LookUp(SymbolFactory::GenerateSymbol("a")));
}

TEST_F(SymbolTableTest, SymbolTableShadowedWhenNewScope)
{
    table->BeginScope();
    bool shadowed;
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4, shadowed);
    {
        table->BeginScope();
        table->Insert(SymbolFactory::GenerateSymbol("a"), 5, shadowed);
        ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 5);
        ASSERT_FALSE(shadowed);
        table->EndScope();
    }
}

TEST_F(SymbolTableTest, SymbolTableBackToOrigianlWhenBackToOriginalScope)
{
    table->BeginScope();
    bool shadowed;
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4, shadowed);
    table->BeginScope();
    table->Insert(SymbolFactory::GenerateSymbol("a"), 5, shadowed);
    ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 5);
    table->EndScope();
    ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 4);
}

TEST_F(SymbolTableTest, SymbolTableCanGetValueFromOuterScopeIfNotShadowed)
{
    table->BeginScope();
    bool shadowed;
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4, shadowed);
    table->BeginScope();
    table->Insert(SymbolFactory::GenerateSymbol("b"), 5, shadowed);
    ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 4);
}
