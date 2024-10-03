#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/definition.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamGroup
{
    using namespace Definition;

    struct Common
    {
        inline static constexpr auto k_magic_identifier = 0x72736770_ui;

        inline static constexpr auto k_version_list = std::array<int, 3>{1, 3, 4};

        inline static constexpr auto k_compression_list = std::array<int, 4>{0, 1, 2, 3};

        inline static constexpr auto k_general_type_string = "general"_sv;

        inline static constexpr auto k_texture_type_string = "texture"_sv;

        inline static constexpr auto k_packet_compression_flag_count = 2_size;

        inline static constexpr auto information_header_section_size = 92_size;

        inline static constexpr auto information_resource_general_additional_section_size = 12_size;

        inline static constexpr auto information_resource_texture_additional_section_size = 32_size;

        inline static constexpr auto information_resource_texture_unknown_bytes_size = 8_size;

        inline static constexpr auto k_begin_index = 0_size;

        struct PacketCompressionFlag
        {
            inline static constexpr auto texture = 0_size;

            inline static constexpr auto general = 1_size;
        };

        struct PacketCompression
        {
            bool general;
            bool texture;
        };

        struct HeaderInformaiton
        {
            uint32_t magic;
            uint32_t version;
            uint32_t resource_data_section_compression;
            uint32_t information_section_size;
            uint32_t general_resource_data_section_offset;
            uint32_t general_resource_data_section_size;
            uint32_t general_resource_data_section_size_original;
            uint32_t texture_resource_data_section_offset;
            uint32_t texture_resource_data_section_size;
            uint32_t texture_resource_data_section_size_original;
            uint32_t resource_information_section_size;
            uint32_t resource_information_section_offset;
        };

        struct TextureInfo
        {
            int index;
            int width;
            int height;
        };

        inline static auto exchange_header(
            DataStreamView &stream,
            HeaderInformaiton &value) -> void
        {
            stream.read_pos = k_begin_index;
            value.magic = stream.readUint32();
            value.version = stream.readUint32();
            stream.read_pos += 8_size;
            value.resource_data_section_compression = stream.readUint32();
            value.information_section_size = stream.readUint32();
            value.general_resource_data_section_offset = stream.readUint32();
            value.general_resource_data_section_size = stream.readUint32();
            value.general_resource_data_section_size_original = stream.readUint32();
            stream.read_pos += 4_size;
            value.texture_resource_data_section_offset = stream.readUint32();
            value.texture_resource_data_section_size = stream.readUint32();
            value.texture_resource_data_section_size_original = stream.readUint32();
            stream.read_pos += 20_size;
            value.resource_information_section_size = stream.readUint32();
            value.resource_information_section_offset = stream.readUint32();
            stream.read_pos += 12_size;
            return;
        }

        inline static auto exchange_header(
            HeaderInformaiton const &value,
            DataStreamView &stream) -> void
        {
            stream.write_pos = k_begin_index;
            stream.writeUint32(value.magic);
            stream.writeUint32(value.version);
            stream.writeNull(8_size);
            stream.writeUint32(value.resource_data_section_compression);
            stream.writeUint32(value.information_section_size);
            stream.writeUint32(value.general_resource_data_section_offset);
            stream.writeUint32(value.general_resource_data_section_size);
            stream.writeUint32(value.general_resource_data_section_size_original);
            stream.writeNull(4_size);
            stream.writeUint32(value.texture_resource_data_section_offset);
            stream.writeUint32(value.texture_resource_data_section_size);
            stream.writeUint32(value.texture_resource_data_section_size_original);
            stream.writeNull(20_size);
            stream.writeUint32(value.resource_information_section_size);
            stream.writeUint32(value.resource_information_section_offset);
            return;
        }

        struct ResourceInformation
        {
            uint32_t resource_data_section_size;
            uint32_t resource_data_section_offset;
            bool read_texture_additional;
            TextureInfo texture_value;
        };

        inline static auto exchange_to_resource_infomation(
            DataStreamView &stream,
            ResourceInformation &value) -> void
        {
            auto read_texture_additional = static_cast<bool>(stream.readUint32());
            value.resource_data_section_offset = stream.readUint32();
            value.resource_data_section_size = stream.readUint32();
            if (read_texture_additional)
            {
                value.read_texture_additional = read_texture_additional;
                value.texture_value.index = stream.readUint32();
                stream.read_pos += information_resource_texture_unknown_bytes_size;
                value.texture_value.width = stream.readUint32();
                value.texture_value.height = stream.readUint32();
            }
            return;
        }

        inline static auto exchange_from_resource_infomation(
            DataStreamView &stream,
            ResourceInformation const &value) -> void
        {
            stream.writeUint32(static_cast<uint32_t>(value.read_texture_additional));
            stream.writeUint32(value.resource_data_section_offset);
            stream.writeUint32(value.resource_data_section_size);
            if (value.read_texture_additional)
            {
                stream.writeUint32(value.texture_value.index);
                stream.writeNull(information_resource_texture_unknown_bytes_size);
                stream.writeUint32(value.texture_value.width);
                stream.writeUint32(value.texture_value.height);
            }
            return;
        }

        inline static auto packet_compression_to_data(
            uint32_t &data,
            PacketCompression const &value) -> void
        {
            auto data_bit = std::bitset<k_packet_compression_flag_count>{};
            data_bit.set(PacketCompressionFlag::general, value.general);
            data_bit.set(PacketCompressionFlag::texture, value.texture);
            data = static_cast<uint32_t>(data_bit.to_ullong());
            return;
        }

        inline static auto packet_compression_from_data(
            uint32_t const &data,
            PacketCompression &value) -> void
        {
            auto data_bit = std::bitset<k_packet_compression_flag_count>(static_cast<uint8_t>(data));
            value.general = data_bit.test(PacketCompressionFlag::general);
            value.texture = data_bit.test(PacketCompressionFlag::texture);
            return;
        }
    };
}
