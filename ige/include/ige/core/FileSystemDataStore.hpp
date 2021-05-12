#ifndef C3D711B6_7A19_4918_846A_8D2CB2855D8B
#define C3D711B6_7A19_4918_846A_8D2CB2855D8B

#include "ige/core/DataStore.hpp"
#include <cstddef>
#include <filesystem>
#include <vector>

namespace ige {
namespace core {

    class FileSystemDataStore : public DataStore {
    private:
        std::filesystem::path m_root;

    public:
        FileSystemDataStore(std::filesystem::path root);

        std::vector<std::byte> get(std::string_view key) const override;
    };

}
}

#endif /* C3D711B6_7A19_4918_846A_8D2CB2855D8B */
