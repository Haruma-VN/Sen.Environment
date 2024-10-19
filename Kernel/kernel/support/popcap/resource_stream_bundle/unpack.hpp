#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/definition.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle
{
    using namespace Definition;

   // using DataSectionViewStored = std::map<std::string, std::vector<uint8_t>, decltype(&case_insensitive_compare)>;

    struct Unpack : Common
    {
    public:
        inline static auto make_original_group_id(
            std::string const &standard_id,
            bool &is_composite) -> std::string
        {
            is_composite = !standard_id.ends_with(k_suffix_of_composite_shell);
            if (is_composite)
            {
                return standard_id;
            }
            else
            {
                return standard_id.substr(k_begin_index, standard_id.size() - k_suffix_of_composite_shell.size());
            }
        }

        inline static auto process_package_manifest(
            DataStreamView &stream,
            HeaderInformaiton const &header_structure,
            ManifestStructure &manifest) -> void
        {
            manifest.manifest_has = true;
            auto group_manifest_information_data_stream = DataStreamView{stream.getBytes(static_cast<size_t>(header_structure.group_manifest_information_section_offset), static_cast<size_t>(header_structure.resource_manifest_information_section_offset))};
            auto resource_manifest_information_data_stream = DataStreamView{stream.getBytes(static_cast<size_t>(header_structure.resource_manifest_information_section_offset), static_cast<size_t>(header_structure.string_manifest_information_section_offset))};
            auto string_manifest_information_data_stream = DataStreamView{stream.getBytes(static_cast<size_t>(header_structure.string_manifest_information_section_offset), static_cast<size_t>(header_structure.information_section_size))};
            auto get_string = [&](
                                  uint32_t const &offset) -> std::string
            {
                string_manifest_information_data_stream.read_pos = static_cast<size_t>(offset);
                return string_manifest_information_data_stream.readStringByEmpty();
            };
            while (group_manifest_information_data_stream.read_pos < group_manifest_information_data_stream.size())
            {
                auto manifest_group_information = ManifestGroupInformation{};
                auto group_manifest_information_structure = GroupManifestInformation{};
                exchange_to_group_manifest(group_manifest_information_data_stream, group_manifest_information_structure);
                auto original_id = make_original_group_id(get_string(group_manifest_information_structure.id_offset), manifest_group_information.composite);
                for (auto &subgroup_manifest_information_structure : group_manifest_information_structure.subgroup_information)
                {
                    auto subgroup_manifest = ManifestSubgroupInformation{};
                    auto subgroup_id = get_string(subgroup_manifest_information_structure.id_offset);
                    subgroup_manifest.category.resolution = subgroup_manifest_information_structure.resolution;
                    if (subgroup_manifest_information_structure.locale != 0_ui)
                    {
                        fourcc_from_integer(subgroup_manifest_information_structure.locale, subgroup_manifest.category.locale);
                    }
                    for (auto resource_manifest_information_structure : subgroup_manifest_information_structure.resource_information)
                    {
                        resource_manifest_information_data_stream.read_pos = static_cast<size_t>(resource_manifest_information_structure.detail_offset);
                        auto resource_detail_manifest_information_structure = ResourceBasicDetailManifestInformation{};
                        exchange_to_resource_basic(resource_manifest_information_data_stream, resource_detail_manifest_information_structure);
                        auto manifest_resource = ManifestResource{};
                        auto manifest_resource_id = get_string(resource_detail_manifest_information_structure.id_offset);
                        manifest_resource.path = String::to_posix_style(get_string(resource_detail_manifest_information_structure.path_offset));
                        manifest_resource.type = resource_detail_manifest_information_structure.type;
                        if (resource_detail_manifest_information_structure.image_property_information_offset != 0_ui)
                        {
                            resource_manifest_information_data_stream.read_pos = static_cast<size_t>(resource_detail_manifest_information_structure.image_property_information_offset);
                            auto resource_image_property_detail_manifest_information_structure = ResourceImagePropertyDetailManifestInformation{};
                            exchange_to_resource_image_property(resource_manifest_information_data_stream, resource_image_property_detail_manifest_information_structure);
                            manifest_resource.property["type"_sv] = resource_image_property_detail_manifest_information_structure.type;
                            manifest_resource.property["flag"_sv] = resource_image_property_detail_manifest_information_structure.flag;
                            manifest_resource.property["x"_sv] = resource_image_property_detail_manifest_information_structure.x;
                            manifest_resource.property["y"_sv] = resource_image_property_detail_manifest_information_structure.y;
                            manifest_resource.property["ax"_sv] = resource_image_property_detail_manifest_information_structure.ax;
                            manifest_resource.property["ay"_sv] = resource_image_property_detail_manifest_information_structure.ay;
                            manifest_resource.property["aw"_sv] = resource_image_property_detail_manifest_information_structure.aw;
                            manifest_resource.property["ah"_sv] = resource_image_property_detail_manifest_information_structure.ah;
                            manifest_resource.property["rows"_sv] = resource_image_property_detail_manifest_information_structure.rows;
                            manifest_resource.property["cols"_sv] = resource_image_property_detail_manifest_information_structure.cols;
                            manifest_resource.property["parent"_sv] = get_string(resource_image_property_detail_manifest_information_structure.parent_offset);
                        }
                        resource_manifest_information_data_stream.read_pos = resource_detail_manifest_information_structure.property_information_offset;
                        for (auto resource_property_index : Range(resource_detail_manifest_information_structure.property_information_count))
                        {
                            auto resource_property_detail_information_manifest_structure = ResourcePropertyDetailManifestInformation{};
                            exchange_to_resource_property(resource_manifest_information_data_stream, resource_property_detail_information_manifest_structure);
                            manifest_resource.property[get_string(resource_property_detail_information_manifest_structure.key_offset)] = String::to_posix_style(get_string(resource_property_detail_information_manifest_structure.value_offset));
                        }
                        subgroup_manifest.resource[manifest_resource_id] = manifest_resource;
                    }
                    manifest_group_information.subgroup[subgroup_id] = subgroup_manifest;
                }
                manifest.group[original_id] = manifest_group_information;
            }
            return;
        }

    protected:
        template <auto unpack_for_work, typename Args>
           requires std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value || std::is_same<Args, std::string_view>::value
        inline static auto process_package(
            DataStreamView &stream,
            BundleStructure &definition,
            ManifestStructure &manifest,
            Args &args) -> void
        {
            static_assert(unpack_for_work == true || unpack_for_work == false, "unpack_for_work must be true or false");
            auto information_structure = Information{};
            exchange_to_header(stream, information_structure.header);
            assert_conditional(information_structure.header.magic == k_magic_identifier, String::format(fmt::format("{}", Language::get("popcap.rsb.unpack.invalid_rsb_magic")), std::to_string(definition.version)), "process_package");
            auto index = std::find(k_version_list.begin(), k_version_list.end(), static_cast<int>(information_structure.header.version));
            assert_conditional((index != k_version_list.end()), String::format(fmt::format("{}", Language::get("popcap.rsb.invalid_rsb_version")), std::to_string(static_cast<int>(information_structure.header.version))), "process"); 
            definition.version = information_structure.header.version;
            CompiledMapData::decode(stream, information_structure.header.group_id_section_offset, information_structure.header.group_id_section_size, information_structure.group_id, &exchange_to_index);
            // CompiledMapData::decode(stream, information_structure.header.subgroup_id_section_offset, information_structure.header.subgroup_id_section_size, information_structure.subgroup_id, &exchange_to_index);
            // CompiledMapData::decode(stream, information_structure.header.resource_path_section_offset, information_structure.header.resource_path_section_size, information_structure.resource_path, &exchange_to_index);
            stream.read_pos = information_structure.header.group_information_section_offset;
            exchange_list(stream, information_structure.group_information, &exchange_to_simple_group, static_cast<size_t>(information_structure.header.group_information_section_block_count));
            stream.read_pos = information_structure.header.subgroup_information_section_offset;
            if (definition.version == 1_ui)
            {
                assert_conditional(definition.version == 1_ui && information_structure.header.unknown_1 == 1_ui, fmt::format("{}", Language::get("popcap.rsb.invalid_header_block")), "process_package");
                exchange_list(stream, information_structure.subgroup_information, &exchange_to_basic_subgroup<1_ui>, static_cast<size_t>(information_structure.header.subgroup_information_section_block_count));
            }
            else
            {
                assert_conditional(definition.version >= 3_ui && information_structure.header.unknown_1 == 0_ui, fmt::format("{}", Language::get("popcap.rsb.invalid_header_block")), "process_package");
                exchange_list(stream, information_structure.subgroup_information, &exchange_to_basic_subgroup<3_ui>, static_cast<size_t>(information_structure.header.subgroup_information_section_block_count));
            }
            stream.read_pos = information_structure.header.pool_information_section_offset;
            exchange_list(stream, information_structure.pool_information, &exchange_to_pool, static_cast<size_t>(information_structure.header.pool_information_section_block_count));
            stream.read_pos = information_structure.header.texture_resource_information_section_offset;
            auto &texture_resource_information_section_block_size = information_structure.header.texture_resource_information_section_block_size;
            switch (texture_resource_information_section_block_size)
            {
            case k_texture_resource_information_section_block_size_version_0:
            {
                for (auto index : Range(information_structure.header.texture_resource_information_section_block_count)) {
                    exchange_to_texture<k_texture_resource_information_section_block_size_version_0>(stream, information_structure.texture_resource_information[index]);
                }
                break;
            }
            case k_texture_resource_information_section_block_size_version_1:
            {
                for (auto index : Range(information_structure.header.texture_resource_information_section_block_count)) {
                    exchange_to_texture<k_texture_resource_information_section_block_size_version_1>(stream, information_structure.texture_resource_information[index]);
                }
                break;
            }
            case k_texture_resource_information_section_block_size_version_2:
            {
                for (auto index : Range(information_structure.header.texture_resource_information_section_block_count)) {
                    exchange_to_texture<k_texture_resource_information_section_block_size_version_2>(stream, information_structure.texture_resource_information[index]);
                }
                break;
            }
            default:
                assert_conditional(false, fmt::format("{}", Language::get("popcap.rsb.invalid_texture_resource_information_section_block_size")), "process_package"); 
            }
            if (information_structure.header.group_manifest_information_section_offset != 0_ui || information_structure.header.resource_manifest_information_section_offset != 0_ui || information_structure.header.string_manifest_information_section_offset != 0_ui)
            {
                assert_conditional(information_structure.header.group_manifest_information_section_offset != 0_ui && information_structure.header.resource_manifest_information_section_offset != 0_ui && information_structure.header.string_manifest_information_section_offset != 0_ui, fmt::format("{}", Language::get("popcap.rsb.invalid_manifest_block_section_offset")), "process_package");
                process_package_manifest(stream, information_structure.header, manifest);
            }
            definition.texture_information_section_size = texture_resource_information_section_block_size;
            #if WINDOWS 
            auto async_work_process = std::vector<std::future<void>>{};
            #endif
            for (auto &[group_id, group_index] : information_structure.group_id)
            {
                auto &simple_group_information = information_structure.group_information.at(group_index);
                auto group_information = GroupInformation{};
                auto original_id = make_original_group_id(simple_group_information.id, group_information.composite);
                if constexpr (unpack_for_work) {
                    toupper_case(original_id);
                }
                for (auto subgroup_index : Range(simple_group_information.subgroup_count))
                {
                    auto &simple_subgroup_infomation = simple_group_information.subgroup_information.at(subgroup_index);
                    auto &basic_subgroup_information = information_structure.subgroup_information.at(simple_subgroup_infomation.index);
                    auto &pool_information = information_structure.pool_information.at(simple_subgroup_infomation.index);
                    auto &subgroup_id = basic_subgroup_information.id;
                    if constexpr (unpack_for_work) {
                        toupper_case(subgroup_id);
                    }
                    auto subgroup_information = SubgroupInformation{};
                    subgroup_information.category.resolution = simple_subgroup_infomation.resolution;
                    if (definition.version >= 3 && simple_subgroup_infomation.locale != 0_ui)
                    {
                        fourcc_from_integer(simple_subgroup_infomation.locale, subgroup_information.category.locale);
                    }
                    subgroup_information.compression = basic_subgroup_information.resource_data_section_compression;
                    // packet_compression_from_data(basic_subgroup_information.resource_data_section_compression, subgroup_information.compression);
                    auto texture_resource_begin = basic_subgroup_information.texture_resource_begin;
                    auto texture_resource_count = basic_subgroup_information.texture_resource_count;
                    assert_conditional(pool_information.texture_resource_begin == 0_ui, fmt::format("{}", Language::get("popcap.rsb.invalid_texture_resource")), "process_package");
                    assert_conditional(pool_information.texture_resource_count == 0_ui, fmt::format("{}", Language::get("popcap.rsb.invalid_texture_resource")), "process_package");
                    auto packet_data = stream.getBytes(basic_subgroup_information.offset, basic_subgroup_information.offset + basic_subgroup_information.size);
                    auto packet_stream = DataStreamView{packet_data};
                    auto packet_structure = PacketStructure{};
                    auto get_packet_structure_only = true;
                    ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_structure, get_packet_structure_only);
                    // assert_conditional(subgroup_information.compression.general == packet_structure.compression.general, "invalid_general_compression");
                    // assert_conditional(subgroup_information.compression.texture == packet_structure.compression.texture, "invalid_texture_compression");
                    for (auto &packet_resource : packet_structure.resource)
                    {
                        if (packet_resource.use_texture_additional_instead)
                        {
                            auto &texture_information_structure = information_structure.texture_resource_information[static_cast<size_t>(texture_resource_begin + packet_resource.texture_additional.value.index)];
                            packet_resource.texture_additional.value.texture_resource_information_section_block_size = texture_resource_information_section_block_size;
                            compare_conditional(packet_resource.texture_additional.value.dimension.width, texture_information_structure.size_width, original_id, "popcap.rsb.mismatch_texture_width"_sv);
                            compare_conditional(packet_resource.texture_additional.value.dimension.height, texture_information_structure.size_height, original_id, "popcap.rsb.mismatch_texture_height"_sv);
                            packet_resource.texture_additional.value.texture_infomation = TextureResourceInformation{
                                .pitch = texture_information_structure.pitch,
                                .format = texture_information_structure.format,
                                .alpha_size = texture_information_structure.alpha_size,
                                .scale = texture_information_structure.scale};
                            subgroup_information.category.is_image = true;
                        }
                    }
                    subgroup_information.resource = packet_structure.resource;
                    group_information.subgroup[subgroup_id] = subgroup_information;
                    if constexpr (std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value)
                    {
                        args[subgroup_id] = std::move(packet_data);
                    }
                    if constexpr (std::is_same<Args, std::string_view>::value)
                    {
                        #if WINDOWS 
                        async_work_process.emplace_back(std::async(&write_bytes, fmt::format("{}/packet/{}.rsg", args, subgroup_id), packet_data));
                        #else
                        write_bytes(fmt::format("{}/packet/{}.rsg", args, subgroup_id), packet_data);
                        #endif
                    }
                }
                definition.group[original_id] = group_information;
            }
            #if WINDOWS 
            async_process_list<void>(async_work_process);
            #endif
            return;
        }

    public:
        template <auto unpack_for_work = false, typename Args>
            requires std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value || std::is_same<Args, std::string_view>::value
        inline static auto process_whole(
            DataStreamView &stream,
            BundleStructure &definition,
            ManifestStructure &manifest,
            Args &args) -> void
        {
            static_assert(unpack_for_work == true || unpack_for_work == false, "unpack_for_work must be true or false");
            process_package<unpack_for_work>(stream, definition, manifest, args);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = BundleStructure{};
            auto manifest = ManifestStructure{};
            process_whole(stream, definition, manifest, destination);
            write_json(fmt::format("{}/data.json", destination), definition);
            if (manifest.manifest_has)
            {
                write_json(fmt::format("{}/resource.json", destination), manifest);
            }
            return;
        }
    };
}