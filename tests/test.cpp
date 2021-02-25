/*
** EPITECH PROJECT, 2021
** oge
** File description:
** test
*/

#include "oge/App.hpp"
#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <iostream>

static void hello_world_system()
{
    std::cout << "hello world!" << std::flush;
}

Test(ecs, hello_world, .init = cr_redirect_stdout, .timeout = 1)
{
    oge::App app;

    app.add_system(hello_world_system);
    app.run();

    cr_assert_stdout_eq_str("hello world!");
}
