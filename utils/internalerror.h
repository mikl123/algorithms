#pragma once

#include <exception>
#include <string>

namespace Utils
{
/**
 * @brief this error indicates the error in template implementation. It is expected
 * to not to be thrown
 */
class InternalError : public std::exception
{
public:
    InternalError(std::string message) : _message{ std::move(message) } {}

    const char *what() const noexcept override { return _message.data(); }

private:
    std::string _message;
};

}    // namespace Utils
