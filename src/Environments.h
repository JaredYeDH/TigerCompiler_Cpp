#pragma once

#include "common.h"
#include "Types.h"
#include "SymbolTable.h"

struct EnvEntry
{
    virtual ~EnvEntry(){};
    virtual Type GetType() const = 0;
    virtual bool IsFunction() const = 0;
    virtual const std::vector<Type>& UseFormals() const = 0;
};

struct VarEntry
    : public EnvEntry
{
    Type type;

    VarEntry(const Type& ty)
        : type(ty)
    {}

    Type GetType() const override
    {
        return type;
    }

    bool IsFunction() const override
    {
        return false;
    }

    const std::vector<Type>& UseFormals() const override
    {
        throw CompilerErrorException("Can't UseFormals for non function");
    }

    bool operator==(const VarEntry& other)
    {
        return AreEqualTypes(type, other.type);
    }
    bool operator!=(const VarEntry& other)
    {
        return !AreEqualTypes(type, other.type);
    } 
};

struct FunEntry
    : public EnvEntry
{
    std::vector<Type> formals;
    Type result;

    Type GetType() const override
    {
        return result;
    }

    bool IsFunction() const override
    {
        return true;
    }

    const std::vector<Type>& UseFormals() const override
    {
        return formals;
    }

    FunEntry(const std::vector<Type>& forms, const Type& ty)
        : formals(forms)
        , result(ty)
    {}
    
    bool operator==(const FunEntry& other)
    {
        if (!AreEqualTypes(result, other.result))
        {
            return false;
        }
        if (formals.size() != other.formals.size())
        {
            return false;
        }
        for (auto i = 0u; i< formals.size(); ++i)
        {
            if (!AreEqualTypes(formals[i], other.formals[i]))
            {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const FunEntry& other)
    {
        return !(*this == other);
    } 
};

class EnvType
{
public:
    EnvType(Type ty)
        : m_type(ty)
    {
    }

    const Type& UseType()
    {
        return m_type;
    }

    bool operator==(const EnvType& other) const
    {
        return AreEqualTypes(m_type, other.m_type);
    }

    bool operator!=(const EnvType& other) const
    {
        return !AreEqualTypes(m_type, other.m_type);
    }

    void ReplaceNameTypeWithType(const Type& typeToAdd)
    {
        if (!Types::IsNameType(m_type))
        {
            // this is may get hit if there was an invalid shadow
            return;
        }
        m_type = typeToAdd;
    }

private:
    Type m_type;
};

class TypeEnvironment
    : public SymbolTable<std::shared_ptr<EnvType>>
{
public:
    static std::shared_ptr<TypeEnvironment> GenerateBaseTypeEnvironment();
    ~TypeEnvironment()
    {
        EndScope();
    }
};

class ValueEnvironment
    : public SymbolTable<std::shared_ptr<EnvEntry>>
{
public:
    static std::shared_ptr<ValueEnvironment> GenerateBaseValueEnvironment();
    ~ValueEnvironment()
    {
        EndScope();
    }
};
