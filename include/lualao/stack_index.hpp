
#pragma once

namespace lualao {
    // generic lua index
    class stack_index {
        int index;

      public:
        stack_index(int i)
            : index(i) {}
        virtual ~stack_index() = default;

        bool is_from_top() {
            return index < 0;
        }

        bool is_from_bottom() {
            return index > 0;
        }

        operator int() {
            return index;
        }

        int get() {
            return index;
        }
    };

    static stack_index STACK_TOP = stack_index(-1);
};
