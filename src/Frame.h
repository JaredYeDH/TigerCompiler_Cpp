#pragma once
#include "Temps.h"
#include "boost/optional.hpp"

namespace FrameAccess
{

enum class AccessKind : uint8_t
{
    InFrame,
    InRegister
};

class Access
{
public:
    Access(int32_t offest)
        : m_kind(AccessKind::InFrame)
        , m_offset(offest)
    { /* empty */ }

    Access(Temps::LocalTemp tempRegister)
        : m_kind(AccessKind::InRegister)
        , m_tempRegister(tempRegister)
    { /* empty */ }

    int32_t GetOffset() const
    {
        if (m_kind != AccessKind::InFrame)
        {
            throw CompilerErrorException("Invalid use of InRegister access");
        }
        return m_offset;
    }

    const Temps::LocalTemp& UseTempRegister()
    {
        if (m_kind != AccessKind::InRegister || !m_tempRegister)
        {
            throw CompilerErrorException("Invalid use of InFrame access");
        }
        return *m_tempRegister;
    }

    AccessKind GetKind()
    {
        return m_kind;
    }

private:
    Access() = delete;
    AccessKind m_kind;
    boost::optional<Temps::LocalTemp> m_tempRegister;
    int32_t m_offset;
};

class Frame
{
public:
    virtual Temps::Label GetName() const = 0;
    virtual const std::vector<Access>& UseFormals() const = 0;
    virtual Access AllocateLocal(bool escapes) = 0;
};

namespace FrameFactory
{
    std::unique_ptr<Frame> MakeFrame(const Temps::Label& name, const std::vector<bool>& formals);
}

}
