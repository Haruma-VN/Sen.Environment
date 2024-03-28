#pragma once

#include "kernel/definition/assert.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/basic/buffer.hpp"
#include "kernel/definition/basic/string.hpp"
#include "kernel/definition/encryption/crc.hpp"

namespace Sen::Kernel::Definition
{
    struct APNGMakerSetting
    {
        std::vector<std::uint32_t> delay_frames_list{};
        std::uint32_t loop{};
        std::uint32_t width{};
        std::uint32_t height{};
        bool trim{};

        APNGMakerSetting(

            ) = default;

        ~APNGMakerSetting(

            ) = default;

        APNGMakerSetting(
            const std::vector<std::uint32_t> &delay_frames_list,
            std::uint32_t loop,
            std::uint32_t width,
            std::uint32_t height,
            bool trim) : delay_frames_list(delay_frames_list), loop(loop), width(width), height(height), trim(trim)
        {
        }
    };

    struct APNGMaker
    {
    protected:
        struct ImageHeader
        {
        public:
            std::uint32_t width;
            std::uint32_t height;
            std::uint32_t x;
            std::uint32_t y;
            std::uint8_t depth;
            std::uint8_t ctype;
            std::uint8_t compress;
            std::uint8_t filter;
            std::uint8_t interlace;
            explicit ImageHeader(

                ) = default;

            explicit ImageHeader(
                std::uint32_t width,
                std::uint32_t height,
                std::uint32_t x,
                std::uint32_t y,
                std::uint8_t depth,
                std::uint8_t ctype,
                std::uint8_t compress,
                std::uint8_t filter,
                std::uint8_t interlace) : width(width), height(height), x(x), y(y),
                                          depth(depth), ctype(ctype), compress(compress), filter(filter), interlace(interlace)
            {
            }

            ~ImageHeader(

                ) = default;
        };

        struct ImageData
        {
            std::vector<std::vector<std::uint32_t>> pixel_list;
            ImageHeader meta_data;
        };

        inline static constexpr auto idat_uint32 = 1229209940u;

        inline static constexpr auto file_signature = 9894494448401390090ull;

        inline static constexpr auto image_trailer_end = 5279712195050102914ull;

        inline static auto find_idat(
            DataStreamViewBigEndian &stream,
            std::vector<std::vector<std::uint32_t>> &pixel_list) -> void
        {

            auto pixel = std::vector<uint32_t>{};
            auto found = 0;
            while (stream.read_pos < stream.size() - 12ull)
            {
                auto read_num = stream.readUint32();
                if (read_num == idat_uint32)
                {
                    if (found > 0) {
                        // pixel.pop_back(); // delete crc32 checksum;
                        pixel.pop_back(); // delete next idat length;
                        pixel_list.emplace_back(pixel);
                    }
                    pixel.clear();
                    ++found;
                }
                else {
                    pixel.emplace_back(read_num);
                }
            }
            pixel_list.emplace_back(pixel);
            return;
        }

        inline static auto to_uint32_vector(
            DataStreamViewBigEndian &stream,
            std::vector<uint32_t> &pixel_list) -> void
        {

            return;
        }

        inline static auto load_image_path(
            const std::vector<std::string> &image_path_list,
            std::vector<ImageData> &image_data_list) -> void
        {
            for (const auto &image_path : image_path_list)
            {
                auto stream = DataStreamViewBigEndian{image_path};
                assert_conditional(stream.readString(4, 0xCull) == "IHDR", String::format(fmt::format("{}", Language::get("mismatch_image_magic")), String::to_posix_style(image_path)), "load_image_path");
                auto image_data = ImageData{
                    .meta_data = ImageHeader{
                        stream.readUint32(),
                        stream.readUint32(),
                        0,
                        0,
                        stream.readUint8(),
                        stream.readUint8(),
                        stream.readUint8(),
                        stream.readUint8(),
                        stream.readUint8()}};
                find_idat(stream, image_data.pixel_list);
                image_data_list.emplace_back(image_data);
            }
            return;
        };

    public:
        inline static auto process_fs(
            const std::vector<std::string> &image_path_list,
            std::string_view destination,
            APNGMakerSetting *setting) -> void
        {
            auto image_data_list = std::vector<ImageData>{};
            load_image_path(image_path_list, image_data_list);
            if (setting->width == 0 || setting->height == 0)
            {
                auto max_width = std::vector<uint32_t>{};
                auto max_height = std::vector<uint32_t>{};
                for (auto &image_data : image_data_list)
                {
                    max_width.emplace_back(image_data.meta_data.width);
                    max_height.emplace_back(image_data.meta_data.height);
                }
                if (setting->width == 0)
                {
                    setting->width = *std::max_element(max_width.begin(), max_width.end());
                }
                if (setting->height == 0)
                {
                    setting->height = *std::max_element(max_height.begin(), max_height.end());
                }
            }
            auto stream = DataStreamViewBigEndian{};
            stream.writeUint64(file_signature);
            // IHDR
            stream.writeUint32(13);
            stream.writeString("IHDR");
            stream.writeUint32(setting->width);
            stream.writeUint32(setting->height);
            stream.writeUint8(8);
            stream.writeUint8(6);
            stream.writeNull(3);
            stream.writeUint32(Encryption::CRC32::Normal::compute(0, stream.readBytes(17, 12ull)));
            auto is_animated = image_data_list.size() > 1;
            if (is_animated)
            {
                stream.writeUint32(8);
                stream.writeString("acTL");
                stream.writeUint32(image_data_list.size());
                stream.writeUint32(setting->loop);
                stream.writeUint32(Encryption::CRC32::Normal::compute(0, stream.readBytes(12, 37ull)));
            }
            auto frame_index = 0;
            for (const auto &i : Range<int>(image_data_list.size()))
            {
                const auto &image_data = image_data_list[i];
                if (is_animated)
                {
                    stream.writeUint32(26);
                    auto pos = stream.write_pos;
                    stream.writeString("fcTL");
                    stream.writeUint32(frame_index++);
                    stream.writeUint32(image_data.meta_data.width);
                    stream.writeUint32(image_data.meta_data.height);
                    stream.writeUint32(image_data.meta_data.x);
                    stream.writeUint32(image_data.meta_data.y);
                    stream.writeUint16(static_cast<uint16_t>(setting->delay_frames_list.at(i)));
                    stream.writeUint16(1000);
                    stream.writeNull(2);
                    stream.writeUint32(Encryption::CRC32::Normal::compute(0, stream.readBytes(30, pos)));
                }
                if (i > 0)
                {
                    for (const auto &pixel : image_data.pixel_list)
                    {
                        auto data_stream = DataStreamViewBigEndian{};
                        auto data_size = static_cast<uint32_t>(pixel.size()  * 4);
                        stream.writeUint32(data_size);
                        data_stream.writeString("fdAT");
                        data_stream.writeUint32(frame_index++);
                        for (const auto &k : Range<int>(pixel.size() - 1)) {
                            data_stream.writeUint32(pixel[k]);
                        }
                        auto data = data_stream.toBytes();
                        stream.writeBytes(data);
                        stream.writeUint32(Encryption::CRC32::Normal::compute(0, data));
                    }
                }
                else
                {
                    for (const auto &pixel : image_data.pixel_list)
                    {
                        stream.writeUint32(static_cast<uint32_t>((pixel.size() - 1) * 4));
                        stream.writeString("IDAT");
                        for (const auto &pixel_num : pixel)
                        {
                            stream.writeUint32(pixel_num);
                        }
                    }
                }
            }
            stream.writeNull(4);
            stream.writeUint64(image_trailer_end);
            stream.out_file(destination);
            return;
        }
    };
}