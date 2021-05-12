#include "ige/core/FileSystemDataStore.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <vector>

using ige::core::FileSystemDataStore;

FileSystemDataStore::FileSystemDataStore(std::filesystem::path root)
    : m_root(root)
{
}

std::vector<std::byte> FileSystemDataStore::get(std::string_view key) const
{
    std::ifstream ifs {
        m_root / std::filesystem::path(key),
        std::ios::binary | std::ios::ate,
    };
    const auto size = ifs.tellg();

    std::vector<std::byte> buffer(size);
    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}
