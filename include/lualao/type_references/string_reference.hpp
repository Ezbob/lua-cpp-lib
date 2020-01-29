
#pragma once

#include <memory>
#include "stack_reference_base.hpp"
#include "lua.h"
#include "lualao/type.hpp"

namespace lualao {

    class string_reference: public stack_reference_base {
      public:
        string_reference(std::shared_ptr<lua_State> s, stack_index i)
            : stack_reference_base(s, i, type::STRING_TYPE) {}
        virtual ~string_reference() = default;

        std::string getValue() {
            if (isValid()) {
                return lua_tostring(m_parent.get(), m_index.get());
            }
            return "";
        }

        std::string operator*() {
            return getValue();
        }
    };

}; // namespace lualao
