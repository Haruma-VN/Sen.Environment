#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle
{

    using namespace Definition;

    struct Common
    {
        inline static constexpr auto k_magic_identifier = 0x72736231_ui;

        inline static constexpr auto k_version_list = std::array<int, 3>{1, 3, 4};

        inline static constexpr auto k_texture_resource_information_section_block_size_list = std::array<size_t, 3>{16, 20, 24};

        inline static constexpr auto k_texture_resource_information_section_block_size_version_0 = 16_size;

        inline static constexpr auto k_texture_resource_information_section_block_size_version_1 = 20_size;

        inline static constexpr auto k_texture_resource_information_section_block_size_version_2 = 24_size;

        inline static constexpr auto k_suffix_of_composite_shell = "_CompositeShell"_sv;

        inline static constexpr auto k_suffix_of_automation_pool = "_AutoPool"_sv;

        inline static constexpr auto information_header_section_size = 112_size;

        inline static constexpr auto resource_basic_detail_section_size = 28_size;

        inline static constexpr auto k_begin_index = 0_size;

        inline static constexpr auto k_none_size = 0_size;

        inline static constexpr auto k_subgroup_name_string_block_size = 128_size;

        inline static auto fourcc_from_integer(
            uint32_t const &data,
            std::string &locale) -> void
        {
            locale += static_cast<char>(clip_bit(data, 24_size, 8_size));
            locale += static_cast<char>(clip_bit(data, 16_size, 8_size));
            locale += static_cast<char>(clip_bit(data, 8_size, 8_size));
            locale += static_cast<char>(clip_bit(data, 0_size, 8_size));
            try_assert(locale.size() == 4_size, "invalid_locale_size");
            return;
        }

        inline static auto fourcc_to_integer(
            std::string const &locale,
            uint32_t &data) -> void
        {
            try_assert(locale.size() == 4_size, "invalid_locale_size");
            data |= static_cast<uint32_t>(locale[0] << 0);
            data |= static_cast<uint32_t>(locale[1] << 8);
            data |= static_cast<uint32_t>(locale[2] << 16);
            data |= static_cast<uint32_t>(locale[3] << 24);
            return;
        }

        enum TextureInformationVersion {
            SectionVersion0,
            SectionVersion1,
            SectionVersion2
        };

        inline static auto exchange_texture_information_version(
            size_t const &texture_information_section_size
        ) -> TextureInformationVersion
        {
            switch(texture_information_section_size) {
                case k_texture_resource_information_section_block_size_version_0: 
                {
                    return TextureInformationVersion::SectionVersion0;
                }
                case k_texture_resource_information_section_block_size_version_1:
                {
                    return TextureInformationVersion::SectionVersion1;
                }
                case k_texture_resource_information_section_block_size_version_2: {
                    return TextureInformationVersion::SectionVersion2;
                }
                default: {
                    assert_conditional(false, String::format(fmt::format("{}", Language::get("popcap.rsb.invalid_texture_information_section_size")), std::to_string(texture_information_section_size)), "exchange_texture_information_version");
                }
            }
        }

        inline static auto exchange_texture_information_version(
            TextureInformationVersion const &texture_information_version
        ) -> size_t 
        {
            switch(texture_information_version) {
                case TextureInformationVersion::SectionVersion0: 
                {
                    return k_texture_resource_information_section_block_size_version_0;
                }
                case TextureInformationVersion::SectionVersion1:
                {
                    return k_texture_resource_information_section_block_size_version_1;
                }
                case TextureInformationVersion::SectionVersion2: {
                    return k_texture_resource_information_section_block_size_version_2;
                }
                default: {
                    assert_conditional(false, String::format(fmt::format("{}", Language::get("popcap.rsb.popcap.rsb.texture_information_version")), std::to_string(static_cast<int>(texture_information_version))), "exchange_texture_information_version");
                }
            }
        }

        struct HeaderInformaiton
        {
            uint32_t magic;
            uint32_t version;
            uint32_t unknown_1;
            uint32_t information_section_size;
            uint32_t resource_path_section_size;
            uint32_t resource_path_section_offset;
            uint32_t subgroup_id_section_size;
            uint32_t subgroup_id_section_offset;
            uint32_t subgroup_information_section_block_count;
            uint32_t subgroup_information_section_offset;
            uint32_t subgroup_information_section_block_size;
            uint32_t group_information_section_block_count;
            uint32_t group_information_section_offset;
            uint32_t group_information_section_block_size;
            uint32_t group_id_section_size;
            uint32_t group_id_section_offset;
            uint32_t pool_information_section_block_count;
            uint32_t pool_information_section_offset;
            uint32_t pool_information_section_block_size;
            uint32_t texture_resource_information_section_block_count;
            uint32_t texture_resource_information_section_offset;
            uint32_t texture_resource_information_section_block_size;
            uint32_t group_manifest_information_section_offset;
            uint32_t resource_manifest_information_section_offset;
            uint32_t string_manifest_information_section_offset;
            uint32_t information_without_manifest_section_size;
        };

        inline static auto exchange_to_header(
            DataStreamView &stream,
            HeaderInformaiton &value) -> void
        {
            stream.read_pos = k_begin_index;
            value.magic = stream.readUint32();
            value.version = stream.readUint32();
            value.unknown_1 = stream.readUint32();
            value.information_section_size = stream.readUint32();
            value.resource_path_section_size = stream.readUint32();
            value.resource_path_section_offset = stream.readUint32();
            stream.read_pos += 8_size;
            value.subgroup_id_section_size = stream.readUint32();
            value.subgroup_id_section_offset = stream.readUint32();
            value.subgroup_information_section_block_count = stream.readUint32();
            value.subgroup_information_section_offset = stream.readUint32();
            value.subgroup_information_section_block_size = stream.readUint32();
            value.group_information_section_block_count = stream.readUint32();
            value.group_information_section_offset = stream.readUint32();
            value.group_information_section_block_size = stream.readUint32();
            value.group_id_section_size = stream.readUint32();
            value.group_id_section_offset = stream.readUint32();
            value.pool_information_section_block_count = stream.readUint32();
            value.pool_information_section_offset = stream.readUint32();
            value.pool_information_section_block_size = stream.readUint32();
            value.texture_resource_information_section_block_count = stream.readUint32();
            value.texture_resource_information_section_offset = stream.readUint32();
            value.texture_resource_information_section_block_size = stream.readUint32();
            value.group_manifest_information_section_offset = stream.readUint32();
            value.resource_manifest_information_section_offset = stream.readUint32();
            value.string_manifest_information_section_offset = stream.readUint32();
            value.information_without_manifest_section_size = stream.readUint32();
            return;
        }
        
        inline static auto exchange_from_header(
            DataStreamView &stream,
            HeaderInformaiton const &value) -> void
        {
            stream.write_pos = k_begin_index;
            stream.writeUint32(value.magic);
            stream.writeUint32(value.version);
            stream.writeUint32(value.unknown_1);
            stream.writeUint32(value.information_section_size);
            stream.writeUint32(value.resource_path_section_size);
            stream.writeUint32(value.resource_path_section_offset);
            stream.write_pos += 8_size;
            stream.writeUint32(value.subgroup_id_section_size);
            stream.writeUint32(value.subgroup_id_section_offset);
            stream.writeUint32(value.subgroup_information_section_block_count);
            stream.writeUint32(value.subgroup_information_section_offset);
            stream.writeUint32(value.subgroup_information_section_block_size);
            stream.writeUint32(value.group_information_section_block_count);
            stream.writeUint32(value.group_information_section_offset);
            stream.writeUint32(value.group_information_section_block_size);
            stream.writeUint32(value.group_id_section_size);
            stream.writeUint32(value.group_id_section_offset);
            stream.writeUint32(value.pool_information_section_block_count);
            stream.writeUint32(value.pool_information_section_offset);
            stream.writeUint32(value.pool_information_section_block_size);
            stream.writeUint32(value.texture_resource_information_section_block_count);
            stream.writeUint32(value.texture_resource_information_section_offset);
            stream.writeUint32(value.texture_resource_information_section_block_size);
            stream.writeUint32(value.group_manifest_information_section_offset);
            stream.writeUint32(value.resource_manifest_information_section_offset);
            stream.writeUint32(value.string_manifest_information_section_offset);
            if (value.information_without_manifest_section_size != k_none_size) {
                stream.writeUint32(value.information_without_manifest_section_size);
            }
            return;
        }

        struct SimpleSubgroupInformation
        {
            uint32_t index;
            uint32_t resolution;
            uint32_t locale;
        };

        inline static auto exchange_to_simple_subgroup(
            DataStreamView &stream,
            SimpleSubgroupInformation &value) -> void
        {
            value.index = stream.readUint32();
            value.resolution = stream.readUint32();
            value.locale = stream.readUint32();
            stream.read_pos += 4_size;
            return;
        }

        inline static auto exchange_from_simple_subgroup(
            DataStreamView &stream,
            SimpleSubgroupInformation const &value
            ) -> void
        {
            stream.writeUint32(value.index);
            stream.writeUint32(value.resolution);
            stream.writeUint32(value.locale);
            stream.writeNull(4_size);
            return;
        }

        struct SimpleGroupInformation
        {
            std::string id;
            std::vector<SimpleSubgroupInformation> subgroup_information;
            uint32_t subgroup_count;
        };

        inline static auto exchange_to_simple_group(
            DataStreamView &stream,
            SimpleGroupInformation &value) -> void
        {
            value.id = exchange_string_block<k_subgroup_name_string_block_size>(stream);
            auto before_pos = stream.read_pos;
            stream.read_pos += 1024_size;
            value.subgroup_count = stream.readUint32();
            auto end_pos = stream.read_pos;
            stream.read_pos = before_pos;
            value.subgroup_information.resize(value.subgroup_count);
            for (auto i : Range(value.subgroup_count))
            {
                exchange_to_simple_subgroup(stream, value.subgroup_information[i]);
            }
            stream.read_pos = end_pos;
            return;
        }

        inline static auto exchange_from_simple_group(
            DataStreamView &stream,
            SimpleGroupInformation const &value
            ) -> void
        {
            exchange_string_block<k_subgroup_name_string_block_size>(stream, value.id);
            auto padding_to_write = value.subgroup_count * 16_size;
            try_assert(padding_to_write < 1024_size, "out_of_range");
            for (auto i : Range(value.subgroup_count))
            {
                exchange_from_simple_subgroup(stream, value.subgroup_information[i]);
            }
            stream.writeNull(static_cast<size_t>(1024_size - padding_to_write));
            stream.writeUint32(value.subgroup_count);
            return;
        }

        struct BasicSubgroupInformation
        {
            std::string id;
            uint32_t offset;
            uint32_t size;
            uint32_t pool;
            uint32_t resource_data_section_compression;
            uint32_t information_section_size;
            uint32_t general_resource_data_section_offset;
            uint32_t general_resource_data_section_size;
            uint32_t general_resource_data_section_size_original;
            uint32_t general_resource_data_section_size_pool;
            uint32_t texture_resource_data_section_offset;
            uint32_t texture_resource_data_section_size;
            uint32_t texture_resource_data_section_size_original;
            uint32_t texture_resource_data_section_size_pool;
            uint32_t texture_resource_count;
            uint32_t texture_resource_begin;
        };

        template <auto t_version>
        inline static auto exchange_to_basic_subgroup(
            DataStreamView &stream,
            BasicSubgroupInformation &value) -> void
        {
            value.id = exchange_string_block<k_subgroup_name_string_block_size>(stream);
            value.offset = stream.readUint32();
            value.size = stream.readUint32();
            value.pool = stream.readUint32();
            value.resource_data_section_compression = stream.readUint32();
            value.information_section_size = stream.readUint32();
            value.general_resource_data_section_offset = stream.readUint32();
            value.general_resource_data_section_size = stream.readUint32();
            value.general_resource_data_section_size_original = stream.readUint32();
            value.general_resource_data_section_size_pool = stream.readUint32();
            value.texture_resource_data_section_offset = stream.readUint32();
            value.texture_resource_data_section_size = stream.readUint32();
            value.texture_resource_data_section_size_original = stream.readUint32();
            value.texture_resource_data_section_size_pool = stream.readUint32();
            stream.read_pos += 16_size;
            if constexpr (t_version >= 3_size)
            {
                value.texture_resource_count = stream.readUint32();
                value.texture_resource_begin = stream.readUint32();
            }
            return;
        }

        template <auto t_version>
        inline static auto exchange_from_basic_subgroup(
            DataStreamView &stream,
            BasicSubgroupInformation const &value) -> void
        {
            static_assert(sizeof(t_version) == sizeof(uint32_t));
            exchange_string_block<k_subgroup_name_string_block_size>(stream, value.id);
            stream.writeUint32(value.offset);
            stream.writeUint32(value.size);
            stream.writeUint32(value.pool);
            stream.writeUint32(value.resource_data_section_compression);
            stream.writeUint32(value.information_section_size);
            stream.writeUint32(value.general_resource_data_section_offset);
            stream.writeUint32(value.general_resource_data_section_size);
            stream.writeUint32(value.general_resource_data_section_size_original);
            stream.writeUint32(value.general_resource_data_section_size_pool);
            stream.writeUint32(value.texture_resource_data_section_offset);
            stream.writeUint32(value.texture_resource_data_section_size);
            stream.writeUint32(value.texture_resource_data_section_size_original);
            stream.writeUint32(value.texture_resource_data_section_size_pool);
            stream.writeNull(16);
            if constexpr (t_version >= 3_size)
            {
                stream.writeUint32(value.texture_resource_count);
                stream.writeUint32(value.texture_resource_begin);
            }
            return;
        }

        struct PoolInformation
        {
            std::string id;
            uint32_t texture_resource_data_section_offset;
            uint32_t texture_resource_data_section_size;
            uint32_t instance_count;
            uint32_t flag;
            uint32_t texture_resource_count;
            uint32_t texture_resource_begin;
        };

        inline static auto exchange_to_pool(
            DataStreamView &stream,
            PoolInformation &value) -> void
        {
            value.id = exchange_string_block<128_size>(stream);
            value.texture_resource_data_section_offset = stream.readUint32();
            value.texture_resource_data_section_size = stream.readUint32();
            value.instance_count = stream.readUint32();
            value.flag = stream.readUint32();
            value.texture_resource_count = stream.readUint32();
            value.texture_resource_begin = stream.readUint32();
            return;
        }

        inline static auto exchange_from_pool(
            DataStreamView &stream,
            PoolInformation const &value) -> void
        {
            exchange_string_block<128_size>(stream, value.id);
            stream.writeUint32(value.texture_resource_data_section_offset);
            stream.writeUint32(value.texture_resource_data_section_size);
            stream.writeUint32(value.instance_count);
            stream.writeUint32(value.flag);
            stream.writeUint32(value.texture_resource_count);
            stream.writeUint32(value.texture_resource_begin);
            return;
        }

        struct TextureInfomation
        {
            uint32_t size_width;
            uint32_t size_height;
            uint32_t pitch;
            uint32_t format;
            uint32_t alpha_size;
            uint32_t scale;
        };

        template <auto texture_inforamtion_section_block_size>
        inline static auto exchange_to_texture(
            DataStreamView &stream,
            TextureInfomation &value) -> void
        {
            static_assert(sizeof(texture_inforamtion_section_block_size) == sizeof(size_t));
            value.size_width = stream.readUint32();
            value.size_height = stream.readUint32();
            value.pitch = stream.readUint32();
            value.format = stream.readUint32();
            if (texture_inforamtion_section_block_size == k_texture_resource_information_section_block_size_version_1)
            {
                value.alpha_size = stream.readUint32();
            }
            if (texture_inforamtion_section_block_size == k_texture_resource_information_section_block_size_version_2)
            {
                value.alpha_size = stream.readUint32();
                value.scale = stream.readUint32();
            }
            return;
        }

        template <auto texture_inforamtion_section_block_size>
        inline static auto exchange_from_texture(
            DataStreamView &stream,
            TextureInfomation const &value) -> void
        {
            static_assert(sizeof(texture_inforamtion_section_block_size) == sizeof(size_t));
            stream.writeUint32(value.size_width);
            stream.writeUint32(value.size_height);
            stream.writeUint32(value.pitch);
            stream.writeUint32(value.format);
            if (texture_inforamtion_section_block_size == k_texture_resource_information_section_block_size_version_1)
            {
                stream.writeUint32(value.alpha_size);
            }
            if (texture_inforamtion_section_block_size == k_texture_resource_information_section_block_size_version_2)
            {
                stream.writeUint32(value.alpha_size);
                stream.writeUint32(value.scale);
            }
            return;
        }

        struct Information
        {
            HeaderInformaiton header;
            std::map<std::string, uint32_t> resource_path;
            std::map<std::string, uint32_t> subgroup_id;
            std::vector<SimpleGroupInformation> group_information;
            std::map<std::string, uint32_t> group_id;
            std::vector<BasicSubgroupInformation> subgroup_information;
            std::vector<PoolInformation> pool_information;
            std::map<size_t, TextureInfomation> texture_resource_information; // fix index;
        };

        inline static auto exchange_to_index(
            DataStreamView &stream,
            uint32_t &value) -> void
        {
            value = stream.readUint32();
            return;
        }

        inline static auto exchange_from_index(
            DataStreamView &stream,
            uint32_t const &value) -> void
        {
            stream.writeUint32(value);
            return;
        }

        template <auto string_block_size>
        inline static auto exchange_string_block(
            DataStreamView &stream) -> std::string
        {
            static_assert(sizeof(string_block_size) == sizeof(size_t));
            auto value = stream.readStringByEmpty();
            assert_conditional(value.size() < string_block_size, fmt::format("{}", Language::get("popcap.rsb.out_of_range")),"exchange_string_block");
            stream.read_pos += (string_block_size - value.size() - size_t{1});
            return value;
        }

        template <auto string_block_size>
        inline static auto exchange_string_block(
            DataStreamView &stream,
            std::string const &data) -> void
        {
            static_assert(sizeof(string_block_size) == sizeof(size_t));
            stream.writeString(data);
            stream.writeNull(string_block_size - data.size());
            return;
        }

        //-----------------------------------------------------------------

        struct ResourceBasicDetailManifestInformation
        {
            uint16_t type;
            uint16_t header_size;
            uint32_t property_information_offset;
            uint32_t image_property_information_offset;
            uint32_t id_offset;
            uint32_t path_offset;
            uint32_t property_information_count;
        };

        inline static auto exchange_to_resource_basic(
            DataStreamView &stream,
            ResourceBasicDetailManifestInformation &value) -> void
        {
            stream.read_pos += 4_size;
            value.type = stream.readUint16();
            value.header_size = stream.readUint16();
            try_assert(value.header_size == 0x1C_size, "invalid_header_size");
            value.property_information_offset = stream.readUint32();
            value.image_property_information_offset = stream.readUint32();
            value.id_offset = stream.readUint32();
            value.path_offset = stream.readUint32();
            value.property_information_count = stream.readUint32();
            return;
        }

        inline static auto exchange_from_resource_basic(
            DataStreamView &stream,
            ResourceBasicDetailManifestInformation const &value) -> void
        {
            stream.writeNull(4);
            stream.writeUint16(value.type);
            stream.writeUint16(value.header_size);
            try_assert(value.header_size == 0x1C_size, "invalid_header_size");
            stream.writeUint32(value.property_information_offset);
            stream.writeUint32(value.image_property_information_offset);
            stream.writeUint32(value.id_offset);
            stream.writeUint32(value.path_offset);
            stream.writeUint32(value.property_information_count);
            return;
        }

        struct ResourceImagePropertyDetailManifestInformation
        {
            uint16_t type;
            uint16_t flag;
            uint16_t x;
            uint16_t y;
            uint16_t ax;
            uint16_t ay;
            uint16_t aw;
            uint16_t ah;
            uint16_t rows;
            uint16_t cols;
            uint32_t parent_offset;
        };

        inline static auto exchange_to_resource_image_property(
            DataStreamView &stream,
            ResourceImagePropertyDetailManifestInformation &value) -> void
        {
            value.type = stream.readUint16();
            value.flag = stream.readUint16();
            value.x = stream.readUint16();
            value.y = stream.readUint16();
            value.ax = stream.readUint16();
            value.ay = stream.readUint16();
            value.aw = stream.readUint16();
            value.ah = stream.readUint16();
            value.rows = stream.readUint16();
            value.cols = stream.readUint16();
            value.parent_offset = stream.readUint32();
            return;
        }

        inline static auto exchange_from_resource_image_property(
            DataStreamView &stream,
            ResourceImagePropertyDetailManifestInformation const &value) -> void
        {
            stream.writeUint16(value.type);
            stream.writeUint16(value.flag);
            stream.writeUint16(value.x);
            stream.writeUint16(value.y);
            stream.writeUint16(value.ax);
            stream.writeUint16(value.ay);
            stream.writeUint16(value.aw);
            stream.writeUint16(value.ah);
            stream.writeUint16(value.rows);
            stream.writeUint16(value.cols);
            stream.writeUint32(value.parent_offset);
            return;
        }

        struct ResourcePropertyDetailManifestInformation
        {
            uint32_t key_offset;
            uint32_t value_offset;
        };

        inline static auto exchange_to_resource_property(
            DataStreamView &stream,
            ResourcePropertyDetailManifestInformation &value) -> void
        {
            value.key_offset = stream.readUint32();
            stream.read_pos += 4_size;
            value.value_offset = stream.readUint32();
            return;
        }

        inline static auto exchange_from_resource_property(
            DataStreamView &stream,
            ResourcePropertyDetailManifestInformation const &value) -> void
        {
            stream.writeUint32(value.key_offset);
            stream.writeNull(4_size);
            stream.writeUint32(value.value_offset);
            return;
        }

        struct ResourceBasicManifestInformation
        {
            uint32_t detail_offset;
        };

        inline static auto exchange_to_resource_basic_manifest(
            DataStreamView &stream,
            ResourceBasicManifestInformation &value) -> void
        {
            value.detail_offset = stream.readUint32();
            return;
        }

        inline static auto exchange_from_resource_basic_manifest(
            DataStreamView &stream,
            ResourceBasicManifestInformation const &value) -> void
        {
            stream.writeUint32(value.detail_offset);
            return;
        }

        struct SubgroupBasicManifestInformation
        {
            uint32_t resolution;
            uint32_t locale;
            uint32_t id_offset;
            uint32_t resource_count;
            std::vector<ResourceBasicManifestInformation> resource_information;
        };

        inline static auto exchange_to_subgroup_manifest(
            DataStreamView &stream,
            SubgroupBasicManifestInformation &value) -> void
        {
            value.resolution = stream.readUint32();
            value.locale = stream.readUint32();
            value.id_offset = stream.readUint32();
            value.resource_count = stream.readUint32();
            value.resource_information.resize(value.resource_count);
            for (auto i : Range(value.resource_count))
            {
                exchange_to_resource_basic_manifest(stream, value.resource_information[i]);
            }
            return;
        }

        inline static auto exchange_from_subgroup_manifest(
            DataStreamView &stream,
            SubgroupBasicManifestInformation const &value) -> void
        {
            stream.writeUint32(value.resolution);
            stream.writeUint32(value.locale);
            stream.writeUint32(value.id_offset);
            stream.writeUint32(value.resource_count);
            for (auto i : Range(value.resource_count))
            {
                exchange_from_resource_basic_manifest(stream, value.resource_information[i]);
            }
            return;
        }

        struct GroupManifestInformation
        {
            uint32_t id_offset;
            uint32_t subgroup_count;
            uint32_t subgroup_information_size;
            std::vector<SubgroupBasicManifestInformation> subgroup_information;
        };

        inline static auto exchange_to_group_manifest(
            DataStreamView &stream,
            GroupManifestInformation &value) -> void
        {
            value.id_offset = stream.readUint32();
            value.subgroup_count = stream.readUint32();
            value.subgroup_information_size = stream.readUint32();
            try_assert(value.subgroup_information_size == 0x10_size, "invalid_subgroup_information_size");
            value.subgroup_information.resize(value.subgroup_count);
            for (auto i : Range(value.subgroup_count))
            {
                exchange_to_subgroup_manifest(stream, value.subgroup_information[i]);
            }
            return;
        }

        inline static auto exchange_from_group_manifest(
            DataStreamView &stream,
            GroupManifestInformation const &value) -> void
        {
            stream.writeUint32(value.id_offset);
            stream.writeUint32(value.subgroup_count);
            try_assert(value.subgroup_information_size == 0x10_size, "invalid_subgroup_information_size");
            stream.writeUint32(value.subgroup_information_size);
            for (auto i : Range(value.subgroup_count))
            {
                exchange_from_subgroup_manifest(stream, value.subgroup_information[i]);
            }
            return;
        }

        inline static auto compare_conditional(
            int const &value1,
            int const &value2,
            std::string const & where,
            std::string_view localization
        ) -> void
        {
            assert_conditional(value1 == value2, String::format(fmt::format("{}", Language::get(localization)), where, std::to_string(value1), std::to_string(value2)), "compare_conditional");
            return;
        }
    };
}
