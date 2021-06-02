#ifndef AF61A46C_7207_4C78_BE75_D1ECC2F16751
#define AF61A46C_7207_4C78_BE75_D1ECC2F16751

#include "ige/ecs/Entity.hpp"
#include "ige/plugin/physics/Constraint.hpp"
#include <memory>
#include <utility>
#include <vector>

namespace ige::plugin::physics {

struct Collision {
    ecs::EntityId first;
    ecs::EntityId second;
};

class PhysicsWorld {
public:
    PhysicsWorld() = default;

    void add_collision(const ecs::EntityId&, const ecs::EntityId&);
    const std::vector<Collision>& collisions() const;
    bool collide(const ecs::EntityId&, const ecs::EntityId&) const;
    void clear_collisions();

    void add_constraint(const Constraint& constraint);
    const std::vector<Constraint>& new_constraints() const;
    void clear_new_constraints();

private:
    std::vector<Constraint> m_new_constraints;
    std::vector<Collision> m_collisions;
};

}

#endif /* AF61A46C_7207_4C78_BE75_D1ECC2F16751 */
