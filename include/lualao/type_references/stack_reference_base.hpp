
#pragma once

#include <memory>
#include "lua.h"
#include "lualao/stack_index.hpp"

namespace lualao {

    class stack_reference_base {
      private:
        int m_type;

      protected:
        std::shared_ptr<lua_State> m_parent;
        stack_index m_index;

      public:
        stack_reference_base(std::shared_ptr<lua_State> s, stack_index i,
                             int type)
            : m_type(type)
            , m_parent(s)
            , m_index(i.get()) {}

        virtual ~stack_reference_base() = default;

        bool isValid() {
            lua_State *state = m_parent.get();
            int current_top = lua_gettop(state);
            bool is_not_empty_stack = current_top != 0;
            bool is_index_within_bounds =
                m_index.isFromBottom() && m_index <= current_top;
            bool do_type_match = lua_type(state, m_index) == m_type;
            return is_not_empty_stack && is_index_within_bounds &&
                   do_type_match;
        }

        operator bool() {
            return isValid();
        }
    };

}; // namespace lualao
