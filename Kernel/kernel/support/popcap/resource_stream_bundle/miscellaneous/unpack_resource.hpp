#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/unpack.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle::Miscellaneous
{
    using namespace Definition;

    struct UnpackResource 
    {
    protected:
        inline static auto process(
            DataStreamView &stream,
            typename BundleStructure &definition,
            typename ManifestStructure &manifest,
            std::string_view destination
        ) -> void
        {
            auto packet_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            ResourceStreamBundle::Unpack::process_whole(stream, definition, manifest, packet_data_section_view_stored);
            auto packet_definition = ResourceStreamGroup::PacketStructure{};
            for (auto &[packet_id, packet_data] : packet_data_section_view_stored) {
                auto packet_stream = DataStreamView{packet_data};
                ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_definition, destination);
            }
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            typename BundleStructure &definition,
            typename ManifestStructure &manifest,
            std::string_view destination) -> void
        {
            process(stream, definition, manifest, destination);
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
                write_json(fmt::format("{}/manifest.json", destination), manifest);
            }
            return;
        }
    };
}