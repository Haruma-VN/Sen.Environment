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
        inline static auto process(
            DataStreamView &stream,
            typename BundleStructure const &definition,
             typename ManifestStructure const &manifest,
            std::string_view source) -> void
        {
            auto packet_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            for (auto &[group_id, group_information] : definition.group)
            {
                for (auto &[subgroup_id, subgroup_information] : group_information.subgroup)
                {
                    auto packet_stream = DataStreamView{};
                    auto packet_definition = PacketStructure{
                        .version = definition.version,
                        .compression = subgroup_information.compression,
                        .resource = subgroup_information.resource
                    };
                    ResourceStreamGroup::Pack::process_whole(packet_stream, packet_definition, source);
                    packet_data_section_view_stored[subgroup_id] = std::move(packet_stream.toBytes());
                }
            }
            ResourceStreamBundle::Pack::process_whole(stream, definition, manifest, packet_data_section_view_stored);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            typename BundleStructure const &definition,
             typename ManifestStructure const &manifest,
            std::string_view source) -> void
        {
            process(stream, definition, manifest, source);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            auto definition = *FileSystem::read_json(fmt::format("{}/definition.json", destination));
            auto manifest = ManifestStructure{};
            if (definition["version"].get<uint32_t>() <= 3_ui) {
                manifest = *FileSystem::read_json(fmt::format("{}/manifest.json", source));
            }
            process_whole(stream, definition, manifest, source);
            stream.out_file(destination);
        }
    };
}