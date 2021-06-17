#ifndef D2D83F60_8D03_46D7_AFD5_A04004E85C93
#define D2D83F60_8D03_46D7_AFD5_A04004E85C93

#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/asset/Texture.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include <cstdint>
#include <filesystem>
#include <glm/vec3.hpp>
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

enum class LightType {
    AMBIENT,
    POINT,
    DIRECTIONAL,
};

struct Light {
    LightType type = LightType::POINT;
    glm::vec3 color { 1.0f };
    float intensity = 1.0f;
    float range = 4.0f;

    static Light ambient(float intensity, glm::vec3 color = glm::vec3 { 1.0f });
    static Light point(
        float intensity, float range = 10.0f,
        glm::vec3 color = glm::vec3 { 1.0f });
    static Light
    directional(float intensity, glm::vec3 color = glm::vec3 { 1.0f });
};

class Visibility {
public:
    bool visible = true;
    float opacity = 1.0f;

    Visibility(bool visible = true);
    Visibility(float opacity);
    Visibility(bool visible, float opacity = 1.0f);

    bool global_visible() const;
    float global_opacity() const;

    void force_global_update(bool parent_visible, float parent_opacity);

private:
    bool m_global_visible = true;
    float m_global_opacity = 1.0f;
};

struct MeshRenderer {
    asset::Mesh::Handle mesh;
    asset::Material::Handle material;
    std::optional<ecs::EntityId> skeleton_pose;
};

struct RectRenderer {
    RectRenderer& set_fill_rgba(glm::vec4 rgba) &;
    RectRenderer set_fill_rgba(glm::vec4 rgba) &&;
    RectRenderer& set_fill_rgb(glm::vec3 rgb) &;
    RectRenderer set_fill_rgb(glm::vec3 rgb) &&;

    RectRenderer& set_fill_rgba(std::uint32_t rgba) &;
    RectRenderer set_fill_rgba(std::uint32_t rgba) &&;
    RectRenderer& set_fill_rgb(std::uint32_t rgb) &;
    RectRenderer set_fill_rgb(std::uint32_t rgb) &&;

    glm::vec4 fill;
};

struct ImageRenderer {
    enum class Mode {
        STRETCHED,
        SLICED,
        TILED,
    };

    ImageRenderer(asset::Texture::Handle texture, Mode mode = Mode::STRETCHED);

    ImageRenderer& set_tint_rgba(glm::vec4 rgba) &;
    ImageRenderer set_tint_rgba(glm::vec4 rgba) &&;
    ImageRenderer& set_tint_rgb(glm::vec3 rgb) &;
    ImageRenderer set_tint_rgb(glm::vec3 rgb) &&;

    ImageRenderer& set_tint_rgba(std::uint32_t rgba) &;
    ImageRenderer set_tint_rgba(std::uint32_t rgba) &&;
    ImageRenderer& set_tint_rgb(std::uint32_t rgb) &;
    ImageRenderer set_tint_rgb(std::uint32_t rgb) &&;

    ImageRenderer& set_mode(Mode mode) &;
    ImageRenderer set_mode(Mode mode) &&;

    ImageRenderer& set_borders(glm::vec4 left_top_right_bottom) &;
    ImageRenderer set_borders(glm::vec4 left_top_right_bottom) &&;

    asset::Texture::Handle texture;
    glm::vec4 borders { 1.0f / 3.0f };
    glm::vec4 tint { 1.0f };
    Mode mode = Mode::STRETCHED;
};

class RenderPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* D2D83F60_8D03_46D7_AFD5_A04004E85C93 */
