#include "UiRenderer.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "TextureCache.hpp"
#include "VertexArray.hpp"
#include "WeakPtrMap.hpp"
#include "blobs/shaders/gl/ui-img-fs.glsl.h"
#include "blobs/shaders/gl/ui-img-vs.glsl.h"
#include "blobs/shaders/gl/ui-rect-fs.glsl.h"
#include "blobs/shaders/gl/ui-rect-vs.glsl.h"
#include "glad/gl.h"
#include "ige/asset/Texture.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/RenderPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/WindowPlugin.hpp"
#include <algorithm>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <tuple>
#include <variant>
#include <vector>

using glm::vec2;
using glm::vec4;
using ige::asset::Texture;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::render::ImageRenderer;
using ige::plugin::render::RectRenderer;
using ige::plugin::render::Visibility;
using ige::plugin::transform::RectTransform;
using ige::plugin::window::WindowInfo;

template <typename T>
using ConstRef = std::reference_wrapper<const T>;

struct UiRenderCache {
    gl::VertexArray quad_vao;
    gl::Program rect_program;
    gl::Program image_program;
    WeakPtrMap<Texture, TextureCache> textures;

    // called on the first frame
    UiRenderCache()
    {
        vec2 quad[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 0.0f, 1.0f },
            { 1.0f, 1.0f },
        };

        quad_vao.attrib(0, quad);

        {
            gl::Shader vs {
                gl::Shader::ShaderType::VERTEX,
                BLOBS_SHADERS_GL_UI_RECT_VS_GLSL,
            };

            gl::Shader fs {
                gl::Shader::ShaderType::FRAGMENT,
                BLOBS_SHADERS_GL_UI_RECT_FS_GLSL,
            };

            rect_program.link(vs, fs);
        }

        {
            gl::Shader vs {
                gl::Shader::ShaderType::VERTEX,
                BLOBS_SHADERS_GL_UI_IMG_VS_GLSL,
            };

            gl::Shader fs {
                gl::Shader::ShaderType::FRAGMENT,
                BLOBS_SHADERS_GL_UI_IMG_FS_GLSL,
            };

            image_program.link(vs, fs);
        }
    }

    gl::Texture& operator[](Texture::Handle tex)
    {
        auto it = textures.find(tex);

        if (it != textures.end()) {
            return it->second.gl_texture;
        } else {
            textures.clean();
            return textures.emplace(tex, *tex).first->second.gl_texture;
        }
    }
};

namespace systems {

static void render_ui(World& wld)
{
    vec2 winsize(0.0f);

    if (auto wininfo = wld.get<WindowInfo>()) {
        winsize.x = static_cast<float>(wininfo->width);
        winsize.y = static_cast<float>(wininfo->height);
    } else {
        std::cerr << "[WARN] No window info available!" << std::endl;
        return;
    }

    auto& cache = wld.get_or_emplace<UiRenderCache>();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TODO: make queries able to return std::variant directly
    auto rectangles = wld.query<RectRenderer, RectTransform>();
    auto images = wld.query<ImageRenderer, RectTransform>();

    using UiElem = std::tuple<
        EntityId, std::variant<ConstRef<RectRenderer>, ConstRef<ImageRenderer>>,
        ConstRef<RectTransform>>;

    std::vector<UiElem> elements;
    elements.reserve(rectangles.size() + images.size());

    for (const auto& [ent, rect, xform] : rectangles) {
        elements.emplace_back(ent, std::cref(rect), std::cref(xform));
    }

    for (const auto& [ent, img, xform] : images) {
        elements.emplace_back(ent, std::cref(img), std::cref(xform));
    }

    std::sort(
        elements.begin(), elements.end(), [](const UiElem& a, const UiElem& b) {
            auto [a_ent, a_renderer, a_xform] = a;
            auto [b_ent, b_renderer, b_xform] = b;

            return a_xform.get().abs_depth() > b_xform.get().abs_depth();
        });

    cache.quad_vao.bind();

    for (auto [ent, rect_or_img, xform_ref] : elements) {
        auto vis = wld.get_component<Visibility>(ent);

        if (vis && (!vis->global_visible() || vis->global_opacity() == 0.0f)) {
            continue;
        }

        const auto& xform = xform_ref.get();

        if (std::holds_alternative<ConstRef<RectRenderer>>(rect_or_img)) {
            const auto& rect
                = std::get<ConstRef<RectRenderer>>(rect_or_img).get();

            vec4 fill = rect.fill;

            if (vis) {
                fill.a *= vis->global_opacity();
            }

            cache.rect_program.use();
            cache.rect_program.uniform("u_FillColor", fill);
            cache.rect_program.uniform(
                "u_Bounds",
                vec4 {
                    xform.abs_bounds_min() / winsize * 2.0f - 1.0f,
                    xform.abs_bounds_max() / winsize * 2.0f - 1.0f,
                });
        } else if (std::holds_alternative<ConstRef<ImageRenderer>>(
                       rect_or_img)) {
            const auto& img
                = std::get<ConstRef<ImageRenderer>>(rect_or_img).get();

            if (img.texture == nullptr) {
                continue;
            }

            vec2 img_size = xform.abs_bounds_max() - xform.abs_bounds_min();
            vec2 tex_size {
                static_cast<float>(img.texture->width()),
                static_cast<float>(img.texture->height()),
            };

            if (img_size.x <= 0.0f || img_size.y <= 0.0f) {
                continue;
            }

            vec4 tint = img.tint;

            if (vis) {
                tint.a *= vis->global_opacity();
            }

            cache.image_program.use();

            glActiveTexture(GL_TEXTURE0);
            gl::Texture::bind(
                gl::Texture::Target::TEXTURE_2D, cache[img.texture]);
            cache.image_program.uniform("u_Texture", 0);
            cache.image_program.uniform("u_Tint", tint);
            cache.image_program.uniform(
                "u_Bounds",
                vec4 {
                    xform.abs_bounds_min() / winsize * 2.0f - 1.0f,
                    xform.abs_bounds_max() / winsize * 2.0f - 1.0f,
                });

            vec2 repeat_count(1.0f);
            vec4 tex_borders(0.0f, 0.0f, 1.0f, 1.0f);
            vec4 borders_pos(0.0f, 0.0f, 1.0f, 1.0f);

            if (img.mode == ImageRenderer::Mode::TILED) {
                // size of the center rect on the texture
                vec2 tex_center_part_size = vec2 {
                    tex_size.x - img.borders.x - img.borders.z,
                    tex_size.y - img.borders.y - img.borders.w,
                };

                // size of the center rect on screen
                vec2 scr_center_part_size = vec2 {
                    img_size.x - img.borders.x - img.borders.z,
                    img_size.y - img.borders.y - img.borders.w,
                };

                repeat_count = scr_center_part_size / tex_center_part_size;
            }

            if (img.mode != ImageRenderer::Mode::STRETCHED) {
                borders_pos = img.borders / vec4(img_size, img_size);
                borders_pos.z = 1.0f - borders_pos.z;
                borders_pos.w = 1.0f - borders_pos.w;

                tex_borders = img.borders / vec4(tex_size, tex_size);
                tex_borders.z = 1.0f - tex_borders.z;
                tex_borders.w = 1.0f - tex_borders.w;
            }

            cache.image_program.uniform("u_RepeatCount", repeat_count);
            cache.image_program.uniform("u_TextureBorders", tex_borders);
            cache.image_program.uniform("u_Borders", borders_pos);
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

static void clear_cache(World& wld)
{
    wld.remove<UiRenderCache>();
}

}

void UiRenderer::plug(App::Builder& builder) const
{
    builder.add_system(System::from(systems::render_ui));
    builder.add_cleanup_system(System::from(systems::clear_cache));
}
