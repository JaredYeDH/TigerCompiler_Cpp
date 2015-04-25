#include "Translate.h"

using namespace Translate;

class Level
    : public ILevel
    , public std::enable_shared_from_this<Level>
{
public:
    Level(const std::shared_ptr<ILevel>& parent, const Temps::Label& label, const std::vector<bool>& formals);

    const std::vector<Access>& UseFormals() const override;
    Access AllocateLocal(bool escapes) override;
    const std::shared_ptr<ILevel>& GetParent() const override
    {
        return m_parentLevel;
    }

private:
    const std::shared_ptr<ILevel> m_parentLevel;
    std::unique_ptr<FrameAccess::Frame> m_frame;
    mutable std::vector<Access> m_formals;
};

std::shared_ptr<ILevel> Translate::NewLevel(const std::shared_ptr<ILevel>& parent, const Temps::Label& label, const std::vector<bool>& formals)
{
    std::vector<bool> formalsWithStaticLink{formals};
    formalsWithStaticLink.push_back(true);
    return std::make_shared<Level>(parent, label, formalsWithStaticLink);
}

Level::Level(const std::shared_ptr<ILevel>& parent, const Temps::Label& label, const std::vector<bool>& formals)
    : m_parentLevel(parent)
    , m_frame(FrameAccess::FrameFactory::MakeFrame(label, formals))
{}

const std::vector<Access>& Level::UseFormals() const
{
    if (m_formals.empty())
    {
        auto frameFormals = m_frame->UseFormals();
        for (const auto& formal : frameFormals)
        {
            m_formals.push_back(Access{shared_from_this(), formal});
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
    return Access(shared_from_this(), m_frame->AllocateLocal(escapes));
}
