#include "ige/game/Texture.hpp"
#include <filesystem>
#include <memory>

using ige::game::Texture;

std::shared_ptr<Texture> Texture::load_file(const std::filesystem::path&)
{
    return std::make_shared<Texture>();
}
