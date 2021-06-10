#ifndef A3879050_3E91_426B_BD37_E5E44D0860BD
#define A3879050_3E91_426B_BD37_E5E44D0860BD

#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/Resources.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/input/Mouse.hpp"
#include <cstdint>
#include <functional>
#include <glm/vec2.hpp>
#include <utility>
#include <vector>

namespace ige::plugin::ui {

namespace event {

    struct MouseEvent {
        glm::vec2 pos;
        glm::vec2 absolute_pos;
    };

    struct MouseDown : public MouseEvent {
        input::MouseButton button;
    };

    struct MouseUp : public MouseEvent {
        input::MouseButton button;
    };

    struct MouseClick : public MouseEvent {
        input::MouseButton button;
    };

    struct MouseScroll : public MouseEvent {
        glm::vec2 delta;
    };

    struct MouseEnter : public MouseEvent {
    };

    struct MouseLeave : public MouseEvent {
    };

    struct MouseMove : public MouseEvent {
    };

}

class EventTarget {
public:
    template <typename T>
    using Callback = std::function<void(const ecs::EntityId&, const T&)>;

    template <typename T>
    EventTarget& on(Callback<T>&& callback) &
    {
        add_callback<T>(std::forward<Callback<T>>(callback));
        return *this;
    }

    template <typename T>
    EventTarget on(Callback<T>&& callback) &&
    {
        return std::move(on<T>(std::forward<Callback<T>>(callback)));
    }

    template <typename T>
    void trigger(const ecs::EntityId& entity, const T& event)
    {
        trigger_callbacks<T>(entity, event);
    }

private:
    template <typename T>
    using CallbackVector = std::vector<Callback<T>>;

    template <typename T>
    void add_callback(Callback<T> callback)
    {
        auto& cb_vector = m_callbacks.get_or_emplace<CallbackVector<T>>();

        cb_vector.emplace_back(std::forward<Callback<T>>(callback));
    }

    template <typename T>
    void trigger_callbacks(const ecs::EntityId& entity, const T& event)
    {
        if (auto callbacks = m_callbacks.get<CallbackVector<T>>()) {
            for (auto& callback : *callbacks) {
                callback(entity, event);
            }
        }
    }

    ecs::Resources m_callbacks;
};

class UiPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* A3879050_3E91_426B_BD37_E5E44D0860BD */
