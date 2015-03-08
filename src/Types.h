#pragma once

#include "common.h"
#include "Symbol.h"
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

typedef uint64_t UniqueId;

template <typename TypeName>
struct UniqueIdTagged
{
    UniqueId id;
    TypeName type;
};

struct IntTy {};

struct StringTy {};

struct NilTy {};

struct UnitTy {};

typedef boost::make_recursive_variant<
      IntTy
    , StringTy
    , NilTy
    , UnitTy
    , UniqueIdTagged<std::vector<std::pair<Symbol, boost::recursive_variant_>>> // Record Type (((Symbol * Type) list) * id
    , UniqueIdTagged<boost::recursive_variant_>  // array type (Type * Id)
    , std::pair<Symbol, boost::optional<boost::recursive_variant_>> // Name type (Symbol * Type option ref) 
    >::type Type;

typedef std::vector<std::pair<Symbol, Type>> RecordTy;
typedef std::pair<Symbol, boost::optional<Type>> NameTy;

bool AreEqualTypes(const Type& t1, const Type& t2);

namespace Types
{
Type StripLeadingNameTypes(Type& type);
boost::optional<Type> GetFieldFromRecord(const Type& type, const Symbol& symbol);
bool IsRecordTypeWithMatchingFields(const Type& type, const RecordTy& fieldTypes, std::string& errorMsg);
bool IsArrayType(const Type& type);
boost::optional<Type> GetTypeOfArray(const Type& type);
}

class TypeFactory
{
public:
    static Type MakeIntType();
    static Type MakeStringType();
    static Type MakeNilType();
    static Type MakeUnitType();
    static Type MakeRecordType(const RecordTy& fields);
    static Type MakeArrayType(const Type& ty);
    static Type MakeEmptyNameType(const Symbol& name);
    static void AddTypeToName(Type& inNamedType, const Type& typeToTag);
private:
    static UniqueId GetNextId();
    static UniqueId m_next_id;
};
