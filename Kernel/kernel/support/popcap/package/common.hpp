#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Package
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Common
    {
        inline static constexpr auto k_magic_package_identifier = static_cast<int32_t>(0xBAC04AC0);

        inline static constexpr auto k_magic_package_pc_version_identifier = static_cast<int32_t>(0x4D37BD37);

        inline static constexpr auto k_magic_package_tv_version_identifier = static_cast<int32_t>(0x04034B50);

        inline static constexpr auto k_magic_package_xmem_compression_identifier = static_cast<int32_t>(0xED12F20F);

        inline static constexpr auto k_version = 0_ui;

        inline static constexpr auto k_none_size = 0_size;

        inline static constexpr auto xor_point = static_cast<int32_t>(0xF7);

        inline static constexpr auto k_time = 129146222018596744_ul;

        inline static auto constexpr zip_stream_deleter = [](auto zip)
        {
            zip_stream_close(zip);
            return;
        };

        struct ResourceInformationListStateFlag
        {
            inline static constexpr auto next = 0x00_byte;

            inline static constexpr auto done = 0x80_byte;
        };

        struct ResourceInformation
        {
            std::string path;
            uint32_t size;
            uint32_t size_original;
            uint64_t time;
        };

        inline static auto compute_padding_size(
            size_t const &data) -> size_t
        {
            auto padding_size = data & 0b111;
            if (padding_size == k_none_size)
            {
                return 6_size;
            }
            else if (padding_size > 5_size)
            {
                return static_cast<size_t>(14_size - padding_size);
            }
            else
            {
                return static_cast<size_t>(6_size - padding_size);
            }
        }

        inline static auto compute_padding_size_fill_block(
            size_t const &data) -> size_t
        {
            auto padding_size = data & (k_padding_unit_size - 1_size);
            if (padding_size == k_none_size)
            {
                return static_cast<size_t>(k_padding_unit_size - 2_size);
            }
            else if (padding_size > k_padding_unit_size - 2_size)
            {
                return static_cast<size_t>(k_padding_unit_size + k_padding_unit_size - 2_size - padding_size);
            }
            else
            {
                return static_cast<size_t>(k_padding_unit_size - 2_size - padding_size);
            }
        }

        inline static auto zip_package_uncompress(
            std::vector<uint8_t> const &data,
            std::string_view resource_directory,
            std::vector<Resource> &value) -> void
        {
            char *output_bytes = nullptr;
            auto bytes_size = size_t{0};
            auto zip_data = std::vector<char>(data.begin(), data.end());
            auto zip = std::unique_ptr<struct zip_t, decltype(zip_stream_deleter)>(zip_stream_open(zip_data.data(), zip_data.size(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r'), zip_stream_deleter);
            int zip_index, zip_size = zip_entries_total(zip.get());
            for (zip_index = 0; zip_index < zip_size; ++zip_index)
            {
                zip_entry_openbyindex(zip.get(), zip_index);
                {
                    const char *name = zip_entry_name(zip.get());
                    zip_entry_read(zip.get(), (void **)&output_bytes, &bytes_size);
                    write_bytes(fmt::format("{}/{}", resource_directory, name), std::vector<uint8_t>(output_bytes, output_bytes + bytes_size));
                    value.emplace_back(Resource{.path = std::string{name}, .time = k_time});
                }
                zip_entry_close(zip.get());
            }
            delete output_bytes;
            return;
        }

        inline static auto zip_package_compress(
            DataStreamView &stream,
            std::string_view resource_directory,
            std::vector<Resource> const &value) -> void
        {
            char *output_bytes = nullptr;
            auto bytes_size = 0_size;
            auto zip = std::unique_ptr<struct zip_t, decltype(zip_stream_deleter)>(zip_stream_open(nullptr, 0, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w'), zip_stream_deleter);
            for (auto &resource : value)
            {
                auto zip_path = resource.path;
                std::replace(zip_path.begin(), zip_path.end(), '\\', '/');
                auto file_path = fmt::format("{}/{}", resource_directory, zip_path);
                zip_entry_open(zip.get(), zip_path.data());
                {
                    auto current_data = FileSystem::read_binary<char>(file_path);
                    zip_entry_write(zip.get(), current_data.data(), current_data.size());
                }
                zip_entry_close(zip.get());
            }
            zip_stream_copy(zip.get(), (void **)&output_bytes, &bytes_size);
            auto stream_bytes = std::vector<uint8_t>(output_bytes, output_bytes + bytes_size);
            stream.writeBytes(stream_bytes);
            delete output_bytes;
            return;
        }
    };
}