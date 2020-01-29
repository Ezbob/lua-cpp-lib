
#pragma once

#include "lua.h"
#include <memory>
#include "stack_reference_base.hpp"
#include "lualao/stack_index.hpp"
#include "lualao/type.hpp"
#include "lualao/lua_exception.hpp"

namespace lualao {

    class function_reference: public stack_reference_base {
      private:
        int m_input;
        int m_output;

      public:
        function_reference(std::shared_ptr<lua_State> s, stack_index i,
                           const int input, const int output)
            : stack_reference_base(s, i, type::FUNCTION_TYPE)
            , m_input(input)
            , m_output(output) {}
        virtual ~function_reference() = default;

        void safeCall(int handlerIndex = 0) {
            if (isValid()) {
                int top = lua_gettop(m_parent.get());

                if ((top - m_index) < m_input) {
                    throw lua_exception(
                        "Not enough arguments parsed to function");
                }

                if (lua_pcall(m_parent.get(), m_input, m_output,
                              handlerIndex) != LUA_OK) {
                    throw lua_exception(
                        lua_tostring(m_parent.get(), STACK_TOP.get()));
                }
            }
        }

        function_reference &operator*() {
            return *this;
        }

        void operator()(int handlerIndex = 0) {
            safeCall(handlerIndex);
        }
    };

}; // namespace lualao
