#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/modding/resource_stream_bundle/definition.hpp"
#include "kernel/support/miscellaneous/modding/resource_stream_bundle/common.hpp"
#include "kernel/support/popcap/resource_stream_group/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/definition.hpp"
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

    struct Unpack : Common
    {
    private:
        inline static auto exchange_manifest_group(
            std::vector<uint8_t> &packet
        ) -> void
        {
            /*
            auto packet_definition = PacketStructure{};
            auto packet_stream = DataStreamView{};
            auto resource_data_stored = std::map<std::string, std::vector<uint8_t>>{};
            ResourceStreamGroupUnpack::process_whole(packet_stream, packet_definition, resource_data_stored);
            for (auto &[id, value] : resource_data_stored) {
                debug(id);
            }
            */
            return;
        }

        inline static auto process_package(
            DataStreamView &stream,
            InfoStructure &definition,
            std::string const &packet_folder) -> void
        {
            /*
            auto bundle = BundleStructure{};
            auto manifest = ManifestStructure{};
            auto packet_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
            ResourceStreamBundleUnpack::process_whole(stream, bundle, manifest, packet_data_section_view_stored);
            assert_conditional(packet_data_section_view_stored.contains(Common::k_manifest_string), "cannot find manifest", "process_package");
            auto &manifest_packet = packet_data_section_view_stored[Common::k_manifest_string];
            exchange_manifest_group(manifest_packet);
            */

            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            InfoStructure &definition,
            std::string_view destination) -> void
        {
            process_package(stream, definition, fmt::format("{}/Packet", destination));
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
            write_json(fmt::format("{}/definition.json", destination), definition);
            return;
        }
    };
}