#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_bundle/definition.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle
{
    using namespace Definition;

    struct Pack : Common
    {
    protected:
        inline static auto make_standard_group_id(
            std::string const &group_id,
            bool const &is_composite) -> std::string
        {
            if (is_composite)
            {
                return group_id;
            }
            else
            {
                return fmt::format("{}{}", group_id, k_suffix_of_composite_shell);
            }
        }

        inline static auto process_package_manifest(
            DataStreamView &stream,
            HeaderInformaiton &header_structure,
            ManifestStructure const &manifest) -> void
        {
            auto group_manifest_information_structure_stream = DataStreamView{};
            auto resource_manifest_information_data_stream = DataStreamView{};
            auto string_manifest_information_data_stream = DataStreamView{};
            auto string_map = std::map<std::string, size_t>{};
            auto set_string = [&](
                                  std::string const &string) -> uint32_t
            {
                auto result = size_t{};
                auto iterator = string_map.find(string);
                if (iterator != string_map.end())
                {
                    result = (*iterator).second;
                }
                else
                {
                    result = string_manifest_information_data_stream.write_pos;
                    string_map[string] = result;
                    string_manifest_information_data_stream.writeStringByEmpty(string);
                }
                return static_cast<uint32_t>(result);
            };
            set_string("");
            constexpr auto image_property_key_list = std::array<std::string_view, 11>{"type"_sv, "flag"_sv, "x"_sv, "y"_sv, "ax"_sv, "ay"_sv, "aw"_sv, "ah"_sv, "rows"_sv, "cols"_sv, "parent"};
            for (auto &[group_id, group_manifest] : manifest.group)
            {
                auto group_manifest_information_structure = GroupManifestInformation{};
                group_manifest_information_structure.id_offset = set_string(make_standard_group_id(group_id, group_manifest.composite));
                group_manifest_information_structure.subgroup_count = static_cast<uint32_t>(group_manifest.subgroup.size());
                group_manifest_information_structure.subgroup_information_size = 0x10_size;
                for (auto &[subgroup_id, subgroup_manifest] : group_manifest.subgroup)
                {
                    auto subgroup_manifest_information_structure = SubgroupBasicManifestInformation{};
                    subgroup_manifest_information_structure.resolution = subgroup_manifest.category.resolution;
                    if (!subgroup_manifest.category.locale.empty())
                    {
                        fourcc_to_integer(subgroup_manifest.category.locale, subgroup_manifest_information_structure.locale);
                    }
                    subgroup_manifest_information_structure.id_offset = set_string(subgroup_id);
                    subgroup_manifest_information_structure.resource_count = static_cast<uint32_t>(subgroup_manifest.resource.size());
                    for (auto &[resource_id, resource_manifest] : subgroup_manifest.resource)
                    {
                        auto resource_detail_manifest_information_structure = ResourceBasicDetailManifestInformation{};
                        resource_detail_manifest_information_structure.id_offset = set_string(resource_id);
                        resource_detail_manifest_information_structure.path_offset = set_string(String::to_windows_style(resource_manifest.path));
                        resource_detail_manifest_information_structure.header_size = 0x1C_size;
                        resource_detail_manifest_information_structure.type = resource_manifest.type;
                        auto resource_image_property_detail_manifest_information_structure = ResourceImagePropertyDetailManifestInformation{};
                        if (resource_detail_manifest_information_structure.type == 0_us)
                        {
                            resource_image_property_detail_manifest_information_structure.type = resource_manifest.property["type"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.flag = resource_manifest.property["flag"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.x = resource_manifest.property["x"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.y = resource_manifest.property["y"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.ax = resource_manifest.property["ax"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.ay = resource_manifest.property["ay"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.aw = resource_manifest.property["aw"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.ah = resource_manifest.property["ah"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.rows = resource_manifest.property["rows"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.cols = resource_manifest.property["cols"_sv].get<uint16_t>();
                            resource_image_property_detail_manifest_information_structure.parent_offset = set_string(resource_manifest.property["parent"_sv].get<std::string>());
                        }
                        auto resource_property_information_structure_list = std::vector<ResourcePropertyDetailManifestInformation>{};
                        for (auto &[property_id, property_value] : resource_manifest.property.items())
                        {
                            if (!(resource_detail_manifest_information_structure.type == 0_us && std::find(image_property_key_list.begin(), image_property_key_list.end(), property_id) != image_property_key_list.end()))
                            {
                                auto resource_property_information_structure = ResourcePropertyDetailManifestInformation{};
                                resource_property_information_structure.key_offset = set_string(property_id);
                                resource_property_information_structure.value_offset = set_string(String::to_windows_style(property_value.get<std::string>()));
                                resource_property_information_structure_list.emplace_back(resource_property_information_structure);
                            }
                        }
                        resource_detail_manifest_information_structure.property_information_count = static_cast<uint32_t>(resource_property_information_structure_list.size());
                        auto resource_basic_manifest_information = ResourceBasicManifestInformation{
                            .detail_offset = static_cast<uint32_t>(resource_manifest_information_data_stream.write_pos)};
                        resource_manifest_information_data_stream.writeNull(resource_basic_detail_section_size);
                        if (resource_detail_manifest_information_structure.type == 0_us)
                        {
                            resource_detail_manifest_information_structure.image_property_information_offset = resource_manifest_information_data_stream.write_pos;
                            exchange_from_resource_image_property(resource_manifest_information_data_stream, resource_image_property_detail_manifest_information_structure);
                        }
                        resource_detail_manifest_information_structure.property_information_offset = resource_manifest_information_data_stream.write_pos;
                        exchange_list(resource_manifest_information_data_stream, resource_property_information_structure_list, &exchange_from_resource_property);
                        auto current_pos = resource_manifest_information_data_stream.write_pos;
                        resource_manifest_information_data_stream.write_pos = resource_basic_manifest_information.detail_offset;
                        exchange_from_resource_basic(resource_manifest_information_data_stream, resource_detail_manifest_information_structure);
                        resource_manifest_information_data_stream.write_pos = current_pos;
                        subgroup_manifest_information_structure.resource_information.emplace_back(resource_basic_manifest_information);
                    }
                    group_manifest_information_structure.subgroup_information.emplace_back(subgroup_manifest_information_structure);
                }
                exchange_from_group_manifest(group_manifest_information_structure_stream, group_manifest_information_structure);
            }
            header_structure.group_manifest_information_section_offset = stream.write_pos;
            stream.writeBytes(group_manifest_information_structure_stream.toBytes());
            header_structure.resource_manifest_information_section_offset = stream.write_pos;
            stream.writeBytes(resource_manifest_information_data_stream.toBytes());
            header_structure.string_manifest_information_section_offset = stream.write_pos;
            stream.writeBytes(string_manifest_information_data_stream.toBytes());
            return;
        }

        template <typename Args>
            requires std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value || std::is_same<Args, std::string>::value
        inline static auto process_package(
            DataStreamView &stream,
            BundleStructure const &definition,
            ManifestStructure const &manifest,
            Args &args) -> void
        {
            auto index = std::find(k_version_list.begin(), k_version_list.end(), static_cast<int>(definition.version));
            assert_conditional((index != k_version_list.end()), String::format(fmt::format("{}", Language::get("popcap.rsb.invalid_rsb_version")), std::to_string(definition.version)), "process_package");
            auto block_index = std::find(k_texture_resource_information_section_block_size_list.begin(), k_texture_resource_information_section_block_size_list.end(), definition.texture_information_section_size);
            assert_conditional((block_index != k_texture_resource_information_section_block_size_list.end()), String::format(fmt::format("{}", Language::get("popcap.rsb.invalid_texture_information_section_size")), std::to_string(definition.texture_information_section_size)), "process_package"); 
            auto information_structure = Information{};
            auto global_group_index = k_begin_index;
            auto global_subgroup_index = k_begin_index;
            auto global_general_resource_index = k_begin_index;
            auto global_texture_resource_index = k_begin_index;
            auto packet_data_section_view_stored = DataStreamView{};
            for (auto &[group_id, group_information] : definition.group)
            {
                auto standard_group_id = make_standard_group_id(group_id, group_information.composite);
                information_structure.group_id[toupper_back(standard_group_id)] = static_cast<uint32_t>(global_group_index);
                auto simple_group_information_structure = SimpleGroupInformation{};
                simple_group_information_structure.id = standard_group_id;
                simple_group_information_structure.subgroup_count = static_cast<uint32_t>(group_information.subgroup.size());
                for (auto &[subgroup_id, subgroup_information] : group_information.subgroup)
                {
                    auto packet_data = std::vector<uint8_t>{};
                    if constexpr (std::is_same_v<Args, std::map<std::string, std::vector<uint8_t>>>)
                    {
                        packet_data = std::move(args.at(subgroup_id));
                    }
                    if constexpr (std::is_same_v<Args, std::string>)
                    {
                        packet_data = std::move(FileSystem::read_binary<uint8_t>(fmt::format("{}/packet/{}.rsg", args, subgroup_id)));
                    }
                    auto packet_stream = DataStreamView{packet_data};
                    auto packet_structure = PacketStructure{};
                    auto get_packet_structure_only = true;
                    ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_structure, get_packet_structure_only);
                    auto packet_header_structure = ResourceStreamGroup::Common::HeaderInformaiton{};
                    ResourceStreamGroup::Common::exchange_header(packet_stream, packet_header_structure);
                    // assert_conditional(subgroup_information.compression.general == packet_structure.compression.general, "invalid_general_compression");
                    // assert_conditional(subgroup_information.compression.texture == packet_structure.compression.texture, "invalid_texture_compression");
                    compare_conditional(packet_structure.version, definition.version, subgroup_id, "popcap.rsb.mismatch_packet_version"_sv);
                    compare_conditional(packet_structure.resource.size(), subgroup_information.resource.size(), subgroup_id, "popcap.rsb.mismatch_resource_size"_sv);
                    auto simple_subgroup_information_structure = SimpleSubgroupInformation{};
                    simple_subgroup_information_structure.index = static_cast<uint32_t>(global_subgroup_index);
                    simple_subgroup_information_structure.resolution = subgroup_information.category.resolution;
                    if (!subgroup_information.category.locale.empty())
                    {
                        fourcc_to_integer(subgroup_information.category.locale, simple_subgroup_information_structure.locale);
                    }
                    information_structure.subgroup_id[toupper_back(subgroup_id)] = static_cast<uint32_t>(global_subgroup_index);
                    auto basic_subgroup_information_structure = BasicSubgroupInformation{};
                    basic_subgroup_information_structure.id = subgroup_id;
                    basic_subgroup_information_structure.offset = packet_data_section_view_stored.write_pos;
                    basic_subgroup_information_structure.size = packet_data.size();
                    basic_subgroup_information_structure.pool = static_cast<uint32_t>(global_subgroup_index);
                    auto pool_information_structure = PoolInformation{};
                    pool_information_structure.id = fmt::format("{}{}", subgroup_id, k_suffix_of_automation_pool);
                    pool_information_structure.instance_count = 1_ui;
                    pool_information_structure.flag = 0_ui;
                    auto texture_resource_begin = global_texture_resource_index;
                    auto texture_resource_count = k_none_size;
                    auto texture_resource_index_total = k_none_size;
                    for (auto &resource_information : subgroup_information.resource)
                    {
                        auto &resource_path = resource_information.path;
                        information_structure.resource_path[String::to_windows_style(toupper_back(resource_path))] = static_cast<uint32_t>(global_subgroup_index);
                        auto packet_resource_index = k_begin_index;
                        for (packet_resource_index; packet_resource_index < packet_structure.resource.size(); ++packet_resource_index)
                        {
                            if (compare_string(resource_path, packet_structure.resource[packet_resource_index].path))
                            {
                                break;
                            }
                        }
                        assert_conditional(packet_resource_index + 1_size <= packet_structure.resource.size(), String::format(fmt::format("{}", Language::get("popcap.rsb.invalid_packet_resource_index")), std::to_string(packet_resource_index), std::to_string(packet_structure.resource.size() - 1)), "process_package");
                        auto &packet_structure_resource = packet_structure.resource[packet_resource_index];
                        compare_conditional(resource_information.use_texture_additional_instead, packet_structure_resource.use_texture_additional_instead, subgroup_id, "popcap.rsb.mismatch_texture_additional"_sv);
                        if (!resource_information.use_texture_additional_instead)
                        {
                            ++global_general_resource_index;
                        }
                        else
                        {
                            compare_conditional(packet_structure_resource.texture_additional.value.dimension.width, resource_information.texture_additional.value.dimension.width, subgroup_id, "popcap.rsb.mismatch_texture_width"_sv);
                            compare_conditional(packet_structure_resource.texture_additional.value.dimension.height, resource_information.texture_additional.value.dimension.height, subgroup_id, "popcap.rsb.mismatch_texture_height"_sv);
                            assert_conditional(definition.texture_information_section_size == resource_information.texture_additional.value.texture_resource_information_section_block_size, fmt::format("{}", Language::get("popcap.rsb.invalid_information_section_block_size")), "process_package");
                            auto texture_information_structure = TextureInfomation{};
                            texture_information_structure.size_width = resource_information.texture_additional.value.dimension.width;
                            texture_information_structure.size_height = resource_information.texture_additional.value.dimension.height;
                            texture_information_structure.pitch = resource_information.texture_additional.value.texture_infomation.pitch;
                            texture_information_structure.format = resource_information.texture_additional.value.texture_infomation.format;
                            if (definition.texture_information_section_size == k_texture_resource_information_section_block_size_version_1)
                            {
                                texture_information_structure.alpha_size = resource_information.texture_additional.value.texture_infomation.alpha_size;
                            }
                            if (definition.texture_information_section_size == k_texture_resource_information_section_block_size_version_2)
                            {
                                texture_information_structure.alpha_size = resource_information.texture_additional.value.texture_infomation.alpha_size;
                                texture_information_structure.scale = resource_information.texture_additional.value.texture_infomation.scale;
                            }
                            assert_conditional(packet_structure_resource.texture_additional.value.index >= static_cast<int>(k_none_size), String::format(fmt::format("{}", Language::get("popcap.rsb.index_must_higher_than_zero")), std::to_string(packet_structure_resource.texture_additional.value.index)), "process_package");
                            auto texture_resource_information_index = global_texture_resource_index + static_cast<size_t>(packet_structure_resource.texture_additional.value.index);
                            assert_conditional(!information_structure.texture_resource_information.contains(texture_resource_information_index), fmt::format("{}", Language::get("popcap.rsb.invalid_texture_resource_information_index")), "process_package");
                            information_structure.texture_resource_information[texture_resource_information_index] = texture_information_structure;
                            texture_resource_index_total += packet_structure_resource.texture_additional.value.index;
                            ++texture_resource_count;
                        }
                        packet_structure.resource.erase(packet_structure.resource.begin() + packet_resource_index);
                    }
                    if (texture_resource_count != k_none_size) {
                        assert_conditional(texture_resource_index_total == ((texture_resource_count - 1_size) * texture_resource_count / 2_size), String::format(fmt::format("{}", Language::get("popcap.rsb.pack.invalid_index")), subgroup_id), "process_package");
                        global_texture_resource_index += texture_resource_count;
                    }
                    assert_conditional(packet_structure.resource.size() == k_none_size, String::format(fmt::format("{}", Language::get("popcap.rsb.packet_resource_mismatch_size")), std::to_string(k_none_size), std::to_string(packet_structure.resource.size())), "process_package");
                    basic_subgroup_information_structure.resource_data_section_compression = packet_header_structure.resource_data_section_compression;
                    basic_subgroup_information_structure.information_section_size = packet_header_structure.information_section_size;
                    basic_subgroup_information_structure.general_resource_data_section_offset = packet_header_structure.general_resource_data_section_offset;
                    basic_subgroup_information_structure.general_resource_data_section_size = packet_header_structure.general_resource_data_section_size;
                    basic_subgroup_information_structure.general_resource_data_section_size_original = packet_header_structure.general_resource_data_section_size_original;
                    basic_subgroup_information_structure.general_resource_data_section_size_pool = packet_header_structure.general_resource_data_section_size_original;
                    basic_subgroup_information_structure.texture_resource_data_section_offset = packet_header_structure.texture_resource_data_section_offset;
                    basic_subgroup_information_structure.texture_resource_data_section_size = packet_header_structure.texture_resource_data_section_size;
                    basic_subgroup_information_structure.texture_resource_data_section_size_original = packet_header_structure.texture_resource_data_section_size_original;
                    basic_subgroup_information_structure.texture_resource_data_section_size_pool = static_cast<uint32_t>(k_none_size);
                    pool_information_structure.texture_resource_data_section_offset = packet_header_structure.general_resource_data_section_offset + packet_header_structure.general_resource_data_section_size_original;
                    pool_information_structure.texture_resource_data_section_size = packet_header_structure.texture_resource_data_section_size_original;
                    if (definition.version >= 3_ui)
                    {
                        basic_subgroup_information_structure.texture_resource_begin = static_cast<uint32_t>(texture_resource_begin);
                        basic_subgroup_information_structure.texture_resource_count = static_cast<uint32_t>(texture_resource_count);
                    }
                    pool_information_structure.texture_resource_begin = 0_ui;
                    pool_information_structure.texture_resource_count = 0_ui;
                    simple_group_information_structure.subgroup_information.emplace_back(simple_subgroup_information_structure);
                    information_structure.subgroup_information.emplace_back(basic_subgroup_information_structure);
                    information_structure.pool_information.emplace_back(pool_information_structure);
                    packet_data_section_view_stored.writeBytes(packet_data);
                    packet_data_section_view_stored.writeNull(compute_padding_size(packet_data_section_view_stored.write_pos, k_padding_unit_size));
                    ++global_subgroup_index;
                }
                information_structure.group_information.emplace_back(simple_group_information_structure);
                ++global_group_index;
            }
            assert_conditional(information_structure.texture_resource_information.size() == global_texture_resource_index, String::format(fmt::format("{}", Language::get("popcap.rsb.invalid_texture_count")), std::to_string(information_structure.texture_resource_information.size()), std::to_string(global_texture_resource_index)), "process_package");
            auto k_use_resource_manifest_information = definition.version <= 3_ui;
            stream.writeNull(!k_use_resource_manifest_information ? information_header_section_size - stream.write_pos : information_header_section_size - stream.write_pos - k_block_size);
            information_structure.header.resource_path_section_offset = stream.write_pos;
            CompiledMapData::encode(stream, information_structure.resource_path, &exchange_from_index);
            information_structure.header.resource_path_section_size = stream.write_pos - information_structure.header.resource_path_section_offset;
            information_structure.header.subgroup_id_section_offset = stream.write_pos;
            CompiledMapData::encode(stream, information_structure.subgroup_id, &exchange_from_index);
            information_structure.header.subgroup_id_section_size = stream.write_pos - information_structure.header.subgroup_id_section_offset;
            information_structure.header.group_information_section_offset = stream.write_pos;
            exchange_list(stream, information_structure.group_information, &exchange_from_simple_group);
            information_structure.header.group_information_section_block_size = 1156_size;
            information_structure.header.group_information_section_block_count = information_structure.group_information.size();
            information_structure.header.group_id_section_offset = stream.write_pos;
            CompiledMapData::encode(stream, information_structure.group_id, &exchange_from_index);
            information_structure.header.group_id_section_size = stream.write_pos - information_structure.header.group_id_section_offset;
            information_structure.header.subgroup_information_section_offset = stream.write_pos;
            information_structure.header.subgroup_information_section_block_size = 204_size;
            information_structure.header.subgroup_information_section_block_count = information_structure.subgroup_information.size();
            stream.writeNull(static_cast<size_t>(information_structure.header.subgroup_information_section_block_size * information_structure.header.subgroup_information_section_block_count));
            information_structure.header.pool_information_section_offset = stream.write_pos;
            exchange_list(stream, information_structure.pool_information, &exchange_from_pool);
            information_structure.header.pool_information_section_block_size = 152_size;
            information_structure.header.pool_information_section_block_count = information_structure.pool_information.size();
            information_structure.header.texture_resource_information_section_offset = stream.write_pos;
            switch (definition.texture_information_section_size)
            {
            case k_texture_resource_information_section_block_size_version_0:
            {
                for (auto &[e, texture_resource_information_value] : information_structure.texture_resource_information) {
                    exchange_from_texture<k_texture_resource_information_section_block_size_version_0>(stream, texture_resource_information_value);
                }
                break;
            }
            case k_texture_resource_information_section_block_size_version_1:
            {
                for (auto &[e, texture_resource_information_value] : information_structure.texture_resource_information) {
                    exchange_from_texture<k_texture_resource_information_section_block_size_version_1>(stream, texture_resource_information_value);
                }
                break;
            }
            case k_texture_resource_information_section_block_size_version_2:
            {
                for (auto &[e, texture_resource_information_value] : information_structure.texture_resource_information) {
                    exchange_from_texture<k_texture_resource_information_section_block_size_version_2>(stream, texture_resource_information_value);
                }
                break;
            }
            }
            information_structure.header.texture_resource_information_section_block_size = definition.texture_information_section_size;
            information_structure.header.texture_resource_information_section_block_count = information_structure.texture_resource_information.size();
            if (k_use_resource_manifest_information)
            {
                process_package_manifest(stream, information_structure.header, manifest);
            }
            stream.writeNull(compute_padding_size(stream.write_pos, k_padding_unit_size));
            information_structure.header.information_section_size = stream.write_pos;
            if (!k_use_resource_manifest_information)
            {
                information_structure.header.information_without_manifest_section_size = information_structure.header.information_section_size;
            }
            if (definition.version == 1_ui)
            {
                information_structure.header.unknown_1 = 1_ui;
            }
            if (definition.version >= 3_ui)
            {
                information_structure.header.unknown_1 = 0_ui;
            }
            for (auto &subgroup_information : information_structure.subgroup_information)
            {
                subgroup_information.offset += information_structure.header.information_section_size;
            }
            stream.write_pos = information_structure.header.subgroup_information_section_offset;
            if (definition.version == 1_ui)
            {
                exchange_list(stream, information_structure.subgroup_information, &exchange_from_basic_subgroup<1_ui>);
            }
            if (definition.version >= 3_ui)
            {
                exchange_list(stream, information_structure.subgroup_information, &exchange_from_basic_subgroup<3_ui>);
            }
            assert_conditional(stream.write_pos == information_structure.header.pool_information_section_offset, fmt::format("{}", Language::get("popcap.rsb.invalid_basic_subgroup_offset")), "process_package");
            information_structure.header.magic = Common::k_magic_identifier;
            information_structure.header.version = definition.version;
            exchange_from_header(stream, information_structure.header);
            stream.write_pos = information_structure.header.information_section_size;
            stream.writeBytes(packet_data_section_view_stored.toBytes());
            return;
        }

    public:
        template <typename Args>
            requires std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value || std::is_same<Args, std::string>::value
        inline static auto process_whole(
            DataStreamView &stream,
            BundleStructure const &definition,
            ManifestStructure const &manifest,
            Args &args) -> void
        {
            process_package(stream, definition, manifest, args);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            BundleStructure definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            auto manifest = ManifestStructure{};
            if (definition.version <= 3_ui) {
                manifest = *FileSystem::read_json(fmt::format("{}/resource.json", source));
            }
            auto source_path = get_string(source);
            process_whole(stream, definition, manifest, source_path);
            stream.out_file(destination);
            return;
        }
    };
}