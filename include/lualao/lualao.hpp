// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

#pragma once

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "lualao/type_references/stack_reference_base.hpp"
#include "lualao/type_references/function_reference.hpp"
#include "lualao/type_references/number_reference.hpp"
#include "lualao/type_references/table_reference.hpp"
#include "lualao/type_references/string_reference.hpp"
#include "lualao/type_references/boolean_reference.hpp"

#include "lualao/stack_context.hpp"
#include "lualao/state.hpp"

#include "lualao/lua_exception.hpp"
#include "lualao/stack_index.hpp"
#include "lualao/type.hpp"