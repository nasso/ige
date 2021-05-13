#ifndef AE1E95C1_B3C0_478A_839F_FE6848566F31
#define AE1E95C1_B3C0_478A_839F_FE6848566F31

#include <cstddef>
#include <string_view>
#include <vector>

namespace ige {
namespace core {

    class DataStore {
    public:
        static const DataStore& get_engine_data_store();

        virtual ~DataStore() = default;

        virtual std::vector<std::byte> get(std::string_view key) const = 0;

        std::string get_str(std::string_view key) const;
    };

}
}

#endif /* AE1E95C1_B3C0_478A_839F_FE6848566F31 */
