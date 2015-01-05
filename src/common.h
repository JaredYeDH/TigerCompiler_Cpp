#pragma once
#include <memory>
#include <exception>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class CompilerErrorException
    : public std::exception
{
public:
    CompilerErrorException(const char* message)
        : m_message(message)
    {}

    virtual const char* what() const throw() override
    {
        return m_message.c_str();
    }

private:
    std::string m_message;
};

