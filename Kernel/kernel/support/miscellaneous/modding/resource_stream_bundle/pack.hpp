#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/invoke.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/miscellaneous/modding/packet_contains_resource_group/common.hpp"
#include "kernel/support/popcap/reflection_object_notation/encode.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::ResourceStreamBundle
{
    using BundleStructure = Sen::Kernel::Support::PopCap::ResourceStreamBundle::BundleStructure;

    using ManifestStructure = Sen::Kernel::Support::PopCap::ResourceStreamBundle::ManifestStructure;

    using ResourceStreamBundlePack = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Pack;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using namespace Definition;

    using DataSectionViewStored = std::map<std::string, std::vector<uint8_t>, decltype(&case_insensitive_compare)>;

    struct Pack : Common
    {
    private:
        inline static auto exchange_simple_info(
            BundleStructure &bundle,
            InfoStructure const &definition) -> void
        {
            bundle.version = definition.version;
            assert_conditional(bundle.version == 4, "version_has_not_supported_yet", "exchange_simple_info"); // TODO: add to localization.
            bundle.texture_information_section_size = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::exchange_texture_information_version(definition.texture_information_version);
            return;
        }

        inline static auto exchange_package(
            ResInfo &res_info,
            BundleStructure &bundle,
            PackageInfo const &package_info,
            std::vector<uint8_t> &value,
            std::string const &packages_folder) -> void
        {
            auto packages_list = FileSystem::read_whole_directory(packages_folder);
            auto resource_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            auto get_resfile = [](
                                   std::string const &path) -> std::string
            {
                auto str = Sen::Kernel::String(path);
                auto resfile = fmt::format("RESFILE_{}", String::join(str.split(double_backsplash), "_"));
                return resfile.substr(k_begin_index, resfile.size() - ".rton"_sv.size());
            };
            auto packages_string_name = get_string(k_package_string);
            auto &packages_packet_info = res_info.group[packages_string_name].subgroup[packages_string_name].packet[get_string(PacketContainsResourceGroup::Common::k_packet_general_type_name)];
            auto packet_definition = PacketStructure{
                .version = bundle.version,
                .compression = Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::PacketCompression{
                    .general = true,
                    .texture = true}};
            auto push_definition = [&](
                                       std::string const &path) -> void
            {
                packet_definition.resource.emplace_back(Resource{
                    .path = String::to_posix_style(toupper_back(path))});
                packages_packet_info.data[get_resfile(path)] = DataInfo{
                    .type = PacketContainsResourceGroup::Common::DataType::File,
                    .path = String::to_posix_style(tolower_back(path))};
            };
            auto push = [&](std::string_view ext) -> void
            {
                for (auto &element : packages_list)
                {
                    auto file_extension = Path::getExtension(element);
                    if (compare_string(ext, file_extension))
                    {
                        switch (hash_sv(ext))
                        {
                        case (hash_sv(".json"_sv)):
                        {
                            auto pos = packages_folder.size() - k_package_string.size();
                            auto path = toupper_back(element.substr(pos, (element.size() - pos - ".json"_sv.size())) + ".rton");
                            auto data_resource_stream = DataStreamView{};
                            Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Encode::process_whole(data_resource_stream, FileSystem::read_file(element));
                            if (package_info.rton_is_encrypted)
                            {
                                auto data_encrypt_stream = DataStreamView{};
                                Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Instance::encrypt(data_resource_stream, data_encrypt_stream, k_china_reflection_object_natation_rijndael_key_string, k_china_reflection_object_natation_rijndael_iv_string);
                                resource_data_section_view_stored[path] = std::move(data_encrypt_stream.toBytes());
                            }
                            else
                            {
                                resource_data_section_view_stored[path] = std::move(data_resource_stream.toBytes());
                            }
                            push_definition(path);
                            break;
                        }
                        case (hash_sv(".rton"_sv)):
                        {
                            auto pos = packages_folder.size() - k_package_string.size();
                            auto path = toupper_back(element.substr(pos, (element.size() - pos)));
                            if (!resource_data_section_view_stored.contains(path))
                            {
                                resource_data_section_view_stored[path] = std::move(FileSystem::read_binary<uint8_t>(element));
                            }
                            push_definition(path);
                            break;
                        }
                        }
                    }
                }
            };
            if (package_info.auto_convert_jsons_exist)
            {
                push(".json"_sv);
            }
            push(".rton"_sv);
            auto packet_stream = DataStreamView{};
            Sen::Kernel::Support::PopCap::ResourceStreamGroup::Pack::process_whole(packet_stream, packet_definition, resource_data_section_view_stored);
            value = std::move(packet_stream.toBytes());
            bundle.group[packages_string_name].subgroup[packages_string_name] = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation{
                .compression = k_highest_compression_method,
                .resource = packet_definition.resource};
            return;
        }

        inline static auto exchange_group(
            DataStreamView &stream,
            ResInfo &res_info,
            BundleStructure &bundle,
            InfoStructure const &definition,
            DataSectionViewStored &packet_data_section_view_stored,
            std::string const &packet_folder) -> void
        {
            auto header_information = PacketContainsResourceGroup::Common::HeaderInformaiton{};
            auto genenal_type_name = get_string(PacketContainsResourceGroup::Common::k_packet_general_type_name);
            for (auto &group_id : definition.group)
            {
                auto group_stream = DataStreamView{fmt::format("{}/{}.scg", packet_folder, group_id)};
                PacketContainsResourceGroup::Common::exchange_head_information(group_stream, header_information);
                assert_conditional(header_information.magic == PacketContainsResourceGroup::Common::k_magic_identifier, "invaild_scg_magic", "exchange_group"); // TODO: add to localization.
                assert_conditional(header_information.version == bundle.version, "invaild_scg_version", "exchange_group");                                      // TODO: add to localization.
                auto &group_information = bundle.group[group_id];
                group_information.composite = header_information.composite;
                auto &res_group_info = res_info.group[group_id];
                res_group_info.composite = header_information.composite;
                auto subgroup_information_list = std::vector<PacketContainsResourceGroup::Common::SubgroupInformation>{};
                group_stream.read_pos = header_information.subgroup_information_section_offset;
                subgroup_information_list.resize(header_information.subgroup_information_section_block_count);
                exchange_list(group_stream, subgroup_information_list, &PacketContainsResourceGroup::Common::exchange_to_subgroup);
                for (auto &subgroup : subgroup_information_list)
                {
                    packet_data_section_view_stored[subgroup.id] = std::move(group_stream.readBytes(static_cast<size_t>(subgroup.resource_group_stream_section_block_size), static_cast<size_t>(subgroup.resource_group_stream_section_offset)));
                    group_stream.read_pos = static_cast<size_t>(subgroup.resource_content_information_offset);
                    auto resource_content_information = PacketContainsResourceGroup::Common::ResourceContentInformation{};
                    PacketContainsResourceGroup::Common::exchange_resouce_content_information(group_stream, resource_content_information);
                    assert_conditional(resource_content_information.magic == PacketContainsResourceGroup::Common::k_resource_content_information_magic_identifier, fmt::format("invaild_resource_content_magic at {}", group_id), "exchange_group"); // TODO: add to localization.
                    assert_conditional(resource_content_information.version == PacketContainsResourceGroup::Common::k_resource_content_information_version, fmt::format("invaild_resource_content_size at {}", group_id), "exchange_group"); // TODO: add to localization.
                    auto compressed_data = group_stream.readString(static_cast<size_t>(resource_content_information.information_compressed_size));
                    auto content_data_string = Sen::Kernel::Definition::Encryption::Base64::decode(compressed_data);
                    assert_conditional(content_data_string.size() == static_cast<size_t>(resource_content_information.information_string_size), fmt::format("invaild_resource_content_sizeat {}", group_id), "exchange_group"); // TODO: add to localization.
                    SubgroupInfo subgroup_info = nlohmann::ordered_json::parse(content_data_string);
                    res_group_info.subgroup[subgroup.id] = subgroup_info;
                    auto subgroup_information = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation{
                        .category = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SimpleCategoryInformation{
                            .resolution = subgroup_info.resolution}};
                    subgroup_information.compression = k_highest_compression_method;
                    if (subgroup.is_image)
                    {
                        for (auto &[packet_id, packet_value] : subgroup_info.packet)
                        {
                            if (compare_string(k_program_path, packet_value.image_info.path))
                            {
                                continue;
                            }
                            auto resource = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Resource{
                                .use_texture_additional_instead = true};
                            resource.path = toupper_back(fmt::format("{}.PTX", packet_value.image_info.path));
                            resource.texture_additional.value.index = packet_value.image_info.index;
                            definition.is_ios_texture_format ? exchange_texture_value<true>(resource.texture_additional.value, packet_value.image_info, bundle.texture_information_section_size) : exchange_texture_value<false>(resource.texture_additional.value, packet_value.image_info, bundle.texture_information_section_size);
                            subgroup_information.resource.emplace_back(resource);
                        }
                    }
                    else
                    {
                        for (auto &[data_id, data_value] : subgroup_info.packet[genenal_type_name].data)
                        {
                            if (compare_string(k_program_path, data_value.path))
                            {
                                continue;
                            }
                            subgroup_information.resource.emplace_back(Sen::Kernel::Support::PopCap::ResourceStreamBundle::Resource{
                                .path = toupper_back(data_value.path)});
                        }
                    }
                    group_information.subgroup[subgroup.id] = subgroup_information;
                }
            }
            return;
        }

        inline static auto checking_info(
            InfoStructure const &definition) -> void
        {
            auto duplicate_group = std::vector<string>{};
            find_duplicate_elements<string>(definition.group, duplicate_group);
            assert_conditional(duplicate_group.empty(), fmt::format("group: {} is duplicated", duplicate_group[0]), "checking_info"); // TODO: add to localization.
            return;
        }

        inline static auto exchange_manifest(
            BundleStructure &bundle,
            ResourceInfo const &resource_info,
            ResInfo const &res_info,
            DataSectionViewStored &packet_data_section_view_stored) -> void
        {
            auto packet_definition = PacketStructure{
                .version = bundle.version,
                .compression = Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::PacketCompression{
                    .general = true,
                    .texture = true}};
            auto res_info_json = nlohmann::ordered_json{};
            exchange_resource_info(res_info, res_info_json);
            auto resource_json = nlohmann::ordered_json{};
            auto convert = Sen::Kernel::Support::PopCap::ResInfo::Convert::Convert();
            convert.process(res_info_json, resource_json);
            auto resources_name = toupper_back(fmt::format("{}{}", k_resources_string, resource_info.resource_additional_name));
            auto resource_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            if (resource_info.use_new_type_resource) {
                auto encode = Sen::Kernel::Support::PopCap::NewTypeObjectNotation::Encode(resource_json);
                encode.process();
                auto newton_path = fmt::format("PROPERTIES\\{}.NEWTON", resources_name);
                resource_data_section_view_stored[newton_path] = std::move(encode.sen->toBytes());
                packet_definition.resource.emplace_back(Resource{
                    .path = String::to_posix_style(newton_path)
                });
            }
            auto data_stream = DataStreamView{};
            Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Encode::process_whole(data_stream, resource_json.dump());
            auto rton_path = fmt::format("PROPERTIES\\{}.RTON", resources_name);
            resource_data_section_view_stored[rton_path] = std::move(data_stream.toBytes());
            packet_definition.resource.emplace_back(Resource{
                    .path = String::to_posix_style(rton_path)
            });
            auto packet_stream = DataStreamView{};
            Sen::Kernel::Support::PopCap::ResourceStreamGroup::Pack::process_whole(packet_stream, packet_definition, resource_data_section_view_stored);
            auto manifest_name = toupper_back(fmt::format("{}{}", k_manifest_string, resource_info.resource_additional_name));
            packet_data_section_view_stored[manifest_name] = std::move(packet_stream.toBytes());
            bundle.group[manifest_name].subgroup[manifest_name] = Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation{
                .compression = k_highest_compression_method,
                .resource = packet_definition.resource};
            return;
        }

        inline static auto process_package(
            DataStreamView &stream,
            InfoStructure const &definition,
            std::string_view source) -> void
        {
            auto bundle = BundleStructure{};
            auto manifest = ManifestStructure{};
            auto res_info = ResInfo{};
            exchange_simple_info(bundle, definition);
            auto packet_data_section_view_stored = DataSectionViewStored(&case_insensitive_compare);
            if (definition.package_info.use_package_info)
            {
                exchange_package(res_info, bundle, definition.package_info, packet_data_section_view_stored[get_string(k_package_string)], fmt::format("{}/packages", source));
            }
            exchange_group(stream, res_info, bundle, definition, packet_data_section_view_stored, fmt::format("{}/packet", source));
            exchange_manifest(bundle,definition.resource_info, res_info, packet_data_section_view_stored);
            ResourceStreamBundlePack::process_whole(stream, bundle, manifest, packet_data_section_view_stored);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            InfoStructure const &definition,
            std::string_view source) -> void
        {
            checking_info(definition);
            process_package(stream, definition, source);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            auto definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            process_whole(stream, definition, source);
            stream.out_file(destination);
            return;
        }
    };
}