
#pragma once

#include <memory>
#include <string>
#include "lua.h"
#include "lualao/stack_index.hpp"
#include "lualao/type.hpp"
#include "stack_reference_base.hpp"
#include "string_reference.hpp"
#include "boolean_reference.hpp"
#include "number_reference.hpp"
#include "function_reference.hpp"

namespace lualao {

    struct table_reference: public stack_reference_base {
        table_reference(std::shared_ptr<lua_State> s, stack_index i)
            : stack_reference_base(s, i, type::TABLE_TYPE) {}
        virtual ~table_reference() = default;

        string_reference getString(const std::string &name) {
            lua_pushstring(m_parent.get(), name.c_str());
            lua_gettable(m_parent.get(), m_index.get());
            return string_reference(m_parent, lua_gettop(m_parent.get()));
        }

        boolean_reference getBoolean(const std::string &name) {
            lua_pushstring(m_parent.get(), name.c_str());
            lua_gettable(m_parent.get(), m_index.get());
            return boolean_reference(m_parent, lua_gettop(m_parent.get()));
        }

        number_reference getNumber(const std::string &name) {
            lua_pushstring(m_parent.get(), name.c_str());
            lua_gettable(m_parent.get(), m_index.get());
            return number_reference(m_parent, lua_gettop(m_parent.get()));
        }

        function_reference getFunction(const std::string &name,
                                       const int input = 0,
                                       const int output = 0) {
            lua_pushstring(m_parent.get(), name.c_str());
            lua_gettable(m_parent.get(), m_index.get());
            return function_reference(m_parent, lua_gettop(m_parent.get()),
                                      input, output);
        }

        void set(std::string const &name, std::string value) {
            lua_pushstring(m_parent.get(), name.c_str());
            lua_pushstring(m_parent.get(), value.c_str());
            lua_settable(m_parent.get(), m_index);
        }

        void set(std::string const &name, double value) {
            lua_pushstring(m_parent.get(), name.c_str());
            lua_pushnumber(m_parent.get(), value);
            lua_settable(m_parent.get(), m_index);
        }

        void set(std::string const &name, bool value) {
            lua_pushstring(m_parent.get(), name.c_str());
            lua_pushboolean(m_parent.get(), value);
            lua_settable(m_parent.get(), m_index);
        }

        void set(std::string const &name, lua_CFunction f) {
            lua_pushstring(m_parent.get(), name.c_str());
            lua_pushcfunction(m_parent.get(), f);
            lua_settable(m_parent.get(), m_index);
        }
    };

}; // namespace lualao
