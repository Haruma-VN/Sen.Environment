#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_bundle/definition.hpp"
#include "kernel/support/miscellaneous/custom/stream_compressed_group/common.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Custom::ResourceStreamBundle
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle;

    using PacketOriginalInformation = Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::PacketOriginalInformation;

    using CustomResourceInformation = Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::CustomResourceInformation;

    struct Unpack : Common
    {
    private:
        inline static auto exchange_manifest_group(
            CustomResourceInformation &resource_info,
            DataSectionViewStored &packet_data_section_view_stored,
            ManifestGroupInfo &manifest_info, 
            std::string_view destination) -> void
        {
            auto manifest_string = "__MANIFESTGROUP__"_sv;
            for (auto &[packet_id, packet_data] : packet_data_section_view_stored)
            {
                if (packet_id.starts_with(manifest_string))
                {
                    manifest_info.resource_additional_name = packet_id.substr(manifest_string.size(), packet_id.size() - manifest_string.size());
                    auto convertion = [&](nlohmann::ordered_json const &data, bool newtype) -> void
                    {
                       // resource_info.expand_path = Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::get_expand_path(data);
                        auto res_temp = data;
                        if (newtype) {
                            Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_custom_resource_info<true>(res_temp, resource_info);
                           // manifest_info.expand_path = ExpandPath::String;
                        }
                        else {
                            Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_custom_resource_info<false>(res_temp, resource_info);
                           // manifest_info.expand_path = ExpandPath::Array;
                        }
                        auto group_size_left = k_none_size;
                        for (auto &group : res_temp["groups"]) {
                            write_json(fmt::format("{}/unuse_resource/{}.json", destination, group["id"].get<std::string>()), group);
                            ++group_size_left;
                        }
                        try_assert(res_temp["groups"].size() == group_size_left, "group_must_empty");
                    };
                    auto resource_data_section_view_stored = DataSectionViewStored{};
                    unpack_rsg_without_definition(packet_data, resource_data_section_view_stored);
                    assert_conditional(resource_data_section_view_stored.size() != k_none_size, fmt::format("{}", Language::get("popcap.rsb.custom.manifest_is_empty")), "exchange_manifest_group");
                    for (auto &[resource_id, resource_data] : resource_data_section_view_stored)
                    {
                        auto extenstion = Kernel::Path::getExtension(resource_id);
                        if (compare_string(extenstion, k_newton_extension_string))
                        {
                            manifest_info.allow_new_type_resource = true;
                            auto decode = Sen::Kernel::Support::PopCap::NewTypeObjectNotation::Decode(resource_data);
                            convertion(decode.process(), true);
                            break;
                        }
                        else if (compare_string(extenstion, k_rton_extension_string))
                        {
                            auto stream = DataStreamView{resource_data};
                            auto writer = JsonWriter{};
                            Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Decode::process_whole(stream, writer);
                            convertion(nlohmann::ordered_json::parse(writer.ToString()), false);
                            break;
                        }
                        else
                        {
                            assert_conditional(false, fmt::format("{}", Language::get("popcap.rsb.custom.invalid_resource_file")), "exchange_manifest_group");
                        }
                    }
                    packet_data_section_view_stored.erase(packet_id);
                    return;
                }
            }
            assert_conditional(false, String::format(fmt::format("{}", Language::get("popcap.rsb.custom.cannot_find_manifest"))), "exchange_manifest_group");
        }

        inline static auto exchange_package(
            DataSectionViewStored &packet_data_section_view_stored,
            CustomResourceInformation &resource_info,
            std::string_view destination) -> uint32_t
        {
            auto check_rton_is_encrypted = [](std::vector<uint8_t> &data) -> bool
            {
                if (data.size() == k_none_size)
                {
                    return false;
                }
                if (data[0] == 0x10 && data[1] == 0x0)
                {
                    return true;
                }
                return false;
            };
            auto rton_encrypted_count = static_cast<int>(k_begin_index);
            auto packages_string = "Packages"_sv;
            for (auto &[packet_id, packet_data] : packet_data_section_view_stored)
            {
                if (compare_string(packet_id, packages_string))
                {
                    auto resource_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
                    unpack_rsg_without_definition(packet_data, resource_data_section_view_stored);
                    FileSystem::create_directory(tolower_back(fmt::format("{}/{}", destination, packages_string)));
                    auto async_work_process = std::vector<std::future<void>>{};
                    for (auto &[id, data] : resource_data_section_view_stored)
                    {
                        check_rton_is_encrypted(data) ? ++rton_encrypted_count : --rton_encrypted_count;
                        async_work_process.emplace_back(std::async(&write_bytes, fmt::format("{}/{}", destination, id), data));
                    }
                    auto package_info = PackagesInfoFlag{
                        .automatically_converter = false,
                        .encrypted = rton_encrypted_count > static_cast<int>(resource_data_section_view_stored.size() / 2_size)};
                    auto package_info_flag = 0_ui;
                    exchange_packages_info_flag(package_info, package_info_flag);
                    async_process_list<void>(async_work_process);
                    packet_data_section_view_stored.erase(packet_id);
                    resource_info.group.erase(get_string(packages_string));
                    return package_info_flag + 1_ui;
                }
            }
            return 0_ui;
        }

        inline static auto exchange_stream(
            PacketOriginalInformation const &packet_original_information,
            std::string const &destination) -> void
        {
            auto stream = DataStreamView{};
            Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_stream_resource_group(stream, packet_original_information);
            stream.out_file(destination);
            return;
        }

        inline static auto try_find_subgroup_information(
            std::string const &group_id, 
            std::string const &subgroup_id,
            BundleStructure &bundle,
            Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation &subgroup_information
        ) -> bool
        {
            try 
            {
                subgroup_information = bundle.group.at(toupper_back(group_id)).subgroup.at(toupper_back(subgroup_id));
                return true;
            }
            catch (std::out_of_range& e) {
                for (auto &[group_id, group_value] : bundle.group) {
                    for (auto &[id, subgroup_value] : group_value.subgroup) {
                        if (compare_string(subgroup_id, id)) {
                            subgroup_information = subgroup_value;
                            return true;
                        }
                    }
                }
                return false;
            }
        }

        inline static auto exchange_packet(
            InformationStructure &definition,
            BundleStructure &bundle,
            CustomResourceInformation &resource_info,
            DataSectionViewStored &packet_data_section_view_stored,
            std::string_view destination,
            Setting const &setting) -> void
        {
            auto async_work_process = std::vector<std::future<void>>{};
            for (auto &[group_id, group_value] : resource_info.group)
            {
                auto packet_original_information = PacketOriginalInformation{
                    .version = definition.version,
                    .composite = group_value.composite,
                    .texture_format_category = setting.texture_format_category};
                for (auto &[subgroup_id, subgroup_value] : group_value.subgroup)
                {
                    if (setting.only_high_resolution && subgroup_value.texture.resolution != static_cast<int>(k_none_size))
                    {
                        if (std::find(k_high_resolution_list.begin(), k_high_resolution_list.end(), subgroup_value.texture.resolution) == k_high_resolution_list.end())
                        {
                            continue;
                        }
                    }
                    auto subgroup_information = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation{};
                    auto contain = try_find_subgroup_information(group_id, subgroup_id, bundle, subgroup_information);
                    if (!packet_data_section_view_stored.contains(toupper_back(subgroup_id)) || !contain)
                    {
                        auto unuse_resource = nlohmann::ordered_json{};
                        Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_subgroup_compression_info(subgroup_value, unuse_resource);
                        write_json(fmt::format("{}/unuse_resource/{}.json", destination, subgroup_id), unuse_resource);
                        continue;
                    }
                    auto &subgroup_raw_content = packet_original_information.subgroup[subgroup_id];
                    subgroup_raw_content.is_image = subgroup_value.texture.resolution != static_cast<int>(k_none_size);
                    subgroup_raw_content.packet_data = std::move(packet_data_section_view_stored.at(toupper_back(subgroup_id)));
                    subgroup_raw_content.info = subgroup_value;
                    if (subgroup_raw_content.is_image)
                    {
                        subgroup_raw_content.info.texture.locale = subgroup_information.category.locale;
                        for (auto &[packet_id, packet_value] : subgroup_raw_content.info.texture.packet)
                        {
                            auto process_done = false;
                            for (auto &resource : subgroup_information.resource)
                            {
                                if (compare_string(resource.path, fmt::format("{}.ptx", packet_value.path)))
                                {
                                    packet_value.additional.index = resource.texture_additional.value.index;
                                    packet_value.additional.format = static_cast<int>(resource.texture_additional.value.texture_infomation.format);
                                    packet_value.additional.pitch = static_cast<int>(resource.texture_additional.value.texture_infomation.pitch);
                                    packet_value.additional.alpha_size = static_cast<int>(resource.texture_additional.value.texture_infomation.alpha_size);
                                    packet_value.additional.scale = static_cast<int>(resource.texture_additional.value.texture_infomation.scale);
                                    packet_value.dimension.width = static_cast<int>(resource.texture_additional.value.dimension.width);
                                    packet_value.dimension.height = static_cast<int>(resource.texture_additional.value.dimension.height);
                                    process_done = true;
                                    break;
                                }
                            }
                            assert_conditional(process_done, fmt::format("{}: {}", Language::get("popcap.rsb.custom.cannot_find_image_in_packet"), packet_value.path), "exchange_packet");
                        }
                    }
                    else {
                        //subgroup_raw_content.info.general.locale = subgroup_information.category.locale;
                        try_assert(subgroup_raw_content.info.general.locale == subgroup_information.category.locale, fmt::format("mismatch_locale_at_{}", subgroup_id));
                    }
                    packet_data_section_view_stored.erase(toupper_back(subgroup_id));
                }
                if (packet_original_information.subgroup.size() != k_none_size)
                {
                    definition.packet.emplace_back(group_id);
                    async_work_process.emplace_back(std::async(&exchange_stream, packet_original_information, fmt::format("{}/packet/{}.scg", destination, group_id)));
                }
            }
            for (auto &[packet_id, packet_data] : packet_data_section_view_stored) {
                async_work_process.emplace_back(std::async(&write_bytes, fmt::format("{}/unuse_packet/{}.scg", destination, packet_id), packet_data));
            }
            async_process_list<void>(async_work_process);
            return;
        }

        inline static auto exchange_bundle(
            DataStreamView &stream,
            InformationStructure &definition,
            std::string_view destination,
            Setting const &setting) -> void
        {
            auto bundle = BundleStructure{};
            auto manifest = ManifestStructure{};
            auto packet_data_section_view_stored = DataSectionViewStored{};
            Sen::Kernel::Support::PopCap::ResourceStreamBundle::Unpack::process_whole<true>(stream, bundle, manifest, packet_data_section_view_stored);
            assert_conditional(bundle.version == 4_ui, fmt::format("{}", Language::get("popcap.rsb.custom.version_is_not_support")), "exchange_bundle");
            definition.version = bundle.version;
            definition.texture_information_version = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::exchange_texture_information_version(bundle.texture_information_section_size);
            auto resource_info = CustomResourceInformation{};
            exchange_manifest_group(resource_info, packet_data_section_view_stored, definition.manifest_info, destination);
            definition.packages_info_flag = exchange_package(packet_data_section_view_stored, resource_info, destination);
            exchange_packet(definition, bundle, resource_info, packet_data_section_view_stored, destination, setting);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            InformationStructure &definition,
            std::string_view destination,
            Setting const &setting) -> void
        {
            exchange_bundle(stream, definition, destination, setting);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination,
            Setting const &setting) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = InformationStructure{};
            process_whole(stream, definition, destination, setting);
            write_json(fmt::format("{}/data.json", destination), definition);
            return;
        }
    };
}