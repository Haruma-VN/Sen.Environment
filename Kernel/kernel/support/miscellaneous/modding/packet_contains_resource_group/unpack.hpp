#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::PacketContainsResourceGroup
{

    using namespace Definition;

    struct Unpack : Common
    {
    private:
        inline static auto exchange_nessessory_data(
            DataStreamView &stream,
            std::vector<SubgroupInformation> const &data,
            std::map<std::string, PacketInformation> &value) -> void
        {
            for (auto &subgroup : data)
            {
                value[subgroup.id].is_image = subgroup.is_image;
                auto packet_stream = DataStreamView{std::move(stream.readBytes(static_cast<size_t>(subgroup.resource_group_stream_section_block_size), static_cast<size_t>(subgroup.resource_group_stream_section_offset)))};
                Sen::Kernel::Support::PopCap::ResourceStreamGroup::Unpack::process_whole(packet_stream, value[subgroup.id].packet_structure, value[subgroup.id].resource_data_section_view_stored);
                stream.read_pos = static_cast<size_t>(subgroup.resource_content_information_offset);
                auto resource_content_information = ResourceContentInformation{};
                exchange_resouce_content_information(stream, resource_content_information);
                assert_conditional(resource_content_information.magic == k_resource_content_information_magic_identifier, "invaild_resource_content_magic", "exchange_resource_info"); // TODO: add to localization.
                assert_conditional(resource_content_information.version == k_resource_content_information_version, "invaild_resource_content_version", "exchange_resource_info");         // TODO: add to localization.
                auto compressed_data = stream.readString(static_cast<size_t>(resource_content_information.information_compressed_size));
                auto content_data_string = Sen::Kernel::Definition::Encryption::Base64::decode(compressed_data);
                assert_conditional(content_data_string.size() == static_cast<size_t>(resource_content_information.information_string_size), "invaild_resource_content_size", "exchange_resource_info"); // TODO: add to localization.
                value[subgroup.id].resoucre_content_information = nlohmann::ordered_json::parse(content_data_string);
            }
            return;
        }

        inline static auto exchange_general_additional(
            std::map<std::string, PacketInformation> resource_information,
            std::string_view destination
        ) -> void
        {
            for (auto &[group_id, resource_value] : resource_information) {
                debug(group_id);
            }
            return;
        }

        inline static auto exchange_texture_additional(
            std::map<std::string, PacketInformation> resource_information,
            std::string_view destination
        ) -> void
        {
            return;
        }

        inline static auto process_package(
            DataStreamView &stream,
            Information &definition,
            std::string_view destination) -> void
        {
            auto header_information = HeaderInformaiton{};
            exchange_head_information(stream, header_information);
            assert_conditional(header_information.magic == k_magic_identifier, "invaild_scg_magic", "process_package"); // TODO: add to localization.
            // assert_conditional(header_information.version == bundle.version, "invaild_scg_version", "exchange_group"); // TODO: add to localization.
            definition.composite = header_information.composite;
            auto subgroup_information_list = std::vector<SubgroupInformation>{};
            stream.read_pos = header_information.subgroup_information_section_offset;
            subgroup_information_list.resize(header_information.subgroup_information_section_block_count);
            exchange_list(stream, subgroup_information_list, &exchange_to_subgroup);
            auto resource_information = std::map<std::string, PacketInformation>{};
            exchange_nessessory_data(stream, subgroup_information_list, resource_information);
            definition.composite ? exchange_texture_additional(resource_information, destination) : exchange_general_additional(resource_information, destination);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            Information &definition,
            std::string_view destination) -> void
        {
            process_package(stream, definition, destination);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = Information{};
            process_whole(stream, definition, destination);
            write_json(fmt::format("{}/data.json", destination), definition);
            return;
        }
    };
}