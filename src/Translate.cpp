#include "Translate.h"

using namespace Translate;

Level::Level(const std::shared_ptr<Level>& parent, const Temps::Label& label, const std::vector<bool>& formals)
    : m_parentLevel(parent)
    , m_frame(FrameAccess::FrameFactory::MakeFrame(label, formals))
{}

const std::vector<Access>& Level::UseFormals() const
{
    throw CompilerErrorException("Not implemented");
}

Access Level::AllocateLocal(bool escapes)
{
    if (!m_frame)
    {
        throw CompilerErrorException("Uninitialized frame in level. Is this this the outermost level?");
    }
    std::shared_ptr<Level> level(this);
    return Access(level, m_frame->AllocateLocal(escapes));
}

