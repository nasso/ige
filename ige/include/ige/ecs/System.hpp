#ifndef FE472698_E495_4B4C_95C4_2F2A646A84F5
#define FE472698_E495_4B4C_95C4_2F2A646A84F5

#include "World.hpp"
#include <functional>
#include <memory>

namespace ige::ecs {

class System {
public:
    static std::unique_ptr<System> from(std::function<void(World&)>);

    virtual ~System() = default;

    virtual void run(World&) = 0;
};

}

#endif /* FE472698_E495_4B4C_95C4_2F2A646A84F5 */
