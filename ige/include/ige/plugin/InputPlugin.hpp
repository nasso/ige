#ifndef DF4046F7_8231_4537_88BD_6CE78AD4CEC4
#define DF4046F7_8231_4537_88BD_6CE78AD4CEC4

#include "ige/core/App.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "input/InputManager.hpp"
#include "input/InputRegistry.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include <string>

namespace ige::plugin::input {

template <typename AxisId = std::string, typename ActionId = std::string>
class InputPlugin : public ige::core::App::Plugin {
public:
    void plug(ige::core::App::Builder& builder) const
    {
        builder.emplace<InputManager<AxisId, ActionId>>();
        builder.emplace<ige::core::EventChannel<InputEvent>>();
        builder.add_system(ige::ecs::System::from(update_input_manager));
    }

private:
    struct InputSubscription {
        ige::core::EventChannel<InputEvent>::Subscription sub;

        InputSubscription(ige::core::EventChannel<InputEvent>& chan)
            : sub(chan.subscribe())
        {
        }
    };

    static void update_input_manager(ige::ecs::World& wld)
    {
        auto& manager = wld.get_or_emplace<InputManager<AxisId, ActionId>>();
        auto& channel
            = wld.get_or_emplace<ige::core::EventChannel<InputEvent>>();
        auto& sub = wld.get_or_emplace<InputSubscription>(channel).sub;

        manager.reset();
        while (const auto& event = sub.next_event()) {
            manager.push_event(*event);
        }
    }
};

}

#endif /* DF4046F7_8231_4537_88BD_6CE78AD4CEC4 */
