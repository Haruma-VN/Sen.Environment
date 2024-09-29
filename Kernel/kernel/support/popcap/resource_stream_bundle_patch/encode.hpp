#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_bundle/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle_patch/common.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundlePatch
{
    
    using namespace Definition;

    struct Encode : Common
    {
    protected:
        inline static auto test_hash(
            std::vector<uint8_t> const &data,
            std::string &hash
        ) -> void
        {
            hash = Encryption::MD5::hash(data);
            return;
        }

        inline static auto process_sub(
            std::vector<uint8_t> const &before_data,
            std::vector<uint8_t> const &after_data
        ) -> std::vector<uint8_t>
        {
            return Diff::VCDiff::encode<std::size_t, Diff::VCDiff::Flag::VCD_FORMAT_INTERLEAVED>(
                    reinterpret_cast<char const *>(before_data.data()), before_data.size(),
                    reinterpret_cast<char const *>(after_data.data()), after_data.size());
        }

        inline static auto process(
            DataStreamView &stream_before,
            DataStreamView &stream_after,
            DataStreamView &stream_patch
        ) -> void
        {
            auto package_information = PackageInformation{};
            auto information_section_before_structure = ResourceStreamBundle::Common::Information{};
            auto information_section_after_structure = ResourceStreamBundle::Common::Information{};
            read_package_information_structure(stream_before, information_section_before_structure);
            read_package_information_structure(stream_after, information_section_after_structure);
            auto information_section_before = stream_before.getBytes(k_begin_index, static_cast<size_t>(information_section_before_structure.header.information_section_size));
            auto information_section_after = stream_after.getBytes(k_begin_index, static_cast<size_t>(information_section_after_structure.header.information_section_size));
            test_hash(information_section_before, package_information.before_hash);
            auto information_section_patch_exist = false;
            auto information_section_patch_size = k_none_size;
            stream_patch.writeNull(information_header_section_size);
            information_section_patch_exist = information_section_after != information_section_before;
            if (information_section_patch_exist) {
                auto vcdiff_data = process_sub(information_section_before, information_section_after);
                information_section_patch_size = vcdiff_data.size();
                stream_patch.writeBytes(vcdiff_data);
            }
            package_information.patch_exist = static_cast<uint32_t>(information_section_patch_exist);
            package_information.patch_size = static_cast<uint32_t>(information_section_patch_size);
            auto packet_before_subgroup_information_index_map = std::map<std::string, size_t>{};
            indexing_subgroup_information_by_id(information_section_before_structure.subgroup_information, packet_before_subgroup_information_index_map);
            auto packet_count = information_section_after_structure.header.subgroup_information_section_block_count;
            package_information.packet_count = packet_count;
            for (auto packet_index : Range<size_t>(packet_count)) {
                auto & packet_after_subgroup_information = information_section_after_structure.subgroup_information.at(packet_index);
                auto packet_information = PacketInformation{};
                auto packet_patch_size = k_none_size;
                auto packet_name = packet_after_subgroup_information.id;
                auto packet_before = std::vector<uint8_t>{};
                if (packet_before_subgroup_information_index_map.contains(packet_name)) {
                    auto packet_before_subgroup_information_index = packet_before_subgroup_information_index_map[packet_name];
                    auto & packet_before_subgroup_information = information_section_before_structure.subgroup_information.at(packet_before_subgroup_information_index);
                    packet_before = stream_before.getBytes(static_cast<size_t>(packet_before_subgroup_information.offset), static_cast<size_t>(packet_before_subgroup_information.offset + packet_before_subgroup_information.size));
                }
                test_hash(packet_before, packet_information.before_hash);
                auto packet_after = stream_after.getBytes(static_cast<size_t>(packet_after_subgroup_information.offset), static_cast<size_t>(packet_after_subgroup_information.offset + packet_after_subgroup_information.size));
                auto packet_patch_exist = !((packet_before.size() == packet_after.size()) && std::equal(packet_before.begin(), packet_before.end(), packet_after.begin()));
                packet_information.name = packet_name;
                packet_information.patch_exist = static_cast<uint32_t>(packet_patch_exist);
                packet_information.patch_size = static_cast<uint32_t>(packet_patch_size);
                auto vcdiff_data = std::vector<uint8_t>{};
                if (packet_patch_exist) {
                    vcdiff_data = process_sub(packet_before, packet_after);
                    packet_information.patch_size = vcdiff_data.size();
                }
                exchange_packet_information(packet_information, stream_patch);
                stream_patch.writeBytes(vcdiff_data);
            }
            package_information.all_after_size = static_cast<uint32_t>(stream_after.size());
            package_information.packet_count = static_cast<uint32_t>(packet_count);
            package_information.patch_exist = static_cast<uint32_t>(information_section_patch_exist);
            package_information.patch_size = static_cast<uint32_t>(information_section_patch_size);
            package_information.magic = k_magic_identifier;
            package_information.version = k_magic_version;
            package_information.unknown_1 = k_unknown_section_1;
            exchange_package_information(package_information, stream_patch);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream_before,
            DataStreamView &stream_after,
            DataStreamView &stream_patch
        ) -> void
        {
            process(stream_before, stream_after, stream_patch);
            return;
        }


        inline static auto process_fs(
            std::string_view source_before,
            std::string_view source_after,
            std::string_view destination
        ) -> void
        {
            auto stream_before = DataStreamView{source_before};
            auto stream_after = DataStreamView{source_after};
            auto stream_patch = DataStreamView{};
            process_whole(stream_before, stream_after, stream_patch);
            stream_patch.out_file(destination);
            return;
        }
    };

}