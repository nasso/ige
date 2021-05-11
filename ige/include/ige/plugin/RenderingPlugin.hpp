#ifndef D2D83F60_8D03_46D7_AFD5_A04004E85C93
#define D2D83F60_8D03_46D7_AFD5_A04004E85C93

#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/core/App.hpp"
#include <memory>
#include <optional>

namespace ige {
namespace plugin {

    struct PerspectiveCamera {
        float fov;
        float near;
        float far;

        constexpr PerspectiveCamera(
            float fov, float near = 0.1f, float far = 1000.0f) noexcept
            : fov(fov)
            , near(near)
            , far(far)
        {
        }
    };

    struct MeshRenderer {
        std::shared_ptr<asset::Mesh> mesh;
        std::shared_ptr<asset::Material> material;
    };

    class RenderingPlugin : public core::App::Plugin {
    public:
        void plug(core::App::Builder&) const override;
    };

}
}

#endif /* D2D83F60_8D03_46D7_AFD5_A04004E85C93 */
