#ifndef D2D83F60_8D03_46D7_AFD5_A04004E85C93
#define D2D83F60_8D03_46D7_AFD5_A04004E85C93

#include "ige/core/App.hpp"

namespace ige {
namespace game {

    class RenderingPlugin : public core::App::Plugin {
    public:
        void plug(core::App::Builder&) const override;
    };

}
}

#endif /* D2D83F60_8D03_46D7_AFD5_A04004E85C93 */
