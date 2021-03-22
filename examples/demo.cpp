/*
** EPITECH PROJECT, 2021
** un
** File description:
** demo
*/

#include "un.hpp"
#include <iostream>

using un::core::App;
using un::core::State;
using un::ecs::Schedule;

class RootState : public State {
    int counter = 0;

    void on_update(App& app) override
    {
        counter++;

        std::cout << "Hello " << counter << std::endl;

        if (counter >= 10) {
            app.quit();
        }
    }
};

int main()
{
    App::Builder().run<RootState>();
}
