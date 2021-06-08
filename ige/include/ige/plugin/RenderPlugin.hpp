#ifndef D2D83F60_8D03_46D7_AFD5_A04004E85C93
#define D2D83F60_8D03_46D7_AFD5_A04004E85C93

#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/core/App.hpp"
#include <cstdint>
#include <glm/vec4.hpp>
#include <memory>
#include <optional>

namespace ige::plugin::render {

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
    asset::Mesh::Handle mesh;
    asset::Material::Handle material;
};

struct RectRenderer {
    glm::vec4 background_color;

    RectRenderer& set_background_rgba(glm::vec4 rgba) &;
    RectRenderer set_background_rgba(glm::vec4 rgba) &&;
    RectRenderer& set_background_rgb(glm::vec3 rgb) &;
    RectRenderer set_background_rgb(glm::vec3 rgb) &&;

    RectRenderer& set_background_rgba(std::uint32_t rgba) &;
    RectRenderer set_background_rgba(std::uint32_t rgba) &&;
    RectRenderer& set_background_rgb(std::uint32_t rgb) &;
    RectRenderer set_background_rgb(std::uint32_t rgb) &&;
};

class RenderPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* D2D83F60_8D03_46D7_AFD5_A04004E85C93 */
