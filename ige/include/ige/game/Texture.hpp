#ifndef CE8932E3_3765_4F70_8A57_81C1AAF1C4C6
#define CE8932E3_3765_4F70_8A57_81C1AAF1C4C6

#include <filesystem>
#include <memory>

namespace ige {
namespace game {

    class Texture {
    public:
        static std::shared_ptr<Texture> load_file(const std::filesystem::path&);
    };

}
}

#endif /* CE8932E3_3765_4F70_8A57_81C1AAF1C4C6 */
