
#ifndef _HEADER_FILE_luacontext_20191218211150_
#define _HEADER_FILE_luacontext_20191218211150_

#include "lua.h"
#include <memory>
#include "state.hpp"

namespace lualao {

    class stack_context {
      private:
        state m_state;
        int m_top;

      public:
        stack_context(state s)
            : m_state(s) {
            m_top = lua_gettop(s);
        }

        ~stack_context() {
            int size = lua_gettop(m_state) - m_top;
            if (size > 0)
                lua_pop(m_state, size);
        }

        state get_state() {
            return m_state;
        }
    };

};

#endif