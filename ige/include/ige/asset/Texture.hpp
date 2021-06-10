#ifndef CE8932E3_3765_4F70_8A57_81C1AAF1C4C6
#define CE8932E3_3765_4F70_8A57_81C1AAF1C4C6

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

namespace ige::asset {

class Texture {
public:
    using Handle = std::shared_ptr<Texture>;

    enum class Format {
        R = 6403,
        RG = 33319,
        RGB = 6407,
        RGBA = 6408,
    };

    enum class MagFilter {
        LINEAR = 9729,
        NEAREST = 9728,
    };

    enum class MinFilter {
        NEAREST = 9728,
        LINEAR = 9729,
        NEAREST_MIPMAP_NEAREST = 9984,
        LINEAR_MIPMAP_NEAREST = 9985,
        NEAREST_MIPMAP_LINEAR = 9986,
        LINEAR_MIPMAP_LINEAR = 9987,
    };

    enum class WrappingMode {
        CLAMP_TO_EDGE = 33071,
        MIRRORED_REPEAT = 33648,
        REPEAT = 10497,
    };

private:
    std::uint64_t m_version = 0;
    Format m_format = Format::R;
    std::size_t m_width = 0;
    std::size_t m_height = 0;
    std::vector<std::byte> m_pixels;

    MagFilter m_mag_filter = MagFilter::NEAREST;
    MinFilter m_min_filter = MinFilter::NEAREST_MIPMAP_NEAREST;
    WrappingMode m_wrap_s = WrappingMode::CLAMP_TO_EDGE;
    WrappingMode m_wrap_t = WrappingMode::CLAMP_TO_EDGE;

public:
    Texture(Format format, std::size_t width, std::size_t height);
    Texture(std::span<const std::byte> image_data);
    Texture(const std::filesystem::path&);

    std::size_t width() const;
    std::size_t height() const;
    std::uint8_t channels() const;
    std::span<const std::byte> data() const;
    std::uint64_t version() const;
    Format format() const;
    MagFilter mag_filter() const;
    MinFilter min_filter() const;
    WrappingMode wrap_s() const;
    WrappingMode wrap_t() const;

    void set_mag_filter(MagFilter);
    void set_min_filter(MinFilter);
    void set_wrap_s(WrappingMode);
    void set_wrap_t(WrappingMode);

    void set_data(
        Format format, std::size_t width, std::size_t height,
        std::vector<std::byte> pixels);
    void load(std::span<const std::byte>);
    void load(const std::filesystem::path&);
};

}

#endif /* CE8932E3_3765_4F70_8A57_81C1AAF1C4C6 */
