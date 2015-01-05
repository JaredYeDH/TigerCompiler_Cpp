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
    table->EndScope();
}


TEST_F(SymbolTableTest, NewSymTable_AddingContainsAbitrarySymbol) 
{
    table->BeginScope();
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4);
    ASSERT_TRUE(table->LookUp(SymbolFactory::GenerateSymbol("a")));
    table->EndScope();
}

TEST_F(SymbolTableTest, SymbolTableErrorWhenOverwrittingExistingValue) 
{
    table->BeginScope();
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4);
    // overwrites original mapping
    EXPECT_THROW(table->Insert(SymbolFactory::GenerateSymbol("a"), 5), SemanticAnalysisException);
    ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 4);
    table->EndScope();
}

TEST_F(SymbolTableTest, SymbolTableRemovedFromTableWhenOutOfScope)
{
    table->BeginScope();
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4);
    table->EndScope();
    ASSERT_FALSE(table->LookUp(SymbolFactory::GenerateSymbol("a")));
}

TEST_F(SymbolTableTest, SymbolTableShadowedWhenNewScope)
{
    table->BeginScope();
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4);
    {
        table->BeginScope();
        table->Insert(SymbolFactory::GenerateSymbol("a"), 5);
        ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 5);
        table->EndScope();
    }
    table->EndScope();
}

TEST_F(SymbolTableTest, SymbolTableBackToOrigianlWhenBackToOriginalScope)
{
    table->BeginScope();
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4);
    {    
        table->BeginScope();
        table->Insert(SymbolFactory::GenerateSymbol("a"), 5);
        ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 5);
        table->EndScope();
    }
    ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 4);
    table->EndScope();
}

TEST_F(SymbolTableTest, SymbolTableCanGetValueFromOuterScopeIfNotShadowed)
{
    table->BeginScope();
    table->Insert(SymbolFactory::GenerateSymbol("a"), 4);
    {    
        table->BeginScope();
        table->Insert(SymbolFactory::GenerateSymbol("b"), 5);
        ASSERT_EQ(table->LookUp(SymbolFactory::GenerateSymbol("a")), 4);
        table->EndScope();
    }
    table->EndScope();
}


