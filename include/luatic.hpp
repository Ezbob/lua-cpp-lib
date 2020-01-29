// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

#pragma once

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "type_references/stack_reference_base.hpp"
#include "type_references/function_reference.hpp"
#include "type_references/number_reference.hpp"
#include "type_references/table_reference.hpp"
#include "type_references/string_reference.hpp"
#include "type_references/boolean_reference.hpp"

#include "stack_context.hpp"
#include "state.hpp"

#include "lua_exception.hpp"
#include "stack_index.hpp"
#include "type.hpp"