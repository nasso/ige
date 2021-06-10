#ifndef A67D2A52_E2E7_4E85_AF56_5FC716E978D2
#define A67D2A52_E2E7_4E85_AF56_5FC716E978D2

#include "Collider.hpp"

namespace ige::plugin::physics {
class GhostObject {
public:
    GhostObject(Collider collider);

    const Collider& collider() const;

private:
    Collider m_collider;
};
}

#endif /* A67D2A52_E2E7_4E85_AF56_5FC716E978D2 */
