#include "ige/asset/Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
#include <cerrno>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <stdexcept>
#include <vector>

using ige::asset::Texture;

Texture::Texture(Format format, std::size_t width, std::size_t height)
    : m_format(format)
    , m_width(width)
    , m_height(height)
    , m_pixels(width * height)
{
}

Texture::Texture(std::span<const std::byte> image_data)
{
    load(image_data);
    m_version = 0;
}

Texture::Texture(const std::filesystem::path& path)
{
    load(path);
    m_version = 0;
}

std::size_t Texture::width() const
{
    return m_width;
}

std::size_t Texture::height() const
{
    return m_height;
}

std::uint8_t Texture::channels() const
{
    switch (m_format) {
    case Texture::Format::RGBA:
        return 4;
    case Texture::Format::RGB:
        return 3;
    case Texture::Format::RG:
        return 2;
    case Texture::Format::R:
        return 1;
    default:
        return 0;
    }
}

std::span<const std::byte> Texture::data() const
{
    return m_pixels;
}

std::uint64_t Texture::version() const
{
    return m_version;
}

Texture::Format Texture::format() const
{
    return m_format;
}

Texture::MagFilter Texture::mag_filter() const
{
    return m_mag_filter;
}

Texture::MinFilter Texture::min_filter() const
{
    return m_min_filter;
}

Texture::WrappingMode Texture::wrap_s() const
{
    return m_wrap_s;
}

Texture::WrappingMode Texture::wrap_t() const
{
    return m_wrap_t;
}

void Texture::set_mag_filter(MagFilter value)
{
    m_mag_filter = value;
}

void Texture::set_min_filter(MinFilter value)
{
    m_min_filter = value;
}

void Texture::set_wrap_s(WrappingMode value)
{
    m_wrap_s = value;
}

void Texture::set_wrap_t(WrappingMode value)
{
    m_wrap_t = value;
}

void Texture::set_data(
    Format format, std::size_t width, size_t height,
    std::vector<std::byte> pixels)
{
    m_format = format;

    if (pixels.size() < width * height * channels()) {
        throw std::runtime_error("Pixel buffer is too small.");
    }

    m_version++;
    m_width = width;
    m_height = height;
    m_pixels = std::move(pixels);
}

void Texture::load(std::span<const std::byte> buffer)
{
    int width;
    int height;
    int channels;

    unsigned char* data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char*>(buffer.data()),
        static_cast<int>(buffer.size()), &width, &height, &channels, 0);

    if (!data) {
        throw std::runtime_error(stbi_failure_reason());
    } else if (channels < 1 || channels > 4) {
        stbi_image_free(data);
        throw std::runtime_error(
            "Invalid channel count (" + std::to_string(channels) + ")");
    }

    const Texture::Format formats[] = {
        Format::R,
        Format::RG,
        Format::RGB,
        Format::RGBA,
    };

    try {
        std::byte* bytes = reinterpret_cast<std::byte*>(data);

        set_data(
            formats[channels - 1], width, height,
            { bytes, bytes + width * height * channels });
    } catch (...) {
        stbi_image_free(data);
        throw;
    }

    stbi_image_free(data);
}

static std::vector<std::byte> read_file(const std::filesystem::path& path)
{
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);

    if (!ifs) {
        throw std::runtime_error(path.string() + ": " + std::strerror(errno));
    }

    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    auto size = std::size_t(end - ifs.tellg());

    if (size == 0) {
        throw std::runtime_error(path.string() + ": file is empty");
    }

    std::vector<std::byte> buffer(size);

    if (!ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
        throw std::runtime_error(path.string() + ": " + std::strerror(errno));
    }

    return buffer;
}

void Texture::load(const std::filesystem::path& path)
{
    auto buffer = read_file(path);

    try {
        load(buffer);
    } catch (const std::exception& e) {
        throw std::runtime_error(path.string() + ": " + e.what());
    }
}
