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
        inline static constexpr auto k_magic_identifier = 1935894321_ui;

        inline static constexpr auto k_version_list = std::array<int, 3>{1, 3, 4};

        inline static constexpr auto k_empty_section_block_size = 0x08_size;

        inline static constexpr auto string_block_size = 128_size;

        inline static constexpr auto k_data_block_padding_size = 0x10_size;

        inline static constexpr auto k_subgroup_information_section_block_size = 144_size;

        inline static constexpr auto k_packet_general_type_name = "general_type"_sv;

        inline static constexpr auto k_resource_content_information_magic_identifier = 1936876393_ui;

        inline static constexpr auto k_resource_content_information_version = 1_ui;

        struct HeaderInformaiton
        {
            uint32_t magic;
            uint32_t version;
            bool composite; // uint32_t
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
            value.composite = static_cast<bool>(stream.readUint32());
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
            stream.writeUint32(static_cast<uint32_t>(value.composite));
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

        inline static auto exchange_padding_block(
            DataStreamView &stream
        ) -> void
        {
            stream.writeNull(compute_padding_size(stream.write_pos, k_data_block_padding_size));
            return;
        }

        /*
        struct PacketInformation
        {
            uint32_t image_count;
            std::vector<ImageFormat> image_format;
        };

        inline static auto exchange_packet_information(
            DataStreamView &stream,
            PacketInformation &value
        ) -> void
        {
            value.image_count = stream.readUint32();
            for (auto i : Range(value.image_count)) {
                value.image_format.emplace_back(static_cast<ImageFormat>(stream.readUint32()));
            }
            return;
        }

        inline static auto exchange_packet_information(
            PacketInformation const &value,
            DataStreamView &stream
        ) -> void 
        {
            stream.writeUint32(value.image_count);
            for (auto i : Range(value.image_count)) {
                try_assert(static_cast<uint32_t>(value.image_format[i]) != 0_ui, "invalid_format");
                stream.writeUint32(static_cast<uint32_t>(value.image_format[i]));
            }
            return;
        }
        */

        struct ResourceContentInformation 
        {
            uint32_t magic;
            uint32_t version;
            uint32_t information_compressed_size;
            uint32_t information_string_size;
        };

        inline static auto exchange_resouce_content_information(
            DataStreamView &stream,
            ResourceContentInformation &value
        ) -> void
        {
            value.magic = stream.readUint32();
            value.version = stream.readUint32();
            value.information_compressed_size = stream.readUint32();
            value.information_string_size = stream.readUint32();
            /*
            auto compressed_data = stream.readString(static_cast<size_t>(value.info_compressed_size));
            assert_conditional(compressed_data.size() == static_cast<size_t>(value.info_compressed_size), "invaild_compressed_size", "exchange_resouce_content_information");
            value.data = nlohmann::ordered_json::parse(::Kernel::Definition::Encryption::Base64::decode(compressed_data));
            */
            return;
        }

        inline static auto exchange_resouce_content_information(
            ResourceContentInformation const &data,
            DataStreamView &stream
        ) -> void
        {
            try_assert(data.magic == k_resource_content_information_magic_identifier, "invaild_magic");
            try_assert(data.version == k_resource_content_information_version, "invaild_version");
            stream.writeUint32(data.magic);
            stream.writeUint32(data.version);
            stream.writeUint32(data.information_compressed_size);
            stream.writeUint32(data.information_string_size);
            return;
        }

        struct SubgroupInformation
        {
            string id;
            bool is_image;
            uint32_t resource_group_stream_section_offset;
            uint32_t resource_group_stream_section_block_size;
            uint32_t resource_content_information_offset;
        };

        inline static auto exchange_to_subgroup(
            DataStreamView &stream,
            SubgroupInformation &value
        ) -> void
        {
            value.id = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::exchange_string_block<Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::k_subgroup_name_string_block_size>(stream);
            value.is_image = static_cast<bool>(stream.readUint32());
            value.resource_group_stream_section_offset = stream.readUint32();
            value.resource_group_stream_section_block_size = stream.readUint32();
            value.resource_content_information_offset = stream.readUint32();
           // exchange_resouce_content_information(stream, value.resource_content_information);
            return;
        }

        inline static auto exchange_from_subgroup(
            DataStreamView &stream,
            SubgroupInformation const &data
        ) -> void
        {
            Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::exchange_string_block<Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::k_subgroup_name_string_block_size>(stream, data.id);
            stream.writeUint32(static_cast<uint32_t>(data.is_image));
            stream.writeUint32(data.resource_group_stream_section_offset);
            stream.writeUint32(data.resource_group_stream_section_block_size);
            stream.writeUint32(data.resource_content_information_offset);
          //  exchange_resouce_content_information(data.resource_content_information, stream);
            return;
        }
    };
}