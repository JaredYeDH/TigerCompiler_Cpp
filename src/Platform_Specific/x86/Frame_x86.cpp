#include "Frame.h"

#include "common.h"

using namespace Temps;
using namespace std;

class Access
{
};

class X86Frame
    : public Frame
{
public:
    X86Frame(const Label& name, const vector<bool>& formals)
        : m_label(name)
        , m_formals(formals)
    { /* empty */ }

    Temps::Label GetName() override
    {
        return m_label;
    }

    const std::vector<Access>& UseFormals() override { throw 1; }
    Access AllocateLocal(bool escapes) override { throw 1; }

private:
    Label m_label;
    vector<bool> m_formals;
};

std::unique_ptr<Frame> FrameFactory::MakeFrame(const Temps::Label& name, const std::vector<bool>& formals)
{
    return make_unique<X86Frame>(name, formals);
}
