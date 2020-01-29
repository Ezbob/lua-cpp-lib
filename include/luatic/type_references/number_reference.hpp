
#pragma once

#include "stack_reference_base.hpp"
#include "luatic/type.hpp"

namespace luatic {

    class number_reference : public stack_reference_base {
    public:
        number_reference(std::shared_ptr<lua_State> s, stack_index i)
            : stack_reference_base(s, i, type::NUMBER_TYPE) {}
        virtual ~number_reference() = default;

        double getValue() {
            if (isValid()) {
                return lua_tonumber(m_parent.get(), m_index.get());
            }
            return 0;
        }

        double operator*() {
            return getValue();
        }
    };

}; // namespace luatic
