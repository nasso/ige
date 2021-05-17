#ifndef A729B70C_0915_4141_B23A_529877E68174
#define A729B70C_0915_4141_B23A_529877E68174

#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/ecs/World.hpp"
#include <fx/gltf.h>
#include <istream>
#include <memory>
#include <string>
#include <vector>

namespace ige {
namespace asset {

    class Gltf {
    private:
        template <typename T>
        using Ref = std::shared_ptr<T>;

        std::vector<Ref<Material>> m_materials;
        std::vector<Ref<Mesh>> m_meshes;

    public:
        static Gltf from_text(const std::string& path);
        static Gltf from_text(std::istream&, const std::string& root_path);
        static Gltf from_bin(const std::string& path);
        static Gltf from_bin(std::istream&, const std::string& root_path);

        explicit Gltf(fx::gltf::Document);

        ecs::World::EntityRef create_entities(ecs::World&);
    };

}
}

#endif /* A729B70C_0915_4141_B23A_529877E68174 */
