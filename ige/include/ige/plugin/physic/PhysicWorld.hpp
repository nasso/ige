#ifndef AF61A46C_7207_4C78_BE75_D1ECC2F16751
#define AF61A46C_7207_4C78_BE75_D1ECC2F16751

#include "ige/ecs/Entity.hpp"
#include "ige/plugin/physic/Constraint.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace ige::plugin::physic {
struct Collision {
    ecs::EntityId first;
    ecs::EntityId second;
};

class PhysicWorld {
public:
    PhysicWorld() = default;

    void add_collision(ecs::EntityId entity1, ecs::EntityId entity2);
    const std::vector<Collision>& get_collisions() const;
    bool collide(ecs::EntityId entity1, ecs::EntityId entity2);
    void clear_collisions();

    void add_constraint(const Constraint& constraint);
    const std::vector<Constraint>& get_new_constraints() const;
    void clear_new_constraints();

private:
    std::vector<Constraint> m_new_constraints;
    std::vector<Collision> m_collisions;
};
}

#endif /* AF61A46C_7207_4C78_BE75_D1ECC2F16751 */
