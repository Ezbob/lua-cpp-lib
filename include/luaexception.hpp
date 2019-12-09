
#ifndef _HEADER_FILE_luaexception_20191209191527_
#define _HEADER_FILE_luaexception_20191209191527_

#include <exception>
#include <string>

class LuaException : public std::runtime_error {
public:
    LuaException(const std::string &message) : std::runtime_error(message) {}
    LuaException(const char *message) : std::runtime_error(message) {}

    virtual ~LuaException() = default;
};

#endif