#ifndef EAB69569_3F45_402E_951B_71787149823E
#define EAB69569_3F45_402E_951B_71787149823E

#include "ige/game/Material.hpp"
#include "ige/game/Mesh.hpp"
#include <memory>

namespace ige {
namespace game {

    struct MeshRenderer {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
    };

}
}

#endif /* EAB69569_3F45_402E_951B_71787149823E */
