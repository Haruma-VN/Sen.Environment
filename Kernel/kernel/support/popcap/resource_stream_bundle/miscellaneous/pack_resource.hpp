#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/pack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/pack.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle::Miscellaneous
{
    using namespace Definition;

    struct PackResource
    {
    protected:
        inline static auto exchange_packet(
            PacketStructure const &packet_structure,
            std::string source
        ) -> std::vector<uint8_t>
        {
            auto packet_stream = DataStreamView{};
            ResourceStreamGroup::Pack::process_whole(packet_stream, packet_structure, source);
            return packet_stream.toBytes();
        }

        inline static auto process(
            DataStreamView &stream,
            BundleStructure const &definition,
            ManifestStructure const &manifest,
            std::string &source) -> void
        {
            auto work_map = std::map<std::string, std::future<std::vector<uint8_t>>>{};
            for (auto &[group_id, group_information] : definition.group)
            {
                for (auto &[subgroup_id, subgroup_information] : group_information.subgroup)
                {
                    auto packet_structure = PacketStructure{
                        .version = definition.version,
                        .resource = subgroup_information.resource};
                    Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(subgroup_information.compression, packet_structure.compression);
                    work_map[subgroup_id] = std::async(&exchange_packet, packet_structure, source);
                }
            }
            auto packet_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            for (auto &[subgroup_id, future]: work_map) {
                packet_data_section_view_stored[subgroup_id] = future.get();
            }
            ResourceStreamBundle::Pack::process_whole(stream, definition, manifest, packet_data_section_view_stored);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            BundleStructure const &definition,
            ManifestStructure const &manifest,
            std::string_view source) -> void
        {
            auto packet_source = get_string(source);
            process(stream, definition, manifest, packet_source);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            BundleStructure definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            auto manifest = ManifestStructure{};
            if (definition.version <= 3_ui)
            {
                manifest = *FileSystem::read_json(fmt::format("{}/resource.json", source));
            }
            process_whole(stream, definition, manifest, source);
            stream.out_file(destination);
        }
    };
}