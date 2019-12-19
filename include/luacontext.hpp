
#ifndef _HEADER_FILE_luacontext_20191218211150_
#define _HEADER_FILE_luacontext_20191218211150_

#include "lua.h"
#include <memory>
#include "luastate.hpp"

class LuaStackContext {
    LuaState m_state;
    int m_top;
public:
    LuaStackContext(LuaState s) : m_state(s) {
        m_top = lua_gettop(s);
    }

    ~LuaStackContext() {
        int size = lua_gettop(m_state) - m_top;
        if (size > 0) lua_pop(m_state, size);
    }

    LuaState getState() {
        return m_state;
    }
};

#endif