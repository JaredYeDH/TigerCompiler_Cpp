#pragma once
#include "Frame.h"

namespace Translate
{

class ILevel;

class Access
{
public:
    Access(const std::shared_ptr<const ILevel>& level, FrameAccess::Access access)
        : m_level(level)
        , m_frameAccess(access) {}

private:
    const std::shared_ptr<const ILevel> m_level;
    FrameAccess::Access m_frameAccess;
};

class ILevel
{
public:
    virtual const std::vector<Access>& UseFormals() const = 0;
    virtual Access AllocateLocal(bool escapes) = 0;
    virtual const std::shared_ptr<ILevel>& GetParent() const = 0;
};

std::shared_ptr<ILevel> NewLevel(const std::shared_ptr<ILevel>& parent, const Temps::Label& label, const std::vector<bool>& formals);

}
