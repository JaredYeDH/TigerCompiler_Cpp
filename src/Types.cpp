#include "Types.h"
#include "Environments.h"

UniqueId TypeFactory::m_next_id{0};

class StripLeadingNameVisitor
    : public boost::static_visitor<Type>
{
public:
    Type operator()(const NameTy& ty) const 
    {
        if (!ty.second)
        {
            std::string msg = "Internal Compiler error: Attempt to strip name off a type in StripLeadingNameVisitor when no underlying type exists. Name was: " + ty.first.UseName();
            throw CompilerErrorException(msg.c_str());
        }
        return Types::StripLeadingNameTypes(*ty.second);
    }

    template <typename T>
    Type operator()(T& ty) const
    {
        return ty;
    }
};

Type Types::StripLeadingNameTypes(const Type& type)
{
    return boost::apply_visitor(StripLeadingNameVisitor(), type);
}

Type TypeFactory::MakeRecordType(const RecordTy& fields)
{
    UniqueIdTagged<RecordTy> tagged;
    tagged.id = GetNextId();
    tagged.type = fields;
    return tagged;
}

Type TypeFactory::MakeArrayType(const Type& ty)
{
    UniqueIdTagged<Type> tagged;
    tagged.id = GetNextId();
    tagged.type = ty;
    return tagged;
}

Type TypeFactory::MakeEmptyNameType(const Symbol& name)
{
    NameTy ty{name, boost::none};
    return ty;
}

Type TypeFactory::MakeIntType()
{
    return Type{IntTy{}};
}

Type TypeFactory::MakeStringType()
{
    return Type{StringTy{}};
}

Type TypeFactory::MakeNilType()
{
    return Type{NilTy{}};
}
 
Type TypeFactory::MakeUnitType()
{
    return Type{UnitTy{}};
} 

UniqueId TypeFactory::GetNextId()
{
    return m_next_id++;
}

class AddNameVisitor
    : public boost::static_visitor<>
{
public:
    AddNameVisitor(const Type& typeToTag)
        : m_TypeToTag(typeToTag)
    {
        // empty
    }

    void operator()(NameTy& ty) const 
    {
        if (ty.second)
        {
            throw CompilerErrorException("attempt to re-tag name value");
        }
        ty.second = m_TypeToTag;
    }

    template<typename ty>
    void operator()(ty& t) const
    {
        throw CompilerErrorException("Attempt to tag a non-name type with an actual type");
    }

private:
    Type m_TypeToTag;
};

void TypeFactory::AddTypeToName(Type& inNamedType, const Type& typeToTag)
{
    boost::apply_visitor(AddNameVisitor(typeToTag), inNamedType);
}

class AreEqualTypesVisitor
    : public boost::static_visitor<bool>
{
public:
    bool operator()(const UniqueIdTagged<RecordTy>& lhs, const NilTy& rhs) const
    {
        return true;
    }

    bool operator()(const NilTy& rhs, const UniqueIdTagged<RecordTy>& lhs) const
    {
        return true;
    }

    bool operator()(const UniqueIdTagged<RecordTy>& lhs, const UniqueIdTagged<RecordTy>& rhs) const
    {
        if (lhs.id != rhs.id)
        {
            return false;
        }
        return true;
    }

    bool operator()(const UniqueIdTagged<Type>& lhs, const UniqueIdTagged<Type>& rhs) const
    {
        // The id check should be sufficient, but let's keep it here for completeness.
        // At least until there is a good reason to remove it.
        return lhs.id == rhs.id && AreEqualTypes(lhs.type, rhs.type);
    }

    bool operator()(const NameTy& lhs, const NameTy& rhs) const
    {
        // TODO - Should two nametypes, where the names match but the tagged types don't match even be possible?
        if (lhs.first != rhs.first)
        {
            return false;
        }
        if (lhs.second)
        {
            if (!rhs.second)
            {
                return false;
            }
            return AreEqualTypes(*(lhs.second), (*rhs.second));
        }
        else if (rhs.second)
        {
            // lhs is not tagged but this side is
            return false;
        }
        return true;
    }

    template<typename T, typename U>
    bool operator()(const T&, const U&) const
    {
        return false;
    }

   template<typename T>
   bool operator()(const T&, const T&) const
   {
       return true;
   }
};

bool AreEqualTypes(const Type& t1, const Type& t2)
{
    return boost::apply_visitor(AreEqualTypesVisitor(), t1, t2);
}

class GetFieldFromRecordVisitor
    : public boost::static_visitor<boost::optional<Type>>
{
public:
    GetFieldFromRecordVisitor(const Symbol& sym)
        : symbol(sym)
    {}
    
    boost::optional<Type> operator()(const UniqueIdTagged<RecordTy>& record) const
    {
        boost::optional<Type> ty;
        RecordTy rec = record.type;
        for (const auto& pair : rec)
        {
            if (pair.first == symbol)
            {
                ty = pair.second;
                break;
            }
        }
        return ty;
    }

   template<typename T>
   boost::optional<Type> operator()(const T&) const
   {
       return nullptr;
   }

private:
   Symbol symbol;
};

boost::optional<Type> Types::GetFieldFromRecord(const Type& type, const Symbol& symbol)
{
    return boost::apply_visitor(GetFieldFromRecordVisitor(symbol), type);
}

class IsArrayTypeVisitor
    : public boost::static_visitor<bool>
{
public:
    bool operator()(const UniqueIdTagged<RecordTy>& record) const
    {
        return false;
    }

    template<typename T>
    bool operator()(const UniqueIdTagged<T>& record) const
    {
        return true;
    }

   template<typename T>
   bool operator()(const T&) const
   {
       return false;
   }
};

bool Types::IsArrayType(const Type& type)
{
    return boost::apply_visitor(IsArrayTypeVisitor(), type);
}

class GetTypeOfArrayVisitor
    : public boost::static_visitor<boost::optional<Type>>
{
public:
    boost::optional<Type>operator()(const UniqueIdTagged<RecordTy>& record) const
    {
        return nullptr;
    }

    template<typename T>
    boost::optional<Type> operator()(const UniqueIdTagged<T>& record) const
    {
        return record.type;
    }

   template<typename T>
   boost::optional<Type> operator()(const T&) const
   {
       return nullptr;
   }
};

boost::optional<Type> Types::GetTypeOfArray(const Type& type)
{
    return boost::apply_visitor(GetTypeOfArrayVisitor(), type);
}

class IsRecordTypeWithMatchingFieldsVisitor
    : public boost::static_visitor<bool>
{
public:
    IsRecordTypeWithMatchingFieldsVisitor(const RecordTy& fields, const std::shared_ptr<TypeEnvironment>& env, ErrorCode& errorCode, std::string& errorMsg)
        : m_fields(fields)
        , m_env(env)
        , m_errorCode(errorCode)
        , m_errorMsg(errorMsg)
    {
    }

    bool operator()(const UniqueIdTagged<RecordTy>& record) const
    {
        if (record.type.size() != m_fields.size())
        {
            m_errorCode = ErrorCode::Err26;
            return false;
        }

        for (unsigned int i = 0; i < record.type.size(); i++)
        {
            auto recTy = Types::StripLeadingNameTypes(record.type[i].second);
            auto fieldTy = m_fields[i].second;
            if (Types::IsNameType(fieldTy))
            {
                auto lookup = m_env->LookUp(Types::GetSymbolFromNameType(m_fields[i].second));
                if (!lookup)
                {
                    throw CompilerErrorException("Record uses a name type that was not filled out");
                }
                fieldTy = (*lookup)->UseType();
            }

            if (!AreEqualTypes(recTy, fieldTy))
            {
                m_errorCode = ErrorCode::Err27;
                std::string msg  = "Expected: " + record.type[i].first.UseName() + " saw: " + m_fields[i].first.UseName();
                m_errorMsg = msg;
                return false;
            }
            if (record.type[i].first != m_fields[i].first)
            {
                m_errorCode = ErrorCode::Err28;
                return false;
            }
        }

        return true;
    }

    template<typename T>
    bool operator()(const T&) const
    {
        m_errorMsg = "Type is of non record type";
        m_errorCode = ErrorCode::Err29;
        return false;
    }

private:
    const RecordTy& m_fields;
    const std::shared_ptr<TypeEnvironment> m_env;
    ErrorCode& m_errorCode;
    std::string& m_errorMsg;
};

bool Types::IsRecordTypeWithMatchingFields(const Type& type, const RecordTy& fieldTypes, const std::shared_ptr<TypeEnvironment>& env, ErrorCode& errorCode, std::string& errorMsg)
{
    return boost::apply_visitor(IsRecordTypeWithMatchingFieldsVisitor(fieldTypes, env, errorCode, errorMsg), type);
}

class IsNameTypeVisitor
    : public boost::static_visitor<bool>
{
public:
    bool operator()(const NameTy&) const
    {
        return true;
    }

    template<typename T>
    bool operator()(const T&) const
    {
        return false;
    }
};
 
bool Types::IsNameType(const Type& type)
{
    return boost::apply_visitor(IsNameTypeVisitor(), type);
}

class FillNameTypesVisitor
    : public boost::static_visitor<bool>
{
public:
    FillNameTypesVisitor(
            const std::shared_ptr<TypeEnvironment>& env,
            ErrorCode& errorCode, std::string& errorMsg)
        : m_env(env)
        , m_errorCode(errorCode)
        , m_errorMsg(errorMsg)
        {
        }

    bool operator()(UniqueIdTagged<RecordTy>& record) const
    {
        for (auto& field : record.type)
        {
            if (Types::IsNameType(field.second))
            {
                auto ty = m_env->LookUp(Types::GetSymbolFromNameType(field.second));
                if (!ty || !*ty)
                {
                    throw CompilerErrorException("Attempt to fill in name types when not all name types are valid");
                }
                
                TypeFactory::AddTypeToName(field.second, (*ty)->UseType());
            }
        }
        return true;
    }

    template <typename T>
    bool operator()(T&) const
    {
        m_errorCode = ErrorCode::Err29;
        return false;
    }

private:
    std::shared_ptr<TypeEnvironment> m_env;
    ErrorCode& m_errorCode;
    std::string& m_errorMsg;
};

bool Types::FillNameTypes(Type& type, const std::shared_ptr<TypeEnvironment>& env, ErrorCode& errorCode, std::string& errorMsg)
{
    return boost::apply_visitor(FillNameTypesVisitor(env, errorCode, errorMsg), type);
}

class GetSymbolFromNameTypeVisitor
    : public boost::static_visitor<Symbol>
{
public:
    Symbol operator()(const NameTy& type) const
    {
        return type.first;
    }

    template <typename T>
    Symbol operator()(const T&) const
    {
        throw CompilerErrorException("Attempt to get symbol from non-name type");
    }
};

Symbol Types::GetSymbolFromNameType(const Type& type)
{
    return boost::apply_visitor(GetSymbolFromNameTypeVisitor(), type);
}

class IsStrictlyNilVisitor
    : public boost::static_visitor<bool>
{
public:
    bool operator()(const NilTy& ty) const
    {
        return true;
    }

    template <typename T>
    bool operator()(const T&) const
    {
        return false;
    }
};

bool Types::IsStrictlyNil(const Type& type)
{
    return boost::apply_visitor(IsStrictlyNilVisitor(), type);
}

class TypeStringVisitor
    : public boost::static_visitor<std::string>
{
public:
    std::string operator()(const IntTy&) const
    {
        return "int";
    }

    std::string operator()(const StringTy&) const
    {
        return "string";
    }

    std::string operator()(const NilTy&) const
    {
        return "nil";
    }
    
    std::string operator()(const UnitTy&) const
    {
        return "unit";
    }

    std::string operator()(const UniqueIdTagged<RecordTy>& rec) const
    {
        return "TODO: record";
    }

    template<typename T>
    std::string operator()(const UniqueIdTagged<T>& array) const
    {
        return "array of " + Types::TypeString(array.type);
    }

    std::string operator()(const NameTy& name) const
    {
        return "TODO: nameTy";
    }
};

std::string Types::TypeString(const Type& type)
{
    return boost::apply_visitor(TypeStringVisitor(), type);
}
