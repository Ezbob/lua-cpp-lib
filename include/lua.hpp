// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <string>
#include <iostream>
#include <memory>
#include <exception>

const int STACK_TOP = -1;

class LuaType {
public:
    enum Types {
        NONE_TYPE = LUA_TNONE,
        NIL_TYPE = LUA_TNIL,
        NUMBER_TYPE = LUA_TNUMBER,
        BOOLEAN_TYPE = LUA_TBOOLEAN,
        STRING_TYPE = LUA_TSTRING,
        TABLE_TYPE = LUA_TTABLE,
        FUNCTION_TYPE = LUA_TFUNCTION,
        USERDATA_TYPE = LUA_TUSERDATA,
        THREAD_TYPE = LUA_TTHREAD,
        LIGHTUSERDATA_TYPE = LUA_TLIGHTUSERDATA
    };

    LuaType(int val) {
        switch (val) {
            case LUA_TNONE: value = NONE_TYPE; break;
            case LUA_TNIL: value = NIL_TYPE; break;
            case LUA_TNUMBER: value = NUMBER_TYPE; break;
            case LUA_TBOOLEAN: value = BOOLEAN_TYPE; break;
            case LUA_TSTRING: value = STRING_TYPE; break;
            case LUA_TTABLE: value = TABLE_TYPE; break;
            case LUA_TFUNCTION: value = FUNCTION_TYPE; break;
            case LUA_TUSERDATA: value = USERDATA_TYPE; break;
            case LUA_TTHREAD: value = THREAD_TYPE; break;
            case LUA_TLIGHTUSERDATA: value = LIGHTUSERDATA_TYPE; break;
            default: value = NONE_TYPE; break;
        }
    };
    virtual ~LuaType() = default;

    operator int() const {
        return value;
    }

private:
    enum Types value;
};

class LuaException : public std::runtime_error {
public:
    LuaException(const std::string &message) : std::runtime_error(message) {}
    LuaException(const char *message) : std::runtime_error(message) {}

    virtual ~LuaException() = default;
};

class LuaState;

class LuaStackReferenceBase {
    LuaType m_type;
    std::weak_ptr<lua_State> m_parent;
    int m_index;
public:
    LuaStackReferenceBase(std::shared_ptr<lua_State> s, LuaType type) : m_type(type), m_parent(s), m_index(STACK_TOP) {}
    LuaStackReferenceBase(std::shared_ptr<lua_State> s, int i, LuaType type) : m_type(type), m_parent(s), m_index(i) {}
    virtual ~LuaStackReferenceBase() = default;

    int getIndex() {
        return m_index;
    }

    bool isValid() {
        auto state = getParent();
        auto is_index_within_bounds = (m_index < 0 || (0 < m_index && m_index <= lua_gettop(state.get())));
        auto is_right_type = lua_type(state.get(), m_index) == m_type;
        return is_index_within_bounds && is_right_type;
    }

    operator bool() {
        return isValid();
    }

protected:
    std::shared_ptr<lua_State> getParent() {
        if (auto shrd_ptr = m_parent.lock()) {
            return shrd_ptr;
        }
        throw LuaException("LuaState is out of scope");
    }
};

struct LuaNumberRef : public LuaStackReferenceBase {
    LuaNumberRef(std::shared_ptr<lua_State> s, int i, LuaType type) : LuaStackReferenceBase(s, i, type) {}
    virtual ~LuaNumberRef() = default;

    double getValue() {
        if ( isValid() ) {
            auto shrd_ptr = getParent();
            return lua_tonumber(shrd_ptr.get(), getIndex());
        }
        return 0;
    }

    double operator *() {
        return getValue();
    }
};

struct LuaStringRef : public LuaStackReferenceBase {
    LuaStringRef(std::shared_ptr<lua_State> s, int i, LuaType type) : LuaStackReferenceBase(s, i, type) {}
    virtual ~LuaStringRef() = default;

    std::string getValue() {
        if ( isValid() ) {
            auto shrd_ptr = getParent();
            return lua_tostring(shrd_ptr.get(), getIndex());
        }
        return "";
    }

    std::string operator *() {
        return getValue();
    }
};

struct LuaBooleanRef : public LuaStackReferenceBase {
    LuaBooleanRef(std::shared_ptr<lua_State> s, int i, LuaType type) : LuaStackReferenceBase(s, i, type) {}
    virtual ~LuaBooleanRef() = default;

    bool getValue() {
        if ( isValid() ) {
            auto shrd_ptr = getParent();
            return lua_toboolean(shrd_ptr.get(), getIndex());
        }
        return false;
    }

    bool operator *() {
        return getValue();
    }
};

struct LuaFunctionRef : public LuaStackReferenceBase {
    LuaFunctionRef(std::shared_ptr<lua_State> s, int i, LuaType type) : LuaStackReferenceBase(s, i, type) {}
    virtual ~LuaFunctionRef() = default;

    void safeCall(unsigned int input, unsigned int output, int handlerIndex = 0) {
        if ( isValid() ) {
            auto shrd_ptr = getParent();
            if (lua_pcall(shrd_ptr.get(), (int) input, (int) output, handlerIndex) != LUA_OK) {
                throw LuaException(lua_tostring(shrd_ptr.get(), STACK_TOP));
            }
        }
    }

    LuaFunctionRef &operator *() {
        return *this;
    }

    void operator()(unsigned int input, unsigned int output, int handlerIndex = 0) {
        safeCall(input, output, handlerIndex);
    }
};

struct LuaTableRef : public LuaStackReferenceBase {
    LuaTableRef(std::shared_ptr<lua_State> s, int i, LuaType type) : LuaStackReferenceBase(s, i, type) {}
    virtual ~LuaTableRef() = default;

    LuaStringRef getString(const std::string &name) {
        auto state = getParent();
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), getIndex());
        return LuaStringRef(state, lua_gettop(state.get()), LuaType(LuaType::STRING_TYPE));
    }

    LuaBooleanRef getBoolean(const std::string &name) {
        auto state = getParent();
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), getIndex());
        return LuaBooleanRef(state, lua_gettop(state.get()), LuaType(LuaType::BOOLEAN_TYPE));
    }

    LuaNumberRef getNumber(const std::string &name) {
        auto state = getParent();
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), getIndex());
        return LuaNumberRef(state, lua_gettop(state.get()), LuaType(LuaType::NUMBER_TYPE));
    }

    LuaFunctionRef getFunction(const std::string &name) {
        auto state = getParent();
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), getIndex());
        return LuaFunctionRef(state, lua_gettop(state.get()), LuaType(LuaType::FUNCTION_TYPE));
    }

};

/*
 * shared_ptr lua state
 */
class LuaState {

public:
    LuaState() {
        m_state = std::shared_ptr<lua_State>(luaL_newstate(), lua_close);
    }

    virtual ~LuaState() = default;

    lua_State *ptr() {
        return m_state.get();
    }

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
            throw LuaException(lua_tostring(m_state.get(), -1));
        }
    }

    void loadFile(const std::string &path) {
        loadFile(path.c_str());
    }

    LuaType requestGlobal(const std::string &name) {
        return requestGlobal(name.c_str());
    }

    LuaType requestGlobal(const char *name) {
        return static_cast<LuaType>(lua_getglobal(m_state.get(), name));
    }

    LuaStringRef requestGlobalString(const char *name) {
        lua_getglobal(m_state.get(), name);
        return LuaStringRef(m_state, size(), LuaType(LuaType::STRING_TYPE));
    }

    LuaTableRef requestGlobalTable(const char *name) {
        lua_getglobal(m_state.get(), name);
        return LuaTableRef(m_state, size(), LuaType(LuaType::TABLE_TYPE));
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

    bool hasNumber(int index = STACK_TOP) {
        return lua_isnumber(m_state.get(), index);
    }

    double getNumber(int index = STACK_TOP) {
        return (double) lua_tonumber(m_state.get(), index);
    }

    bool hasTable(int index = STACK_TOP) {
        return lua_istable(m_state.get(), index);
    }

    void requestFromTable(int table_index = STACK_TOP) {
        lua_gettable(m_state.get(), table_index);
    }

    LuaType requestFromTable(const std::string &name, int table_index = STACK_TOP) {
        push(name);
        return lua_gettable(m_state.get(), table_index);
    }

    bool hasString(int index = STACK_TOP) {
        return lua_isstring(m_state.get(), index);
    }

    std::string getString(int index = STACK_TOP) {
        return lua_tostring(m_state.get(), index);
    }

    bool hasBool(int index = STACK_TOP) {
        return lua_isboolean(m_state.get(), index);
    }

    bool getBool(int index = STACK_TOP) {
        return lua_toboolean(m_state.get(), index);
    }

    bool hasFunction(int index = STACK_TOP) {
        return lua_isfunction(m_state.get(), index);
    }

    void getFunction(const char *name) {
        auto returnType = requestGlobal(name);
        if ( returnType != LuaType::FUNCTION_TYPE ) {
            throw LuaException("'" + std::string(name) + "' is not a function");
        }
    }

    void getFunction(const std::string &name) {
        getFunction(name.c_str());
    }

    void safeCallFunction(int inputArgs, int outputArgs, int handlerIndex = 0) {
        if (lua_pcall(m_state.get(), inputArgs, outputArgs, handlerIndex) != LUA_OK) {
            throw LuaException(lua_tostring(m_state.get(), -1));
        }
    }

    void callFunction(int inputArgs, int outputArgs) {
        lua_call(m_state.get(), inputArgs, outputArgs);
    }

    void pop(int number_of_elements = 1) {
        lua_pop(m_state.get(), number_of_elements);
    }

    int size() {
        return lua_gettop(m_state.get());
    }

private:
    std::shared_ptr<lua_State> m_state;
};