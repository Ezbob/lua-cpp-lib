
#ifndef _HEADER_FILE_luatype_20191209191735_
#define _HEADER_FILE_luatype_20191209191735_

#include "lua.hpp"
#include <string>

std::string type_to_string(int value) {
    switch(value) {
        case LUA_TNIL:
            return "nil";
        case LUA_TNONE:
            return "none";
        case LUA_TNUMBER:
            return "number";
        case LUA_TSTRING:
            return "string";
        case LUA_TBOOLEAN:
            return "boolean";
        case LUA_TTABLE:
            return "table";
        case LUA_TFUNCTION:
            return "function";
        case LUA_TUSERDATA:
            return "userdata";
        case LUA_TLIGHTUSERDATA:
            return "lightuserdata";
        case LUA_TTHREAD:
            return "thread";
        default:
            return "unknown";
    }
}

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

    LuaType(const int val) {
        Types value;
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
        m_value = value;
    };
    virtual ~LuaType() = default;

    operator int() const {
        return m_value;
    }

    bool operator ==(const LuaType &other) const {
        return m_value == other.m_value;
    }

    bool operator !=(const LuaType &other) const {
        return m_value != other.m_value;
    }

    bool operator ==(const int &other) const {
        return m_value == other;
    }

    bool operator !=(const int &other) const {
        return m_value != other;
    }

    std::string toString() const {
        return type_to_string(static_cast<int>(m_value));
    }

private:
    enum Types m_value;
};

#endif