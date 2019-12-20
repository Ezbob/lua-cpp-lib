
#ifndef _HEADER_FILE_luastate_20191209191857_
#define _HEADER_FILE_luastate_20191209191857_

#include <memory>
#include <string>
#include <iostream>
#include <functional>
#include "lua.hpp"
#include "luaref.hpp"
#include "luaindex.hpp"
#include "luaexception.hpp"

class LuaState {

public:
    LuaState() {
        m_state = std::shared_ptr<lua_State>(luaL_newstate(), lua_close);
    }

    LuaState(std::shared_ptr<lua_State> p) {
        m_state = p;
    }

    virtual ~LuaState() = default;

    operator lua_State *() {
        return m_state.get();
    }

    bool checkError(int return_code) {
        if (return_code != LUA_OK) {
            std::string errormsg = lua_tostring(m_state.get(), -1);
            std::cerr << errormsg << std::endl;
            return false;
        }
        return true;
    }

    void loadFile(const char *path) {
        if ( luaL_dofile(m_state.get(), path) != LUA_OK ) {
            throw LuaException(lua_tostring(m_state.get(), STACK_TOP));
        }
    }

    void loadFile(const std::string &path) {
        loadFile(path.c_str());
    }

    void push(void) {
        lua_pushnil(m_state.get());
    }

    void push(double val) {
        lua_pushnumber(m_state.get(), val);
    }

    void push(int val) {
        lua_pushnumber(m_state.get(), val);
    }

    void push(bool val) {
        lua_pushboolean(m_state.get(), val);
    }

    void push(const char *val) {
        lua_pushstring(m_state.get(), val);
    }
    
    void push(const std::string val) {
        lua_pushstring(m_state.get(), val.c_str());
    }

    LuaStringRef getString(LuaStackIndex i = STACK_TOP) {
        lua_tostring(m_state.get(), i.get());
        return LuaStringRef(m_state, size());
    }

    LuaNumberRef getNumber(LuaStackIndex i = STACK_TOP) {
        lua_tonumber(m_state.get(), i.get());
        return LuaNumberRef(m_state, lua_gettop(m_state.get()));
    }

    LuaBooleanRef getBoolean(LuaStackIndex i = STACK_TOP) {
        lua_toboolean(m_state.get(), i.get());
        return LuaBooleanRef(m_state, lua_gettop(m_state.get()));
    }

    LuaStringRef getString(const std::string &name) {
        lua_getglobal(m_state.get(), name.c_str());
        return LuaStringRef(m_state, top());
    }

    LuaNumberRef getNumber(const std::string &name) {
        lua_getglobal(m_state.get(), name.c_str());
        return LuaNumberRef(m_state, top());
    }

    LuaBooleanRef getBoolean(const std::string &name) {
        lua_getglobal(m_state.get(), name.c_str());
        return LuaBooleanRef(m_state, top());
    }

    LuaFunctionRef getFunction(const std::string &name, const int input = 0, const int output = 0) {
        lua_getglobal(m_state.get(), name.c_str());
        return LuaFunctionRef(m_state, top(), input, output);
    }

    LuaTableRef getTable(const std::string &name) {
        lua_getglobal(m_state.get(), name.c_str());
        return LuaTableRef(m_state, top());
    }

    void pop(int number_of_elements = 1) {
        lua_pop(m_state.get(), number_of_elements);
    }

    int size() const {
        return lua_gettop(m_state.get());
    }

    LuaStackIndex top() const {
        return lua_gettop(m_state.get());
    }

    void runContext(std::function<void()> &&f) {
        int t = size(), s;
        f();
        s = size() - t;
        if (s > 0) pop(s);
    }

private:
    std::shared_ptr<lua_State> m_state;
};

void stack_debug_print(LuaState &stack) {
    auto top = stack.size();
    std::cout << "DEBUG Stack size " << top << "\n";
    if (top > 0) {
        std::cout << "DEBUG Stack content: \n";
    }
    for (int i = top; i > 0; --i) {
        std::cout << "DEBUG index(" << i << ", " << -((top - i) + 1) << ") => " << type_to_string(lua_type(stack, i)) << "\n";
    }
}



#endif