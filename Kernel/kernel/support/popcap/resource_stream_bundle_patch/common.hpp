#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_bundle/definition.hpp"
#include "kernel/support/popcap/resource_stream_bundle/unpack.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundlePatch
{
    using namespace Definition;

    struct Common
    {
        inline static constexpr auto k_magic_identifier = uint32_t{0x52534250};

        inline static constexpr auto k_magic_version = uint32_t{1};

        inline static constexpr auto k_unknown_section_1 = uint32_t{2};

        inline static constexpr auto k_hash_section_size = size_t{16};

        inline static constexpr auto k_begin_index = size_t{0};

        inline static constexpr auto information_header_section_size = size_t{48};

        struct PackageInformation
        {
            uint32_t magic;
            uint32_t version;
            uint32_t unknown_1;
            uint32_t all_after_size;
            uint32_t patch_size;
            std::string before_hash;
            uint32_t packet_count;
            uint32_t patch_exist;
        };

        inline static auto bytes_to_hex_string(
            std::vector<uint8_t> const &data) -> std::string
        {
            std::stringstream result;
            for (auto &v : data)
            {
                result
                    << std::setfill('0') << std::setw(sizeof(v) * 2)
                    << std::hex << +v;
            }
            return result.str();
        }

        inline static auto hex_string_to_bytes(const std::string &data) -> std::vector<uint8_t>
        {
            auto result = std::vector<uint8_t>{};
            for (auto i = 0; i < data.length(); i += 2)
            {
                auto byte_string = data.substr(i, 2);
                auto byte_value = static_cast<uint8_t>(stoi(byte_string, nullptr, 16));
                result.emplace_back(byte_value);
            }

            return result;
        }

        inline static auto exchange_package_information(
            DataStreamView &stream,
            PackageInformation &value) -> void
        {
            stream.read_pos = k_begin_index;
            value.magic = stream.readUint32();
            value.version = stream.readUint32();
            value.unknown_1 = stream.readUint32();
            value.all_after_size = stream.readUint32();
            stream.read_pos += 4;
            value.patch_size = stream.readUint32();
            value.before_hash = bytes_to_hex_string(stream.readBytes(k_hash_section_size));
            value.packet_count = stream.readUint32();
            value.patch_exist = stream.readUint32();
            return;
        }

        inline static auto exchange_package_information(
            PackageInformation const &value,
            DataStreamView &stream) -> void
        {
            stream.write_pos = k_begin_index;
            stream.writeUint32(value.magic);
            stream.writeUint32(value.version);
            stream.writeUint32(value.unknown_1);
            stream.writeUint32(value.all_after_size);
            stream.writeNull(4);
            stream.writeUint32(value.patch_size);
            stream.writeBytes(hex_string_to_bytes(value.before_hash));
            stream.writeUint32(value.packet_count);
            stream.writeUint32(value.patch_exist);
            return;
        }

        struct PacketInformation
        {
            uint32_t patch_exist;
            uint32_t patch_size;
            std::string name;
            std::string before_hash;
        };

        inline static auto exchange_packet_information(
            DataStreamView &stream,
            PacketInformation &value) -> void
        {
            value.patch_exist = stream.readUint32();
            value.patch_size = stream.readUint32();
            value.name = ResourceStreamBundle::Common::exchange_string_block<size_t{128}>(stream);
            value.before_hash = bytes_to_hex_string(stream.readBytes(k_hash_section_size));
            return;
        }

        inline static auto exchange_packet_information(
            PacketInformation const &value,
            DataStreamView &stream) -> void
        {
            stream.writeUint32(value.patch_exist);
            stream.writeUint32(value.patch_size);
            ResourceStreamBundle::Common::exchange_string_block<size_t{128}>(stream, value.name);
            stream.writeBytes(hex_string_to_bytes(value.before_hash));
            return;
        }

        inline static auto indexing_subgroup_information_by_id(
            std::vector<ResourceStreamBundle::Common::BasicSubgroupInformation> &list,
            std::map<std::string, size_t> &index_map) -> void
        {
            for (auto index : Range(list.size()))
            {
                index_map[list[index].id] = index;
            }
            return;
        }

        inline static auto read_package_information_structure(
            DataStreamView &stream,
            ResourceStreamBundle::Common::Information &value) -> void
        {
            ResourceStreamBundle::Common::exchange_to_header(stream, value.header);
            assert_conditional(value.header.magic == ResourceStreamBundle::Common::k_magic_identifier, fmt::format("{}", Language::get("popcap.rsb_patch.invalid_magic_header")), "process_package");
            auto index = std::find(ResourceStreamBundle::Common::k_version_list.begin(), ResourceStreamBundle::Common::k_version_list.end(), static_cast<int>(value.header.version));
            assert_conditional((index != ResourceStreamBundle::Common::k_version_list.end()), String::format(fmt::format("{}", Language::get("popcap.rsb.invalid_rsb_version")), std::to_string(value.header.version)), "process");
            stream.read_pos = value.header.subgroup_information_section_offset;
            value.subgroup_information.resize(value.header.subgroup_information_section_block_count);
            for (auto &subgroup_information : value.subgroup_information)
            {
                if (value.header.version == 1)
                {
                    ResourceStreamBundle::Common::exchange_to_basic_subgroup<uint32_t{1}>(stream, subgroup_information);
                }
                else
                {
                    ResourceStreamBundle::Common::exchange_to_basic_subgroup<uint32_t{3}>(stream, subgroup_information);
                }
            }
            return;
        }
    };
}