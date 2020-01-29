#include <iostream>
#include <string>
#include "lualao/lualao.hpp"

struct Player {
    std::string title;
    std::string name;
    int level;
};

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "Error: Expected first argument to be path to script"
                  << std::endl;
        exit(1);
    }

    Player player;

    char *filename = argv[1];

    lualao::state L;

    L.open_libs();

    L.load_file(filename);

    {
        lualao::stack_context ctx(L);
        if (auto funref = L.get_function("AddStuff", 2, 1)) {

            L.push(32);
            L.push(11);

            funref();

            lualao::stack_debug_print(L);

            std::cout << "Is function ref still valid " << funref.isValid()
                      << std::endl;

            std::cout << "Got result from lua: " << (*L.get_number())
                      << std::endl;
        }

        if (auto funref = L.get_function("JustPrint")) {
            funref();
        }
    }

    std::cout << "1 size " << L.size() << std::endl;
    lualao::stack_debug_print(L);

    {
        lualao::stack_context ctx(L);

        if (auto tableRef = L.get_table("Player")) {

            if (auto nameRef = tableRef.get_string("Name")) {
                player.name = (*nameRef);
            }

            if (auto titleRef = tableRef.get_string("Title")) {
                player.title = (*titleRef);
            }

            if (auto levelRef = tableRef.get_number("Level")) {
                player.level = (*levelRef);
            }

            lualao::stack_debug_print(L);

            if (auto funcRef = tableRef.get_function("F", 1, 1)) {
                L.push(12);
                funcRef();
                std::cout << "Function F: " << L.get_number() << std::endl;
            }

            if (auto boolRef = tableRef.get_boolean("IsHero")) {
                std::cout << "Is hero? " << (*boolRef) << std::endl;
            }

            std::cout << player.name << std::endl;
            std::cout << player.title << std::endl;
            std::cout << player.level << std::endl;

            std::cout << "2 size " << L.size() << std::endl;
        }
    }

    lualao::stack_debug_print(L);

    return 0;
}