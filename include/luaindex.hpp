
#ifndef _HEADER_FILE_luaindex_20191209191657_
#define _HEADER_FILE_luaindex_20191209191657_

// generic lua index
class LuaStackIndex {
    int index;

public:
    LuaStackIndex(int i) : index(i) {}
    virtual ~LuaStackIndex() = default;

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

static LuaStackIndex STACK_TOP = LuaStackIndex(-1);

#endif