// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

#pragma once

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "luatic/type_references/stack_reference_base.hpp"
#include "luatic/type_references/function_reference.hpp"
#include "luatic/type_references/number_reference.hpp"
#include "luatic/type_references/table_reference.hpp"
#include "luatic/type_references/string_reference.hpp"
#include "luatic/type_references/boolean_reference.hpp"

#include "luatic/stack_context.hpp"
#include "luatic/state.hpp"

#include "luatic/lua_exception.hpp"
#include "luatic/stack_index.hpp"
#include "luatic/type.hpp"