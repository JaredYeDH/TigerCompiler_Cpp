#pragma once

#include "common.h"
#include "Types.h"
#include "SymbolTable.h"
#include "Translate.h"

struct EnvEntry
{
    virtual ~EnvEntry(){};
    virtual Type GetType() const = 0;
    virtual bool IsFunction() const = 0;
    virtual const std::vector<Type>& UseFormals() const = 0;
    virtual Translate::Access GetAccess() const = 0;
    virtual std::shared_ptr<const Translate::ILevel> GetLevel() const = 0;
    virtual const Temps::Label& UseLabel() const = 0;
};

class VarEntry
    : public EnvEntry
{
public:
    VarEntry(const Type& ty, const Translate::Access& acc)
        : type(ty)
        , access(acc)
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

    Translate::Access GetAccess() const override
    {
        return access;
    }

    std::shared_ptr<const Translate::ILevel> GetLevel() const override
    {
        throw CompilerErrorException("Can't GetLevel for non function");
    }

    const Temps::Label& UseLabel() const override
    {
        throw CompilerErrorException("Can't UseLabel for non function");
    }

    bool operator==(const VarEntry& other)
    {
        return AreEqualTypes(type, other.type);
    }
    bool operator!=(const VarEntry& other)
    {
        return !AreEqualTypes(type, other.type);
    } 

private:
    Type type;
    Translate::Access access;
    VarEntry() = delete;
};

class FunEntry
    : public EnvEntry
{
public:
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

    Translate::Access GetAccess() const override
    {
        throw CompilerErrorException("Can't GetAccess for function type");
    }

    std::shared_ptr<const Translate::ILevel> GetLevel() const override
    {
        return m_level;
    }

    const Temps::Label& UseLabel() const override
    {
        return m_label;
    }

    FunEntry(const std::vector<Type>& forms, const Type& ty, const std::shared_ptr<const Translate::ILevel>& level, const Temps::Label& label)
        : formals(forms)
        , result(ty)
        , m_level(level)
        , m_label(label)
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
private:
    std::vector<Type> formals;
    Type result;
    std::shared_ptr<const Translate::ILevel> m_level;
    Temps::Label m_label;
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
