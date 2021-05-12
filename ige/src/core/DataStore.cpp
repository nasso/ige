#include "ige/core/DataStore.hpp"
#include <string>
#include <string_view>

using ige::core::DataStore;

std::string DataStore::get_str(std::string_view key) const
{
    auto data = get(key);

    return { reinterpret_cast<char*>(data.data()), data.size() };
}

#ifdef IGE_ENGINE_FS_DATA_STORE_PATH
#define STRING_(s) #s
#define STRING(s) STRING_(s)
#define PATH_STRING STRING(IGE_ENGINE_FS_DATA_STORE_PATH)
#include "ige/core/FileSystemDataStore.hpp"

ige::core::FileSystemDataStore engine_store(PATH_STRING);

const DataStore& DataStore::get_engine_data_store()
{
    return engine_store;
}
#endif
