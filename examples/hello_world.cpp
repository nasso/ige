/*
** EPITECH PROJECT, 2021
** un
** File description:
** hello_world
*/

#include "un/App.hpp"
#include <iostream>

static void hello_world_system()
{
    std::cout << "hello world!" << std::flush;
}

int main()
{
    un::App app;

    app.add_system(hello_world_system);
    app.run();
}
