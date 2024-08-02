#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/modding/resource_stream_bundle/definition.hpp"
#include "kernel/support/miscellaneous/modding/resource_stream_bundle/common.hpp"
#include "kernel/support/miscellaneous/modding/packet_contains_resource_group/common.hpp"
#include "kernel/support/popcap/resource_stream_group/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/definition.hpp"
#include "kernel/support/popcap/reflection_object_notation/decode.hpp"
#include "kernel/support/popcap/new_type_object_notation/decode.hpp"
#include "kernel/support/popcap/resource_group/convert.hpp"
#include "kernel/support/texture/invoke.hpp"
#include "kernel/support/miscellaneous/shared.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::ResourceStreamBundle
{
    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using namespace Definition;

    using BundleStructure = Sen::Kernel::Support::PopCap::ResourceStreamBundle::BundleStructure;

    using ManifestStructure = Sen::Kernel::Support::PopCap::ResourceStreamBundle::ManifestStructure;

    using ResourceStreamBundleUnpack = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Unpack;

    using ImageFormat = Sen::Kernel::Support::Texture::Format;

    using namespace Sen::Kernel::Support::PopCap::ResourceStreamGroup;

    using ResourceStreamGroupUnpack = Sen::Kernel::Support::PopCap::ResourceStreamGroup::Unpack;

    using DataSectionViewStored = std::map<std::string, std::vector<uint8_t>, decltype(&case_insensitive_compare)>;

    using namespace Sen::Kernel::Support::Miscellaneous::Modding::PacketContainsResourceGroup;

    using SubgroupToWork = std::map<std::string, Sen::Kernel::Support::PopCap::ResourceStreamBundle::SubgroupInformation, decltype(&case_insensitive_compare)>;

    struct GroupInformationToWork
    {
        bool composite;
        SubgroupToWork subgroup;
    };

    using GroupToWork = std::map<std::string, GroupInformationToWork, decltype(&case_insensitive_compare)>;

    struct Unpack : Common
    {
    private:
        inline static auto exchange_manifest(
            std::vector<uint8_t> const &data,
            ResourceInfo &resource_info,
            ResInfo &res_info) -> void
        {
            auto packet_definition = PacketStructure{};
            auto packet_stream = DataStreamView{data};
            auto resource_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            ResourceStreamGroupUnpack::process_whole(packet_stream, packet_definition, resource_data_section_view_stored);
            auto resource_path_name = std::string{};
            for (auto &[id, _v] : resource_data_section_view_stored)
            {
                auto file_extenstion = Kernel::Path::getExtension(id);
                if (compare_string(file_extenstion, k_newton_extension_string))
                {
                    resource_path_name = id;
                    resource_info.use_new_type_resource = true;
                    break; // use_newton_if_it's_exist
                }
                if (compare_string(file_extenstion, k_rton_extension_string))
                {
                    resource_path_name = id;
                }
            }
            assert_conditional(!resource_path_name.empty(), fmt::format("{}", Language::get("pvz2.rsb.modding.manifest_does_not_contain_resource_info")), "exchange_manifest");
            auto &resource_data = resource_data_section_view_stored[resource_path_name];
            auto resource_json = nlohmann::ordered_json{};
            if (resource_info.use_new_type_resource)
            {
                auto decode = Sen::Kernel::Support::PopCap::NewTypeObjectNotation::Decode(resource_data);
                resource_json = decode.process();
            }
            else
            {
                auto stream = DataStreamView{resource_data};
                auto writer = JsonWriter{};
                Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Decode::process_whole(stream, writer);
                resource_json = nlohmann::ordered_json::parse(writer.ToString());
            }
            auto res_info_json = nlohmann::ordered_json{};
            try
            {
                auto convert = Sen::Kernel::Support::PopCap::ResourceGroup::Convert<true>();
                res_info_json = convert.convert_whole(resource_json);
                resource_info.expand_path = ExpandPath::String;
            }
            catch (nlohmann::ordered_json::exception &e)
            {
                auto convert = Sen::Kernel::Support::PopCap::ResourceGroup::Convert<false>();
                res_info_json = convert.convert_whole(resource_json);
                resource_info.expand_path = ExpandPath::Array;
            }
            exchange_res_info<true>(res_info_json, res_info);
            return;
        }

        inline static auto exchange_simple_info(
            InfoStructure &definition,
            BundleStructure const &bundle) -> void
        {
            definition.version = bundle.version;
            assert_conditional(definition.version == 4, "version_has_not_supported_yet", "exchange_simple_info"); // TODO: add to localization.
            definition.texture_information_version = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::exchange_texture_information_version(bundle.texture_information_section_size);
            return;
        }

        inline static auto exchange_unuse_resource(
            std::string const &id,
            SubgroupInfo const &value,
            std::string_view destination) -> void
        {
            write_json(fmt::format("{}/unuse_resource/{}.json", destination, id), value);
            return;
        }

        inline static auto exchange_group_to_work(
            BundleStructure const &bundle,
            GroupToWork &group_to_work) -> void
        {
            for (auto &[g_name, g_value] : bundle.group)
            {
                auto subgroup_to_work = SubgroupToWork(&case_insensitive_compare);
                for (auto &[s_name, s_value] : g_value.subgroup)
                {
                    subgroup_to_work[s_name] = std::move(s_value);
                }
                group_to_work[g_name] = GroupInformationToWork{
                    .composite = g_value.composite,
                    .subgroup = subgroup_to_work};
            }
            return;
        }

        inline static auto write_sen_compressed_group(
            std::vector<uint8_t> const &data,
            std::string const &path) -> void
        {
            write_bytes(path, data);
            return;
        };

        inline static auto exchange_group(
            InfoStructure &definition,
            BundleStructure const &bundle,
            ResInfo &res_info,
            DataSectionViewStored const &packet_data_section_view_stored,
            std::string_view destination) -> void
        {
            auto group_to_work = GroupToWork(&case_insensitive_compare);
            exchange_group_to_work(bundle, group_to_work);
            auto header_information = PacketContainsResourceGroup::Common::HeaderInformaiton{
                .magic = PacketContainsResourceGroup::Common::k_magic_identifier,
                .version = definition.version,
                .subgroup_information_section_block_size = PacketContainsResourceGroup::Common::k_subgroup_information_section_block_size};
            auto async_work_process = std::vector<std::future<void>>{};
            for (auto &[group_id, group_value] : res_info.group)
            {
                header_information.subgroup_information_section_block_count = static_cast<uint32_t>(k_begin_index);
                header_information.composite = group_value.composite;
                auto group_stream = DataStreamView{};
                auto resource_packet_view_stored = DataStreamView{};
                PacketContainsResourceGroup::Common::exchange_head_information(header_information, group_stream);
                PacketContainsResourceGroup::Common::exchange_padding_block(group_stream);
                header_information.subgroup_information_section_offset = static_cast<uint32_t>(group_stream.write_pos);
                auto subgroup_information_list = std::vector<PacketContainsResourceGroup::Common::SubgroupInformation>{};
                for (auto &[subgroup_id, subgroup_value] : group_value.subgroup)
                {
                    if (!packet_data_section_view_stored.contains(subgroup_id))
                    {
                        exchange_unuse_resource(subgroup_id, subgroup_value, destination);
                        continue;
                    }
                    auto &packet_data = packet_data_section_view_stored.at(subgroup_id);
                    auto is_image = subgroup_value.resolution != static_cast<int>(k_none_size);
                    auto subgroup_information = PacketContainsResourceGroup::Common::SubgroupInformation{
                        .id = subgroup_id,
                        .is_image = is_image,
                        .resource_group_stream_section_offset = static_cast<uint32_t>(resource_packet_view_stored.write_pos),
                        .resource_group_stream_section_block_size = static_cast<uint32_t>(packet_data.size())};
                    resource_packet_view_stored.writeBytes(packet_data);
                    PacketContainsResourceGroup::Common::exchange_padding_block(resource_packet_view_stored);
                    if (is_image)
                    { // if is_image
                        for (auto &[packet_id, packet_value] : subgroup_value.packet)
                        {
                            auto packet_resource_index = k_begin_index;
                            for (packet_resource_index; packet_resource_index < group_to_work.at(group_id).subgroup.at(subgroup_id).resource.size(); ++packet_resource_index)
                            {
                                auto &resource = group_to_work.at(group_id).subgroup.at(subgroup_id).resource[packet_resource_index];
                                auto &resource_path = resource.path;
                                if (compare_string(resource_path, fmt::format("{}.ptx", packet_value.image_info.path)))
                                {
                                    break;
                                }
                            }
                            try_assert(packet_resource_index + 1_size <= group_to_work.at(group_id).subgroup.at(subgroup_id).resource.size(), "invalid_packet_resource_index");
                            auto &resource = group_to_work.at(group_id).subgroup.at(subgroup_id).resource[packet_resource_index];
                            auto index = resource.texture_additional.value.index;
                            auto format = resource.texture_additional.value.texture_infomation.format;
                            // assert_conditional(format != empty_virtual_image_format, String::format(fmt::format("{}", Language::get("pvz2.rsb.modding.invalid_image_format")), std::to_string(format)), "exchange_group");
                            if (definition.is_ios_texture_format)
                            {
                                packet_value.image_info.format = exchange_image_format<true>(format);
                                // check if image trimed
                                if (packet_value.image_info.dimension.width < resource.texture_additional.value.dimension.width)
                                {
                                    packet_value.image_info.dimension.width = resource.texture_additional.value.dimension.width;
                                }
                                if (packet_value.image_info.dimension.height < resource.texture_additional.value.dimension.height)
                                {
                                    packet_value.image_info.dimension.height = resource.texture_additional.value.dimension.height;
                                }
                            }
                            else
                            {
                                packet_value.image_info.format = check_etc_format(exchange_image_format<false>(format), bundle.texture_information_section_size);
                            }
                            packet_value.image_info.index = resource.texture_additional.value.index;
                            group_to_work.at(group_id).subgroup.at(subgroup_id).resource.erase(group_to_work.at(group_id).subgroup.at(subgroup_id).resource.begin() + packet_resource_index);
                        }
                    }
                    auto content_data_string = dump_json(subgroup_value);
                    auto resource_content_information = PacketContainsResourceGroup::Common::ResourceContentInformation{
                        .magic = PacketContainsResourceGroup::Common::k_resource_content_information_magic_identifier,
                        .version = PacketContainsResourceGroup::Common::k_resource_content_information_version,
                        .information_string_size = static_cast<uint32_t>(content_data_string.size())};
                    subgroup_information.resource_content_information_offset = static_cast<uint32_t>(resource_packet_view_stored.write_pos);
                    auto compressed_data = Sen::Kernel::Definition::Encryption::Base64::encode(content_data_string);
                    resource_content_information.information_compressed_size = static_cast<uint32_t>(compressed_data.size());
                    PacketContainsResourceGroup::Common::exchange_resouce_content_information(resource_content_information, resource_packet_view_stored);
                    resource_packet_view_stored.writeString(compressed_data);
                    PacketContainsResourceGroup::Common::exchange_padding_block(resource_packet_view_stored);
                    subgroup_information_list.emplace_back(subgroup_information);
                    // PacketContainsResourceGroup::Common::exchange_from_subgroup(group_stream, subgroup_information);
                    ++header_information.subgroup_information_section_block_count;
                }
                auto information_section_size = static_cast<uint32_t>(group_stream.size()) + static_cast<uint32_t>(subgroup_information_list.size()) * PacketContainsResourceGroup::Common::k_subgroup_information_section_block_size;
                for (auto &subgroup_information : subgroup_information_list)
                {
                    subgroup_information.resource_group_stream_section_offset += information_section_size;
                    subgroup_information.resource_content_information_offset += information_section_size;
                }
                exchange_list(group_stream, subgroup_information_list, &PacketContainsResourceGroup::Common::exchange_from_subgroup);
                header_information.information_section_size = information_section_size;
                group_stream.writeBytes(resource_packet_view_stored.toBytes());
                PacketContainsResourceGroup::Common::exchange_head_information(header_information, group_stream);
                async_work_process.emplace_back(std::async(&write_bytes, fmt::format("{}/packet/{}.scg", destination, group_id), group_stream.toBytes()));
                definition.group.emplace_back(group_id);
            }
            async_process_list<void>(async_work_process);
            return;
        }

        inline static auto exchange_package(
            DataSectionViewStored &packet_data_section_view_stored,
            PackageInfo &package_info,
            std::string_view destination) -> void
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

            auto rton_encrypted_count = k_begin_index;
            for (auto &[group_id, group_value] : packet_data_section_view_stored)
            {
                if (compare_string(group_id, k_package_string))
                {
                    auto packet_definition = PacketStructure{};
                    auto packet_stream = DataStreamView{group_value};
                    auto resource_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
                    Sen::Kernel::Support::PopCap::ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_definition, resource_data_section_view_stored);
                    FileSystem::create_directory(tolower_back(fmt::format("{}/{}", destination, k_package_string)));
                    for (auto &[id, data] : resource_data_section_view_stored)
                    {
                        if (check_rton_is_encrypted(data))
                        {
                            ++rton_encrypted_count;
                        }
                        write_bytes(fmt::format("{}/{}", destination, id), data);
                    }
                    if (rton_encrypted_count > static_cast<int>(resource_data_section_view_stored.size() / 2))
                    { // > 50%
                        package_info.rton_is_encrypted = true;
                    }
                    package_info.use_package_info = true;
                    break;
                }
            }
            return;
        }

        inline static auto process_package(
            DataStreamView &stream,
            InfoStructure &definition,
            std::string_view destination) -> void
        {
            auto bundle = BundleStructure{};
            auto manifest = ManifestStructure{};
            auto packet_data_section_view_stored = DataSectionViewStored(&case_insensitive_compare);
            ResourceStreamBundleUnpack::process_whole(stream, bundle, manifest, packet_data_section_view_stored);
            exchange_simple_info(definition, bundle);
            auto manifest_name = get_string(k_manifest_string);
            for (auto &[group_name, _v] : packet_data_section_view_stored)
            {
                if (group_name.starts_with(manifest_name))
                {
                    if (group_name.size() > manifest_name.size())
                    {
                        definition.resource_info.resource_additional_name = group_name.substr(manifest_name.size(), group_name.size() - manifest_name.size());
                        manifest_name = group_name;
                    }
                    break;
                }
            }
            assert_conditional(packet_data_section_view_stored.contains(manifest_name), String::format(fmt::format("{}", Language::get("pvz2.rsb.modding.cannot_find_manifest")), manifest_name), "process_package");
            auto &manifest_packet = packet_data_section_view_stored[manifest_name];
            auto res_info = ResInfo{};
            exchange_manifest(manifest_packet, definition.resource_info, res_info);
            exchange_package(packet_data_section_view_stored, definition.package_info, destination);
            if (definition.package_info.use_package_info)
            {
                res_info.group.erase(get_string(k_package_string));
            }
            exchange_group(definition, bundle, res_info, packet_data_section_view_stored, destination);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            InfoStructure &definition,
            std::string_view destination) -> void
        {
            process_package(stream, definition, destination);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination,
            bool is_ios_texture_format) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = InfoStructure{
                .is_ios_texture_format = is_ios_texture_format};
            process_whole(stream, definition, destination);
            write_json(fmt::format("{}/data.json", destination), definition);
            return;
        }
    };
}