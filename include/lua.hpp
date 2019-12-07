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


// generic lua index
class LuaStackIndex {
    int index;

public:
    LuaStackIndex(int i) : index(i) {}
    virtual ~LuaStackIndex() = default;

    bool isFromTop() {
        return index <= 0;
    }

    bool isFromBottom() {
        return index >= 0;
    }

    operator int() {
        return index;
    }

    int get() {
        return index;
    }
};

static LuaStackIndex STACK_TOP = LuaStackIndex(-1);

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

    bool operator ==(const LuaType &other) const {
        return value == other.value;
    }

    bool operator !=(const LuaType &other) const {
        return value != other.value;
    }

    bool operator ==(const int &other) const {
        return value == other;
    }

    bool operator !=(const int &other) const {
        return value != other;
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

protected:
    LuaStackIndex m_index;

    LuaStackReferenceBase(std::shared_ptr<lua_State> s, LuaStackIndex i, int type) 
        : m_type(LuaType(type)), m_parent(s), m_index(i.get()) {}

    std::shared_ptr<lua_State> getParent() {
        if (auto shrd_ptr = m_parent.lock()) {
            return shrd_ptr;
        }
        throw LuaException("lua_State is out of scope");
    }

public:
    LuaStackReferenceBase(std::shared_ptr<lua_State> s, LuaStackIndex i, LuaType type) 
        : m_type(type), m_parent(s), m_index(i.get()) {}
    virtual ~LuaStackReferenceBase() = default;

    bool isValid() {
        try {
            lua_State *state = getParent().get();
            int current_top = lua_gettop(state);
            bool is_not_empty_stack = current_top != 0;
            bool is_index_within_bounds = m_index.isFromBottom() && m_index <= current_top;
            bool do_type_match = lua_type(state, m_index) == m_type;
            return is_not_empty_stack && is_index_within_bounds && do_type_match;
        } catch(const LuaException &e) {
            return false;
        }
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
            auto shrd_ptr = getParent();
            return lua_tonumber(shrd_ptr.get(), m_index.get());
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
            auto shrd_ptr = getParent();
            return lua_tostring(shrd_ptr.get(), m_index.get());
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
            auto shrd_ptr = getParent();
            return lua_toboolean(shrd_ptr.get(), m_index.get());
        }
        return false;
    }

    bool operator *() {
        return getValue();
    }
};

struct LuaFunctionRef : public LuaStackReferenceBase {
    LuaFunctionRef(std::shared_ptr<lua_State> s, LuaStackIndex i) : LuaStackReferenceBase(s, i, LuaType::FUNCTION_TYPE) {}
    virtual ~LuaFunctionRef() = default;

    void safeCall(unsigned int input, unsigned int output, int handlerIndex = 0) {
        if ( isValid() ) {
            auto shrd_ptr = getParent();
            if (lua_pcall(shrd_ptr.get(), (int) input, (int) output, handlerIndex) != LUA_OK) {
                throw LuaException(lua_tostring(shrd_ptr.get(), STACK_TOP.get()));
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
    LuaTableRef(std::shared_ptr<lua_State> s, LuaStackIndex i) : LuaStackReferenceBase(s, i, LuaType::TABLE_TYPE) {}
    virtual ~LuaTableRef() = default;

    LuaStringRef getString(const std::string &name) {
        auto state = getParent();
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), m_index.get());
        return LuaStringRef(state, lua_gettop(state.get()));
    }

    LuaBooleanRef getBoolean(const std::string &name) {
        auto state = getParent();
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), m_index.get());
        return LuaBooleanRef(state, lua_gettop(state.get()));
    }

    LuaNumberRef getNumber(const std::string &name) {
        auto state = getParent();
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), m_index.get());
        return LuaNumberRef(state, lua_gettop(state.get()));
    }

    LuaFunctionRef getFunction(const std::string &name) {
        auto state = getParent();
        lua_pushstring(state.get(), name.c_str());
        lua_gettable(state.get(), m_index.get());
        return LuaFunctionRef(state, lua_gettop(state.get()));
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

    LuaFunctionRef getFunction(const std::string &name) {
        lua_getglobal(m_state.get(), name.c_str());
        return LuaFunctionRef(m_state, top());
    }

    LuaTableRef getTable(const std::string &name) {
        lua_getglobal(m_state.get(), name.c_str());
        return LuaTableRef(m_state, top());
    }

    void pop(int number_of_elements = 1) {
        lua_pop(m_state.get(), number_of_elements);
    }

    int size() {
        return lua_gettop(m_state.get());
    }

    LuaStackIndex top() {
        return lua_gettop(m_state.get());
    }

private:
    std::shared_ptr<lua_State> m_state;
};