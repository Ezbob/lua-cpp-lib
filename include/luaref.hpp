
#ifndef _HEADER_FILE_luaref_20191209191203_
#define _HEADER_FILE_luaref_20191209191203_

#include "lua.hpp"
#include <memory>
#include <string>
#include "luaindex.hpp"
#include "luaexception.hpp"
#include "luatype.hpp"
#include "luastate.hpp"

class LuaStackReferenceBase {
    LuaType m_type;
protected:
    std::shared_ptr<lua_State> m_parent;
    LuaStackIndex m_index;

    LuaStackReferenceBase(std::shared_ptr<lua_State> s, LuaStackIndex i, int type) 
        : m_type(LuaType(type)), m_parent(s), m_index(i.get()) {}

public:
    LuaStackReferenceBase(std::shared_ptr<lua_State> s, LuaStackIndex i, LuaType type) 
        : m_type(type), m_parent(s), m_index(i.get()) {}
    virtual ~LuaStackReferenceBase() = default;

    bool isValid() {
        lua_State *state = m_parent.get();
        int current_top = lua_gettop(state);
        bool is_not_empty_stack = current_top != 0;
        bool is_index_within_bounds = m_index.isFromBottom() && m_index <= current_top;
        bool do_type_match = lua_type(state, m_index) == m_type;
        return is_not_empty_stack && is_index_within_bounds && do_type_match;
    }

    operator bool() {
        return isValid();
    }

};

struct LuaNumberRef : public LuaStackReferenceBase {
    LuaNumberRef(std::shared_ptr<lua_State> s, LuaStackIndex i) : LuaStackReferenceBase(s, i, LuaType::NUMBER_TYPE) {}
    virtual ~LuaNumberRef() = default;

    double getValue() {
        if ( isValid() ) {
            return lua_tonumber(m_parent.get(), m_index.get());
        }
        return 0;
    }

    double operator *() {
        return getValue();
    }
};

struct LuaStringRef : public LuaStackReferenceBase {
    LuaStringRef(std::shared_ptr<lua_State> s, LuaStackIndex i) : LuaStackReferenceBase(s, i, LuaType::STRING_TYPE) {}
    virtual ~LuaStringRef() = default;

    std::string getValue() {
        if ( isValid() ) {
            return lua_tostring(m_parent.get(), m_index.get());
        }
        return "";
    }

    std::string operator *() {
        return getValue();
    }
};

struct LuaBooleanRef : public LuaStackReferenceBase {
    LuaBooleanRef(std::shared_ptr<lua_State> s, LuaStackIndex i) : LuaStackReferenceBase(s, i, LuaType::BOOLEAN_TYPE) {}
    virtual ~LuaBooleanRef() = default;

    bool getValue() {
        if ( isValid() ) {
            return lua_toboolean(m_parent.get(), m_index.get());
        }
        return false;
    }

    bool operator *() {
        return getValue();
    }
};

class LuaFunctionRef : public LuaStackReferenceBase {
    int m_input;
    int m_output;

public:
    LuaFunctionRef(std::shared_ptr<lua_State> s, LuaStackIndex i, const int input, const int output) 
        : LuaStackReferenceBase(s, i, LuaType::FUNCTION_TYPE), m_input(input), m_output(output) {}
    virtual ~LuaFunctionRef() = default;

    void safeCall(int handlerIndex = 0) {
        if ( isValid() ) {
            int top = lua_gettop(m_parent.get());

            if ( (top - m_index) < m_input ) {
                throw LuaException("Not enough arguments parsed to function");
            }

            if (lua_pcall(m_parent.get(), m_input, m_output, handlerIndex) != LUA_OK) {
                throw LuaException(lua_tostring(m_parent.get(), STACK_TOP.get()));
            }
        }
    }

    LuaFunctionRef &operator *() {
        return *this;
    }

    void operator()(int handlerIndex = 0) {
        safeCall(handlerIndex);
    }
};

class LuaState;

struct LuaTableRef : public LuaStackReferenceBase {
    LuaTableRef(std::shared_ptr<lua_State> s, LuaStackIndex i) : LuaStackReferenceBase(s, i, LuaType::TABLE_TYPE) {}
    virtual ~LuaTableRef() = default;

    LuaStringRef getString(const std::string &name) {
        auto state = m_parent;
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), m_index.get());
        return LuaStringRef(state, lua_gettop(state.get()));
    }

    LuaBooleanRef getBoolean(const std::string &name) {
        auto state = m_parent;
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), m_index.get());
        return LuaBooleanRef(state, lua_gettop(state.get()));
    }

    LuaNumberRef getNumber(const std::string &name) {
        auto state = m_parent;
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), m_index.get());
        return LuaNumberRef(state, lua_gettop(state.get()));
    }

    LuaFunctionRef getFunction(const std::string &name, const int input, const int output) {
        auto state = m_parent;
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), m_index.get());
        return LuaFunctionRef(state, lua_gettop(state.get()), input, output);
    }

    void set(std::string const &name, std::string value) {
        auto state = m_parent;
        lua_pushstring(state.get(), name.c_str());
        lua_pushstring(state.get(), value.c_str());
        lua_settable(state.get(), m_index);
    }

    void set(std::string const &name, double value) {
        auto state = m_parent;
        lua_pushstring(state.get(), name.c_str());
        lua_pushnumber(state.get(), value);
        lua_settable(state.get(), m_index);
    }

    void set(std::string const &name, bool value) {
        auto state = m_parent;
        lua_pushstring(state.get(), name.c_str());
        lua_pushboolean(state.get(), value);
        lua_settable(state.get(), m_index);
    }

    void set(std::string const &name,  lua_CFunction f) {
        auto state = m_parent;
        lua_pushstring(state.get(), name.c_str());
        lua_pushcfunction(state.get(), f);
        lua_settable(state.get(), m_index);
    }
};

#endif