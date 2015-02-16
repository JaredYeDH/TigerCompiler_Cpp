#include "gtest/gtest.h"
#include "Types.h"

class TypeTest : public ::testing::Test
{
};
TEST_F(TypeTest, Make_Int_Type)
{
    Type t = TypeFactory::MakeIntType();
}

TEST_F(TypeTest, Make_String_Type)
{
    Type t = TypeFactory::MakeStringType();
}

TEST_F(TypeTest, Make_Nil_Type)
{
    Type t = TypeFactory::MakeNilType();
}

TEST_F(TypeTest, Make_Unit_Type)
{
    Type t = TypeFactory::MakeUnitType();
}


TEST_F(TypeTest, Make_Record_Type)
{
    std::pair<Symbol,Type> field = std::make_pair(SymbolFactory::GenerateSymbol("a"), Type{IntTy{}});
    RecordTy fields = {field};
    Type t = TypeFactory::MakeRecordType(fields);
}

TEST_F(TypeTest, Make_Array_Type)
{
    Type t = TypeFactory::MakeArrayType(Type{StringTy{}});
}

TEST_F(TypeTest, Make_Name_Type)
{
    Type t = TypeFactory::MakeEmptyNameType(SymbolFactory::GenerateSymbol("a"));
}

TEST_F(TypeTest, AddTypeToNameType)
{
    Type t = TypeFactory::MakeEmptyNameType(SymbolFactory::GenerateSymbol("a"));
    TypeFactory::AddTypeToName(t, TypeFactory::MakeIntType());
    // TODO - Check
}

TEST_F(TypeTest, AddTypeToNonNameType_Failure)
{
    Type t = TypeFactory::MakeArrayType(TypeFactory::MakeIntType());
    ASSERT_THROW(TypeFactory::AddTypeToName(t, TypeFactory::MakeIntType()), CompilerErrorException);
    // TODO - Check
}

TEST_F(TypeTest, AddTypeToNameTypeAlreadyTagged_Fails)
{
    Type t = TypeFactory::MakeEmptyNameType(SymbolFactory::GenerateSymbol("a"));
    TypeFactory::AddTypeToName(t, TypeFactory::MakeIntType());
    // TODO - Check

    ASSERT_THROW(TypeFactory::AddTypeToName(t, TypeFactory::MakeStringType()), CompilerErrorException);
}

TEST_F(TypeTest, Int_TypeEquality)
{
    Type t = TypeFactory::MakeIntType();
    Type t1 = TypeFactory::MakeIntType();
    ASSERT_TRUE(AreEqualTypes(t, t1));
}

TEST_F(TypeTest, String_TypeEquality)
{
    Type t = TypeFactory::MakeStringType();
    Type t1 = TypeFactory::MakeStringType();
    ASSERT_TRUE(AreEqualTypes(t, t1));
}

TEST_F(TypeTest, Nil_TypeEquality)
{
    Type t = TypeFactory::MakeNilType();
    Type t1 = TypeFactory::MakeNilType();
    ASSERT_TRUE(AreEqualTypes(t, t1));
}

TEST_F(TypeTest, Unit_TypeEquality)
{
    Type t = TypeFactory::MakeUnitType();
    Type t1 = TypeFactory::MakeUnitType();
    ASSERT_TRUE(AreEqualTypes(t, t1));
}

TEST_F(TypeTest, Record_TypeEquality)
{
    std::pair<Symbol,Type> field = std::make_pair(SymbolFactory::GenerateSymbol("a"), TypeFactory::MakeIntType());
    RecordTy fields = {field};
    Type t = TypeFactory::MakeRecordType(fields);
    Type t1 = TypeFactory::MakeRecordType(fields);
    // non-matching ids
    ASSERT_FALSE(AreEqualTypes(t, t1));
}

TEST_F(TypeTest, Record_TypeEqualitySuccess)
{
    std::pair<Symbol,Type> field = std::make_pair(SymbolFactory::GenerateSymbol("a"), TypeFactory::MakeIntType());
    RecordTy fields = {field};
    Type t = TypeFactory::MakeRecordType(fields);
    ASSERT_TRUE(AreEqualTypes(t, t));
} 

TEST_F(TypeTest, Array_TypeEquality)
{
    Type t = TypeFactory::MakeArrayType(TypeFactory::MakeStringType());
    Type t1 = TypeFactory::MakeArrayType(TypeFactory::MakeStringType());
    // Non-matching ids
    ASSERT_FALSE(AreEqualTypes(t, t1));
}

TEST_F(TypeTest, Array_TypeEqualitySuccess)
{
    Type t = TypeFactory::MakeArrayType(TypeFactory::MakeStringType());
    // Non-matching ids
    ASSERT_TRUE(AreEqualTypes(t, t));
}

TEST_F(TypeTest, Name_TypeEquality)
{
    Type t = TypeFactory::MakeEmptyNameType(SymbolFactory::GenerateSymbol("a"));
    Type t1 = TypeFactory::MakeEmptyNameType(SymbolFactory::GenerateSymbol("a"));
    ASSERT_TRUE(AreEqualTypes(t, t1));
}

TEST_F(TypeTest, NameType_UnTaggedAndTagged_NotEqual)
{
    Type t = TypeFactory::MakeEmptyNameType(SymbolFactory::GenerateSymbol("a"));
    Type t1 = TypeFactory::MakeEmptyNameType(SymbolFactory::GenerateSymbol("a"));
    TypeFactory::AddTypeToName(t1, TypeFactory::MakeIntType());
    ASSERT_FALSE(AreEqualTypes(t, t1));
}
