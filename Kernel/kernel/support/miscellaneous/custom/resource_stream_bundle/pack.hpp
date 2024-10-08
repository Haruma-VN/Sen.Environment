#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_bundle/definition.hpp"
#include "kernel/support/miscellaneous/custom/stream_compressed_group/common.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Custom::ResourceStreamBundle
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle;

    using CustomResourceInformation = Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::CustomResourceInformation;

    using SubgroupCompressedInfo = Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::SubgroupCompressedInfo;

    using PacketOriginalInformation = Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::PacketOriginalInformation;

    struct Pack : Common
    {
    private:
        inline static auto checking_info(
            InformationStructure const &definition) -> void
        {
            auto duplicate_packet = std::vector<string>{};
            find_duplicate_elements<string>(definition.packet, duplicate_packet);
            assert_conditional(duplicate_packet.empty(), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.duplicated_packet")), duplicate_packet[0]), "checking_info");
            return;
        }

        inline static auto exchange_packages(
            DataSectionViewStored &packet_data_section_view_stored,
            CustomResourceInformation &resource_info,
            BundleStructure &bundle,
            PackagesInfoFlag const &package_info,
            std::string const &packages_source,
            Setting const &setting) -> void
        {
            auto &packages_setting = setting.packages_setting;
            auto packages_name = "Packages"_sv;
            auto packages_string = get_string(packages_name);
            auto get_resfile = [](
                                   std::string const &path) -> std::string
            {
                auto str = Sen::Kernel::String(path);
                auto resfile = fmt::format("RESFILE_{}", String::join(str.split("\\"), "_"));
                return resfile.substr(k_begin_index, resfile.size() - ".rton"_sv.size());
            };
            auto packages_list = FileSystem::read_whole_directory(packages_source);
            auto resource_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            auto packet_definition = PacketStructure{
                .version = bundle.version,
                .compression = Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::PacketCompression{
                    .general = setting.compression_setting.packages, // packages will not compress, maybe crash in old version and chinese.
                    .texture = true}};
            auto &packages_subgroup_info = resource_info.group[packages_string].subgroup[packages_string];
            auto json_count = static_cast<size_t>(packages_setting.json_count);
            auto rton_count = static_cast<size_t>(packages_setting.rton_count);
            auto push_definition = [&](std::string const &path) -> void
            {
                packet_definition.resource.emplace_back(Resource{
                    .path = toupper_back(String::to_posix_style(path))});
                auto &data_info = packages_subgroup_info.general.data[get_resfile(path)];
                data_info.type = Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::DataType::File;
                data_info.path = tolower_back(String::to_posix_style(path));
                return;
            };
            auto read_rton = [&](
                                 std::string const &element) -> void
            {
                auto pos = packages_source.size() - packages_name.size();
                auto path = toupper_back(String::to_windows_style(element.substr(pos, (element.size() - pos))));
                if (!resource_data_section_view_stored.contains(path))
                {
                    resource_data_section_view_stored[path] = std::move(FileSystem::read_binary<uint8_t>(element));
                    push_definition(path);
                    --rton_count;
                }
                return;
            };
            if (package_info.automatically_converter)
            {
                for (auto element : packages_list)
                {
                    auto extension = Path::getExtension(element);
                    if (compare_string(extension, ".json"_sv))
                    {
                        auto pos = packages_source.size() - packages_name.size();
                        auto path = toupper_back(String::to_windows_style(element.substr(pos, (element.size() - pos - ".json"_sv.size())) + ".rton"));
                        auto resource_stream = DataStreamView{};
                        Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Encode::process_whole(resource_stream, FileSystem::read_file(element));
                        if (package_info.encrypted)
                        {
                            auto encrypted_stream = DataStreamView{};
                            Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Instance::encrypt(resource_stream, encrypted_stream, packages_setting.key, packages_setting.iv);
                            resource_data_section_view_stored[path] = std::move(encrypted_stream.toBytes());
                        }
                        else
                        {
                            resource_data_section_view_stored[path] = std::move(resource_stream.toBytes());
                        }
                        push_definition(path);
                        --json_count;
                        --rton_count;
                    }
                    if (compare_string(extension, ".rton"_sv))
                    {
                        read_rton(element);
                    }
                }
            }
            else
            {
                for (auto & element : packages_list)
                {
                    if (compare_string(Path::getExtension(element), ".rton"_sv))
                    {
                        read_rton(element);
                    }
                }
            }
            assert_conditional(json_count == k_none_size, fmt::format("{}", Language::get("popcap.rsb.project.json_count_mismatch")), "exchange_packages");
            assert_conditional(rton_count == k_none_size, fmt::format("{}", Language::get("popcap.rsb.project.rton_count_mismatch")), "exchange_packages");
            auto packet_stream = DataStreamView{};
            Sen::Kernel::Support::PopCap::ResourceStreamGroup::Pack::process_whole(packet_stream, packet_definition, resource_data_section_view_stored);
            packet_data_section_view_stored[packages_string] = std::move(packet_stream.toBytes());
            bundle.group[packages_string].subgroup[packages_string] = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation{
                .compression = k_highest_compression_method,
                .resource = packet_definition.resource};

            return;
        }

        inline static auto exchange_packet(
            CustomResourceInformation &resource_info,
            BundleStructure &bundle,
            DataSectionViewStored &packet_data_section_view_stored,
            InformationStructure const &definition,
            std::string const &packet_source,
            Setting const &setting) -> void
        {
            for (auto &group_id : definition.packet)
            {
                auto group_stream = DataStreamView{fmt::format("{}/{}.scg", packet_source, group_id)};
                auto packet_original_information = PacketOriginalInformation{};
                try {
                    Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_stream_resource_group(packet_original_information, group_stream);
                }
                catch (Exception &ex) {
                    assert_conditional(false, fmt::format("{} at {}", ex.what(), group_id), "exchange_packet");
                }
                Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::compare_conditional(packet_original_information.version, definition.version, group_id, "popcap.rsb.custom.mismatch_scg_version");
                Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::compare_conditional(packet_original_information.texture_format_category, setting.texture_format_category, group_id, "popcap.rsb.custom.mismatch_texture_format_category");
                auto &group_information = bundle.group[group_id];
                group_information.composite = packet_original_information.composite;
                resource_info.group[group_id].composite = packet_original_information.composite;
                assert_conditional(packet_original_information.subgroup.size() != k_none_size, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.group_must_contain_at_least_one")), group_id), "exchange_packet");
                for (auto &[subgroup_id, subgroup_value] : packet_original_information.subgroup)
                {
                    auto &subgroup_information = group_information.subgroup[subgroup_id] = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation{
                        .category = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SimpleCategoryInformation{
                            .resolution = subgroup_value.info.texture.resolution}};
                    auto packet_stream = DataStreamView{subgroup_value.packet_data};
                    auto packet_structure = PacketStructure{};
                    auto get_packet_structure_only = true;
                    Sen::Kernel::Support::PopCap::ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_structure, get_packet_structure_only);
                    Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(subgroup_information.compression, packet_structure.compression);
                    // subgroup_information.compression = subgroup_value.compression_flag;
                    resource_info.group[group_id].subgroup[subgroup_id] = subgroup_value.info;
                    packet_data_section_view_stored[subgroup_id] = subgroup_value.packet_data;
                    if (subgroup_value.is_image)
                    {
                        subgroup_information.category.locale = subgroup_value.info.texture.locale;
                        for (auto &resource : packet_structure.resource)
                        {
                            if (resource.use_texture_additional_instead)
                            {
                                for (auto &[packet_id, packet_value] : subgroup_value.info.texture.packet)
                                {
                                    if (!Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::is_program_path(packet_value.path))
                                    {
                                        if (compare_string(resource.path, fmt::format("{}.ptx", packet_value.path)))
                                        {
                                            resource.texture_additional.value.texture_resource_information_section_block_size = bundle.texture_information_section_size;
                                            //resource.texture_additional.value.dimension.width = static_cast<uint32_t>(packet_value.dimension.width);
                                            //resource.texture_additional.value.dimension.height = static_cast<uint32_t>(packet_value.dimension.height);
                                            resource.texture_additional.value.texture_infomation.format = static_cast<uint32_t>(packet_value.additional.format);
                                            resource.texture_additional.value.texture_infomation.pitch = static_cast<uint32_t>(packet_value.additional.pitch);
                                            resource.texture_additional.value.texture_infomation.alpha_size = static_cast<uint32_t>(packet_value.additional.alpha_size);
                                            resource.texture_additional.value.texture_infomation.scale = static_cast<uint32_t>(packet_value.additional.scale);
                                            subgroup_information.resource.emplace_back(resource);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        subgroup_information.category.locale = subgroup_value.info.general.locale;
                        for (auto &resource : packet_structure.resource)
                        {
                            if (!resource.use_texture_additional_instead)
                            {
                                subgroup_information.resource.emplace_back(resource);
                            }
                        }
                    }
                }
            }
            return;
        }

        inline static auto exchange_manifest_group(
            BundleStructure &bundle,
            DataSectionViewStored &packet_data_section_view_stored,
            CustomResourceInformation const &resource_info,
            ManifestGroupInfo const &manifest_info,
            Setting const &setting) -> void
        {
            auto packet_definition = PacketStructure{
                .version = bundle.version,
                .compression = Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::PacketCompression{
                    .general = setting.compression_setting.manifest, // manifest wil not compress, maybe crash in old version and chinese.
                    .texture = true}};
            auto result = nlohmann::ordered_json{};
            if (manifest_info.allow_new_type_resource) {
                
                Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_custom_resource_info<true>(resource_info, result);
            }
            else {
                Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_custom_resource_info<false>(resource_info, result);
            }
            auto resources_name = toupper_back(fmt::format("resources{}", manifest_info.resource_additional_name));
            auto resource_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            if (manifest_info.allow_new_type_resource)
            {
                auto encode = Sen::Kernel::Support::PopCap::NewTypeObjectNotation::Encode(result);
                encode.process();
                auto newton_path = fmt::format("PROPERTIES\\{}.NEWTON", resources_name);
                resource_data_section_view_stored[newton_path] = std::move(encode.sen->toBytes());
                packet_definition.resource.emplace_back(Resource{
                    .path = String::to_posix_style(newton_path)});
            }
            else
            {
                auto data_stream = DataStreamView{};
                Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Encode::process_whole(data_stream, result.dump());
                auto rton_path = fmt::format("PROPERTIES\\{}.RTON", resources_name);
                resource_data_section_view_stored[rton_path] = std::move(data_stream.toBytes());
                packet_definition.resource.emplace_back(Resource{
                    .path = String::to_posix_style(rton_path)});
            }
            auto packet_stream = DataStreamView{};
            Sen::Kernel::Support::PopCap::ResourceStreamGroup::Pack::process_whole(packet_stream, packet_definition, resource_data_section_view_stored);
            auto manifest_name = toupper_back(fmt::format("__MANIFESTGROUP__{}", manifest_info.resource_additional_name));
            packet_data_section_view_stored[manifest_name] = std::move(packet_stream.toBytes());
            bundle.group[manifest_name].subgroup[manifest_name] = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation{
                .compression = k_highest_compression_method,
                .resource = packet_definition.resource};
            return;
        }

        inline static auto exchange_bundle(
            DataStreamView &stream,
            InformationStructure const &definition,
            std::string_view source,
            Setting const &setting) -> void
        {
            assert_conditional(definition.version == 4_ui, fmt::format("{}", Language::get("popcap.rsb.custom.version_is_not_support")), "exchange_bundle");
            auto bundle = BundleStructure{
                    .version = definition.version,
                    .texture_information_section_size = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::exchange_texture_information_version(definition.texture_information_version)};
            auto manifest = ManifestStructure{};
            auto packet_data_section_view_stored = DataSectionViewStored{};
            auto resource_info = CustomResourceInformation{};
            if (definition.packages_info_flag > static_cast<int>(k_none_size))
            {
                auto package_info = PackagesInfoFlag{};
                exchange_packages_info_flag(definition.packages_info_flag - 1_ui, package_info);
                exchange_packages(packet_data_section_view_stored, resource_info, bundle, package_info, fmt::format("{}/packages", source), setting);
            }
            exchange_packet(resource_info, bundle, packet_data_section_view_stored, definition, fmt::format("{}/packet", source), setting);
            exchange_manifest_group(bundle, packet_data_section_view_stored, resource_info, definition.manifest_info, setting);
            Sen::Kernel::Support::PopCap::ResourceStreamBundle::Pack::process_whole(stream, bundle, manifest, packet_data_section_view_stored);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            InformationStructure const &definition,
            std::string_view source,
            Setting const &setting) -> void
        {
            checking_info(definition);
            exchange_bundle(stream, definition, source, setting);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination,
            Setting const &setting) -> void
        {
            auto stream = DataStreamView{};
            auto definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            process_whole(stream, definition, source, setting);
            stream.out_file(destination);
            return;
        }
    };
}