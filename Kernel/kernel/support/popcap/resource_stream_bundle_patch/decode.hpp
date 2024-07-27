#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_bundle/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle_patch/common.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundlePatch
{

    using namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle;

    using namespace Definition;

    struct Decode : Common
    {
    protected:
        inline static auto test_hash(
            std::vector<uint8_t> const &data,
            std::string const &hash) -> void
        {
            auto hash_test = Encryption::MD5::hash(data);
            assert_conditional(hash_test == hash, fmt::format("{}", Language::get("popcap.rsb_patch.invalid_md5")), "test_hash");
            return;
        }

        inline static auto process_sub(
            std::vector<uint8_t> const &before,
            DataStreamView &stream_patch,
            DataStreamView &stream_after,
            size_t const &patch_size
        ) -> void
        {
            auto patch = stream_patch.readBytes(patch_size);
            stream_after.writeBytes(Diff::VCDiff::decode<std::size_t>(reinterpret_cast<char const *>(before.data()), before.size(), reinterpret_cast<char const *>(patch.data()), patch.size()));
            return;
        }

        inline static auto process(
            DataStreamView &stream_before,
            DataStreamView &stream_patch,
            DataStreamView &stream_after) -> void
        {
            auto package_information = PackageInformation{};
            exchange_package_information(stream_patch, package_information);
            assert_conditional(package_information.magic == k_magic_identifier, String::format(fmt::format("{}", Language::get("popcap.rsbpatch.invalid_magic_header")), std::to_string(k_magic_identifier), std::to_string(package_information.magic)), "process"); 
            assert_conditional(package_information.version == k_magic_version, String::format(fmt::format("{}", Language::get("popcap.rsbpatch.invalid_version")), std::to_string(k_magic_version), std::to_string(package_information.version)), "process");
            auto packet_count = static_cast<size_t>(package_information.packet_count);
            auto information_section_patch_exist = static_cast<bool>(package_information.patch_exist);
            auto information_section_patch_size = static_cast<size_t>(package_information.patch_size);
            auto information_section_before_structure = ResourceStreamBundle::Common::Information{};
            auto information_section_after_structure = ResourceStreamBundle::Common::Information{};
            read_package_information_structure(stream_before, information_section_before_structure);
            auto information_section_before = stream_before.getBytes(k_begin_index, static_cast<size_t>(information_section_before_structure.header.information_section_size));
            test_hash(information_section_before, package_information.before_hash);
            if (!information_section_patch_exist)
            {
                stream_after.writeBytes(information_section_before);
            }
            else {
                process_sub(information_section_before, stream_patch, stream_after, information_section_patch_size);
            }
            read_package_information_structure(stream_after, information_section_after_structure);
            try_assert(packet_count == information_section_after_structure.subgroup_information.size(), "invalid_packet_count");
            auto packet_before_subgroup_information_index_map = std::map<std::string, size_t>{};
            indexing_subgroup_information_by_id(information_section_before_structure.subgroup_information, packet_before_subgroup_information_index_map);
            for (auto packet_index : Range(packet_count)) {
                auto & packet_after_subgroup_information = information_section_after_structure.subgroup_information.at(packet_index);
                auto packet_information = PacketInformation{};
                exchange_packet_information(stream_patch, packet_information);
                auto packet_name = packet_after_subgroup_information.id;
                auto packet_patch_exist = static_cast<bool>(packet_information.patch_exist);
                auto packet_patch_size = static_cast<size_t>(packet_information.patch_size);
                auto packet_before = std::vector<uint8_t>{};
                if (packet_before_subgroup_information_index_map.contains(packet_name)) {
                    auto packet_before_subgroup_information_index = packet_before_subgroup_information_index_map[packet_name];
                    auto packet_before_subgroup_information = information_section_before_structure.subgroup_information.at(packet_before_subgroup_information_index);
                    packet_before = stream_before.getBytes(static_cast<size_t>(packet_before_subgroup_information.offset), static_cast<size_t>(packet_before_subgroup_information.offset + packet_before_subgroup_information.size));
                }
                test_hash(packet_before, packet_information.before_hash);
                if (!packet_patch_exist) {
                    try_assert(packet_patch_size == k_none_size, "invalid_packet_patch_size");
                    stream_after.writeBytes(packet_before);
                }
                else {
                    process_sub(packet_before, stream_after, stream_patch, packet_patch_size);
                }
            }
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream_before,
            DataStreamView &stream_patch,
            DataStreamView &stream_after) -> void
        {
            process(stream_before, stream_patch, stream_after);
            return;
        }

        inline static auto process_fs(
            std::string_view source_before,
            std::string_view source_patch,
            std::string_view destination) -> void
        {
            auto stream_before = DataStreamView{source_before};
            auto stream_patch = DataStreamView{source_patch};
            auto stream_after = DataStreamView{};
            process_whole(stream_before, stream_patch, stream_after);
            stream_after.out_file(destination);
            return;
        }
    };

}