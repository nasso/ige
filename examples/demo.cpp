#include "ige.hpp"
#include <iostream>

using ige::core::App;
using ige::core::State;
using ige::ecs::Schedule;

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
