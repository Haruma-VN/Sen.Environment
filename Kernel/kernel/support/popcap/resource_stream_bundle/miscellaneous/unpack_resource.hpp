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
        inline static auto exchange_packet(
            std::vector<uint8_t> const &packet_data,
            std::string destination) -> void
        {
            auto packet_stream = DataStreamView{packet_data};
            auto packet_definition = ResourceStreamGroup::PacketStructure{};
            ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_definition, destination);
            return;
        }

        inline static auto process(
            DataStreamView &stream,
            BundleStructure &definition,
            ManifestStructure &manifest,
            std::string &destination) -> void
        {
            auto packet_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            ResourceStreamBundle::Unpack::process_whole(stream, definition, manifest, packet_data_section_view_stored);
            auto packet_definition = ResourceStreamGroup::PacketStructure{};
            auto work_list = std::vector<std::future<void>>{};
            for (auto const &it : packet_data_section_view_stored)
            {
                work_list.emplace_back(std::async(&exchange_packet, it.second, destination));
            }
            async_process_list<void>(work_list);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            BundleStructure &definition,
            ManifestStructure &manifest,
            std::string_view destination) -> void
        {
            auto bundle_destination = get_string(destination);
            process(stream, definition, manifest, bundle_destination);
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