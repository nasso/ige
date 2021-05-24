#ifndef DF4046F7_8231_4537_88BD_6CE78AD4CEC4
#define DF4046F7_8231_4537_88BD_6CE78AD4CEC4

#include "ige/core/App.hpp"
#include "input/InputManager.hpp"
#include "input/InputRegistry.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"

namespace ige::plugin::input {

class InputPlugin : public ige::core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* DF4046F7_8231_4537_88BD_6CE78AD4CEC4 */
