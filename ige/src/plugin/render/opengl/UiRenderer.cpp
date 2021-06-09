#include "UiRenderer.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "blobs/shaders/gl3/ui-fs.glsl.h"
#include "blobs/shaders/gl3/ui-vs.glsl.h"
#include "glad/gl.h"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/RenderPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/WindowPlugin.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <iostream>

using glm::vec2;
using glm::vec4;
using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::render::RectRenderer;
using ige::plugin::transform::RectTransform;
using ige::plugin::window::WindowInfo;

struct UiRenderCache {
    gl::VertexArray quad_vao;
    gl::Program program;

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
                BLOBS_SHADERS_GL3_UI_VS_GLSL,
            };

            gl::Shader fs {
                gl::Shader::ShaderType::FRAGMENT,
                BLOBS_SHADERS_GL3_UI_FS_GLSL,
            };

            program.link(vs, fs);
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
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto [ent, rect, xform] : wld.query<RectRenderer, RectTransform>()) {
        cache.program.use();
        cache.program.uniform("u_BaseColorFactor", rect.background_color);
        cache.program.uniform(
            "u_Bounds",
            vec4 {
                xform.abs_bounds_min() / winsize * 2.0f - 1.0f,
                xform.abs_bounds_max() / winsize * 2.0f - 1.0f,
            });

        cache.quad_vao.bind();
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
    builder.add_system(System::from(systems::clear_cache));
}
