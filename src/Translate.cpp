#include "Translate.h"

using namespace Translate;

Level::Level(const std::shared_ptr<Level>& parent, const Temps::Label& label, const std::vector<bool>& formals)
    : m_parentLevel(parent)
    , m_frame(FrameAccess::FrameFactory::MakeFrame(label, formals))
{}

const std::vector<Access>& Level::UseFormals() const
{
    if (m_formals.empty())
    {
        std::shared_ptr<const Level> level(this);
        auto frameFormals = m_frame->UseFormals();
        for (const auto formal : frameFormals)
        {
            m_formals.push_back(Access{level, formal});
        }
    }
    return m_formals;
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
