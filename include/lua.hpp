// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "luacontext.hpp"
#include "luastate.hpp"
#include "luaref.hpp"
#include "luaexception.hpp"
#include "luaindex.hpp"
#include "luatype.hpp"