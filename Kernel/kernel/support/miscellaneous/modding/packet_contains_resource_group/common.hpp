#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/invoke.hpp"
#include "kernel/support/miscellaneous/shared.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::PacketContainsResourceGroup
{
    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using namespace Definition;

    using ImageFormat = Sen::Kernel::Support::Texture::Format;

    struct Common
    {
        inline static constexpr auto k_magic_identifier = 0x50435247_ui;

        inline static constexpr auto k_version_list = std::array<int, 3>{1, 3, 4};

        inline static constexpr auto k_empty_section_block_size = 0x08;

        inline static constexpr auto string_block_size = 128_size;

        struct HeaderInformaiton
        {
            uint32_t magic;
            uint32_t version;
            bool is_composite; // uint32_t
            uint32_t subgroup_information_section_block_count;
            uint32_t subgroup_information_section_offset;
            uint32_t subgroup_information_section_block_size;
            uint32_t information_section_size;
        };

        inline static auto exchange_head_information(
            DataStreamView &stream,
            HeaderInformaiton &value
        ) -> void
        {
            stream.read_pos = k_begin_index;
            value.magic = stream.readUint32();
            value.version = stream.readUint32();
            stream.read_pos += static_cast<size_t>(0x08);
            value.is_composite = static_cast<bool>(stream.readUint32());
            value.subgroup_information_section_block_count = stream.readUint32();
            value.subgroup_information_section_offset = stream.readUint32();
            value.subgroup_information_section_block_size = stream.readUint32();
            value.information_section_size = stream.readUint32();
            return;
        }

        inline static auto exchange_head_information(
            HeaderInformaiton const &value,
            DataStreamView &stream
        ) -> void
        {
            stream.write_pos = k_begin_index;
            stream.writeUint32(value.magic);
            stream.writeUint32(value.version);
            stream.writeNull(static_cast<size_t>(0x08));
            stream.writeUint32(static_cast<uint32_t>(value.is_composite));
            stream.writeUint32(value.subgroup_information_section_block_count);
            stream.writeUint32(value.subgroup_information_section_offset);
            stream.writeUint32(value.subgroup_information_section_block_size);
            stream.writeUint32(value.information_section_size);
            return;
        }

        enum DataType : uint8_t
        {
            File,             // 0
            Image,            // 1 - ptx
            PopAnim,          // 2 - pam
            Data,             // 3 - rton
            SoundBank,        // 4 - bank
            DecodedSoundBank, // 5 - bank
            PrimeFont,        // 6 - font
            RenderEffect      // 7 effect
        };

        struct PacketInformation
        {
            DataType data_type;
            ImageFormat image_format;
        };

        inline static auto exchange_packet_information(
            DataStreamView &stream,
            PacketInformation &value
        ) -> void
        {
            value.data_type = static_cast<DataType>(stream.readUint32());
            if (value.data_type == DataType::Image) {
                value.image_format = static_cast<ImageFormat>(stream.readUint32());
            }
            return;
        }

        inline static auto exchange_packet_information(
            PacketInformation const &value,
            DataStreamView &stream
        ) -> void 
        {
            stream.writeUint32(static_cast<uint32_t>(value.data_type));
            if (value.data_type == DataType::Image) {
                try_assert(static_cast<uint32_t>(value.image_format) != 0_ui, "invaild_format");
                stream.writeUint32(static_cast<uint32_t>(value.image_format));
            }
            return;
        }

        struct SubgroupInformation
        {
            string id;
            uint32_t data_pos;
            uint32_t data_size;
            uint32_t info_pos;
            uint32_t info_size;
            PacketInformation packet_info;
        };

        inline static auto exchange_to_subgroup(
            DataStreamView &stream,
            SubgroupInformation &value
        ) -> void
        {
            exchange_string_by_string_block_size(stream, value.id);
            value.data_pos = stream.readUint32();
            value.data_size = stream.readUint32();
            value.info_pos = stream.readUint32();
            value.info_size = stream.readUint32();
            exchange_packet_information(stream, value.packet_info);
            return;
        }

        inline static auto exchange_from_subgroup(
            DataStreamView &stream,
            SubgroupInformation const &value
        ) -> void
        {
            exchange_string_by_string_block_size(value.id, stream);
            stream.writeUint32(value.data_pos);
            stream.writeUint32(value.data_size);
            stream.writeUint32(value.info_pos);
            stream.writeUint32(value.info_size);
            exchange_packet_information(value.packet_info, stream);
            return;
        }

        inline static auto exchange_string_by_string_block_size(
            DataStreamView &stream,
            string &value
        ) -> void
        {
            auto string_size = stream.readUint8();
            try_assert(string_size < string_block_size, "out_of_range");
            value = stream.readString(static_cast<size_t>(string_size));
            return;
        } 

        inline static auto exchange_string_by_string_block_size(
            string const &value,
            DataStreamView &stream
        ) -> void
        {
            try_assert(value.size() < string_block_size, "out_of_range");
            stream.writeUint8(static_cast<uint8_t>(value.size()));
            stream.writeString(value);
            return;
        } 
    };
}