#include <iostream>
#include <string>
#include "luatic/luatic.hpp"

struct Player {
    std::string title;
    std::string name;
    int level;
};

int main(int argc, char **argv) {

    if ( argc < 2 ) {
        std::cerr << "Error: Expected first argument to be path to script" << std::endl;
        exit(1);
    }

    Player player;

    char *filename = argv[1];

    luatic::state L;

    luaL_openlibs(L);

    L.loadFile(filename);

    {
        luatic::stack_context ctx(L);
        if ( auto funref = L.getFunction("AddStuff", 2, 1) ) {

            L.push(32);
            L.push(11);

            funref();

            luatic::stack_debug_print(L);

            std::cout << "Is function ref still valid " << funref.isValid() << std::endl;

            std::cout << "Got result from lua: " << (*L.getNumber()) << std::endl;
        }

        if ( auto funref = L.getFunction("JustPrint") ) {
            funref();
        }
    }


    std::cout << "1 size " << L.size() << std::endl;
    luatic::stack_debug_print(L);

    {
        luatic::stack_context ctx(L);

        if ( auto tableRef = L.getTable("Player") ) {

            if ( auto nameRef = tableRef.getString("Name") ) {
                player.name = (*nameRef);
            }

            if ( auto titleRef = tableRef.getString("Title") ) {
                player.title = (*titleRef);
            }

            if ( auto levelRef = tableRef.getNumber("Level") ) {
                player.level = (*levelRef);
            }

            luatic::stack_debug_print(L);

            if ( auto funcRef = tableRef.getFunction("F", 1, 1) ) {
                L.push(12);
                funcRef();
                std::cout << "Function F: " << L.getNumber() << std::endl;
            }

            if ( auto boolRef = tableRef.getBoolean("IsHero") ) {
                std::cout << "Is hero? " << (*boolRef) << std::endl;
            }

            std::cout << player.name << std::endl;
            std::cout << player.title << std::endl;
            std::cout << player.level << std::endl;

            std::cout << "2 size " << L.size() << std::endl;
        }
    }

    luatic::stack_debug_print(L);

    return 0;
}