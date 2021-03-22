/*
** EPITECH PROJECT, 2021
** un
** File description:
** Schedule
*/

#ifndef C36AD5E7_BDB7_4987_A83E_C6A2BB423A01
#define C36AD5E7_BDB7_4987_A83E_C6A2BB423A01

#include "System.hpp"
#include "World.hpp"
#include <vector>

namespace un {
namespace ecs {

    class Schedule {
    private:
        std::vector<System> m_systems;

        Schedule(std::vector<System>);

    public:
        class Builder {
        private:
            std::vector<System> m_systems;

        public:
            Builder& add_system(System);
            Schedule build() const;
        };

        void operator()(World&);
    };

}
}

#endif /* C36AD5E7_BDB7_4987_A83E_C6A2BB423A01 */
