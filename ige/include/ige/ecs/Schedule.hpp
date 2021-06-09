#ifndef C36AD5E7_BDB7_4987_A83E_C6A2BB423A01
#define C36AD5E7_BDB7_4987_A83E_C6A2BB423A01

#include "System.hpp"
#include "World.hpp"
#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace ige::ecs {

class Schedule {
public:
    class Builder {
    public:
        Builder& add_system(std::unique_ptr<System> system) &;
        Builder add_system(std::unique_ptr<System> system) &&;

        Schedule build();

    private:
        std::vector<std::unique_ptr<System>> m_systems;
    };

    Schedule() = default;

    void run_forward(World&);
    void run_reverse(World&);

private:
    std::vector<std::unique_ptr<System>> m_systems;

    Schedule(std::vector<std::unique_ptr<System>>);
};

}

#endif /* C36AD5E7_BDB7_4987_A83E_C6A2BB423A01 */
