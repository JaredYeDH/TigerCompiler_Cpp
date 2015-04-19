#pragma once
#include "Frame.h"

namespace Translate
{

class Level;

class Access
{
public:
    Access(const std::shared_ptr<Level>& level, FrameAccess::Access access);

private:
    const std::shared_ptr<Level> m_level;
    FrameAccess::Access m_frameAccess;
};

class Level
{
public:
    Level() : m_isOutermost(true) {}
    Level(const std::shared_ptr<Level>& parent, const Temps::Label& label, const std::vector<bool>& formals);
    const std::vector<Access>& UseFormals() const;
    Access AllocateLocal(bool escapes);
private:
    const bool m_isOutermost = false;
    const std::shared_ptr<Level> m_parentLevel;
    std::unique_ptr<FrameAccess::Frame> m_frame;
};

}
