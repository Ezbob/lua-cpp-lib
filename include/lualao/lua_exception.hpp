
#pragma once

#include <exception>
#include <string>

namespace lualao {

    class lua_exception: public std::runtime_error {
      public:
        lua_exception(const std::string &message)
            : std::runtime_error(message) {}
        lua_exception(const char *message)
            : std::runtime_error(message) {}

        virtual ~lua_exception() = default;
    };

};
