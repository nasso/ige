#ifndef AF61A46C_7207_4C78_BE75_D1ECC2F16751
#define AF61A46C_7207_4C78_BE75_D1ECC2F16751

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include <memory>

namespace ige::plugin::physic {
class PhysicWorld {
public:
    PhysicWorld();

    btDiscreteDynamicsWorld* world();

private:
    btDefaultCollisionConfiguration m_collision_config;
    btCollisionDispatcher m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_broadphase;
    btSequentialImpulseConstraintSolver m_solver;
    btDiscreteDynamicsWorld m_world;
};
}

#endif /* AF61A46C_7207_4C78_BE75_D1ECC2F16751 */
