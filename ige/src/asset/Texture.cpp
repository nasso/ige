#include "ige/asset/Texture.hpp"
#include <filesystem>
#include <memory>

using ige::asset::Texture;

std::shared_ptr<Texture> Texture::load_file(const std::filesystem::path&)
{
    return std::make_shared<Texture>();
}
