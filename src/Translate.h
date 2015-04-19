#pragma once
#include "Frame.h"

namespace Translate
{

class Level;

class Access
{
public:
    Access(const std::shared_ptr<const Level>& level, FrameAccess::Access access)
        : m_level(level)
        , m_frameAccess(access) {}

private:
    const std::shared_ptr<const Level> m_level;
    FrameAccess::Access m_frameAccess;
};

class Level
    : public std::enable_shared_from_this<Level>
{
public:
    Level(const std::shared_ptr<Level>& parent, const Temps::Label& label, const std::vector<bool>& formals);
    const std::vector<Access>& UseFormals() const;
    Access AllocateLocal(bool escapes);
    const std::shared_ptr<Level>& GetParent()
    {
        return m_parentLevel;
    }
private:
    const std::shared_ptr<Level> m_parentLevel;
    std::unique_ptr<FrameAccess::Frame> m_frame;
    mutable std::vector<Access> m_formals;
};

}
