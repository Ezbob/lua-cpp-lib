
#pragma once

#include <memory>
#include "lua.h"
#include "stack_reference_base.hpp"
#include "stack_index.hpp"
#include "type.hpp"

namespace luatic {

    class boolean_reference: public stack_reference_base {
    public:
        boolean_reference(std::shared_ptr<lua_State> s, stack_index i)
            : stack_reference_base(s, i, type::BOOLEAN_TYPE) {}
        virtual ~boolean_reference() = default;

        bool getValue() {
            if (isValid()) {
                return lua_toboolean(m_parent.get(), m_index.get());
            }
            return false;
        }

        bool operator*() {
            return getValue();
        }
    };

}; // namespace luatic
