#include "Types.h"

UniqueId TypeFactory::m_next_id{0};

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
    template<typename T, typename U>
    bool operator()(const T&, const U&) const
    {
        return false;
    }

    bool operator()(const UniqueIdTagged<RecordTy>& lhs, const UniqueIdTagged<RecordTy>& rhs) const
    {
        if (lhs.id != rhs.id)
        {
            return false;
        }
        // Strictly speaking, the id check should be all we need. But let's do
        // the whole check here for completeness, at least until we have a good
        // reason not too.
        if (lhs.type.size() != rhs.type.size())
        {
            return false;
        }
        for (auto i = 0u; i < lhs.type.size(); ++i)
        {
            if (lhs.type[i].first != rhs.type[i].first)
            {
                return false;
            }
            if (!AreEqualTypes(lhs.type[i].second, rhs.type[i].second))
            {
                return false;
            }
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
            return AreEqualTypes(lhs, rhs);
        }
        else if (rhs.second)
        {
            // lhs is not tagged but this side is
            return false;
        }
        return true;
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
