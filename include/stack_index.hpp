
#pragma once

namespace luatic {
    // generic lua index
    class stack_index {
        int index;

      public:
        stack_index(int i)
            : index(i) {}
        virtual ~stack_index() = default;

        bool isFromTop() {
            return index < 0;
        }

        bool isFromBottom() {
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
