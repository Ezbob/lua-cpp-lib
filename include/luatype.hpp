
#ifndef _HEADER_FILE_luatype_20191209191735_
#define _HEADER_FILE_luatype_20191209191735_

#include "lua.hpp"
#include <string>

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

    std::string toString() const {
        switch(value) {
            case NIL_TYPE:
                return "nil";
            case NONE_TYPE:
                return "none";
            case NUMBER_TYPE:
                return "number";
            case STRING_TYPE:
                return "string";
            case BOOLEAN_TYPE:
                return "boolean";
            case TABLE_TYPE:
                return "table";
            case FUNCTION_TYPE:
                return "function";
            case USERDATA_TYPE:
                return "userdata";
            case LIGHTUSERDATA_TYPE:
                return "lightuserdata";
            case THREAD_TYPE:
                return "thread";
            default:
                return "unknown";
        }
    }

private:
    enum Types value;
};

#endif