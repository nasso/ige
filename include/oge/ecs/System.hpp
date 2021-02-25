/*
** EPITECH PROJECT, 2021
** oge
** File description:
** System
*/

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <functional>

namespace oge {
namespace ecs {

    class System {
    public:
        System(std::function<void()> run);

        void run();

    private:
        std::function<void()> m_run;
    };

}
}

#endif /* !SYSTEM_HPP_ */
