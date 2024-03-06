#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/rsb/definition.hpp"
#include "kernel/support/popcap/rsb/unpack.hpp"
#include "kernel/support/popcap/rsb_patch/common.hpp"

namespace Sen::Kernel::Support::PopCap::RSBPatch
{

    using namespace Sen::Kernel::Support::PopCap::RSB;
    
    using namespace Definition;

    class Decode
    {
        private:

            using VCDiff = Diff::VCDiff;

            std::unique_ptr<DataStreamView> rsb_before;

            std::unique_ptr<DataStreamView> rsb_patch;

            std::unique_ptr<DataStreamView> result;

        template <typename T>
            requires std::is_integral<T>::value
        struct RSBPatchHeadExpand
        {
        public:
            T rsb_after_file_size;
            T rsb_head_section_patch_size;
            std::string md5_rsb_before;
            T rsg_number;
            bool rsb_need_patch;

            explicit RSBPatchHeadExpand() = default;

            explicit RSBPatchHeadExpand(
                T rsb_after_file_size,
                T rsb_head_section_patch_size,
                const std::string &md5_rsb_before,
                T rsg_number,
                bool rsb_need_patch) : rsb_after_file_size(rsb_after_file_size), rsb_head_section_patch_size(rsb_head_section_patch_size),
                                       md5_rsb_before(md5_rsb_before), rsg_number(rsg_number), rsb_need_patch(rsb_need_patch)
            {
            }

            ~RSBPatchHeadExpand() = default;
        };

        struct RSBPatchSubgroupInfo
        {
        public:
            int packet_patch_size;
            std::string packet_name;
            std::string md5_packet;

            explicit RSBPatchSubgroupInfo() = default;

            explicit RSBPatchSubgroupInfo(
                int packet_patch_size,
                const std::string &packet_name,
                const std::string &md5_packet) : packet_patch_size(packet_patch_size), packet_name(packet_name), md5_packet(md5_packet)
            {
            }

            ~RSBPatchSubgroupInfo() = default;
        };

    protected:
        template <typename T>
            requires std::is_integral<T>::value
        auto read_rsb_patch_head(
            RSBPatchHeadExpand<T> *rsg_patch_head_info)
        {
            auto magic = rsb_patch->readString(4);
            if (magic != "PBSR"_sv)
            {
                throw Exception(fmt::format("{}", Language::get("popcap.rsb_patch.decode.invalid_magic")), std::source_location::current(), "read_rsb_patch_head");
            }
            rsg_patch_head_info->rsb_after_file_size = rsb_patch->readInt32(0xCull);
            rsg_patch_head_info->rsb_head_section_patch_size = rsb_patch->readInt32(0x14ull);
            rsg_patch_head_info->md5_rsb_before = Common::to_string(rsb_patch->readBytes(16));
            rsg_patch_head_info->rsg_number = rsb_patch->readInt32();
            rsg_patch_head_info->rsb_need_patch = rsb_patch->readInt32() == 1;
            return;
        }

        auto read_subgroup_info(
            RSBPatchSubgroupInfo *rsg_patch_subgroup_info)
        {
            auto start_pos = rsb_patch->read_pos;
            rsg_patch_subgroup_info->packet_patch_size = rsb_patch->readInt32(start_pos + 4ull);
            rsg_patch_subgroup_info->packet_name = rsb_patch->readStringByEmpty();
            auto data = rsb_patch->readBytes(16, start_pos + 136ull);
            rsg_patch_subgroup_info->md5_packet = Common::to_string(data);
            return;
        }

    public:
        explicit Decode(
            std::string_view source_before,
            std::string_view rsb_patch) : rsb_before(std::make_unique<DataStreamView>(source_before)),
                                             rsb_patch(std::make_unique<DataStreamView>(rsb_patch)), result(std::make_unique<DataStreamView>())
        {
        }

        ~Decode(

            ) = default;

        template <typename T, typename D, auto use_raw_packet = false>
        requires std::is_integral<T>::value && std::is_unsigned<T>::value && std::is_integral<D>::value
        auto process()
        {
            static_assert(use_raw_packet == true or use_raw_packet == false, "use_raw_packet is a boolean value");
            auto rsb_before_head_infomation = RSB_HeadInfo<T>{};
            Common::read_head<T>(&rsb_before_head_infomation, rsb_before);
            auto rsg_patch_head_info = RSBPatchHeadExpand<D>{};
            read_rsb_patch_head<D>(&rsg_patch_head_info);
            auto rsb_before_head_section_byte = rsb_before->readBytes(rsb_before_head_infomation.file_offset, 0ull);
            Common::test_hash(rsb_before_head_section_byte, rsg_patch_head_info.md5_rsb_before);
            auto rsb_after_head_section_byte = std::vector<uint8_t>{};
            if (rsg_patch_head_info.rsb_need_patch)
            {
                rsb_after_head_section_byte = std::move(rsb_before_head_section_byte);
            }
            {
                auto rsb_patch_head_section_byte = rsb_patch->readBytes(rsg_patch_head_info.rsb_head_section_patch_size);
                rsb_after_head_section_byte = std::move(VCDiff::decode<std::size_t>(
                    reinterpret_cast<char const *>(rsb_before_head_section_byte.data()), rsb_before_head_section_byte.size(),
                    reinterpret_cast<char const *>(rsb_patch_head_section_byte.data()), rsb_patch_head_section_byte.size()));
            }
            result->writeBytes(rsb_after_head_section_byte);
            auto rsb_after_head_infomation = RSB_HeadInfo<T>{};
            Common::read_head<T>(&rsb_after_head_infomation, result);
            if (rsb_after_head_infomation.rsg_number != rsg_patch_head_info.rsg_number)
            {
                throw Exception(fmt::format("{}", Language::get("popcap.rsb_patch.decode.invalid_rsg_number")), std::source_location::current(), "process");
            }
            auto rsb_before_rsg_info_list = std::vector<Common::RSGInfo<D>>{};
            auto rsb_after_rsg_info_list = std::vector<Common::RSGInfo<D>>{};
            Common::read_rsg_info<T, D>(rsb_before, &rsb_before_head_infomation, &rsb_before_rsg_info_list);
            Common::read_rsg_info<T, D>(result, &rsb_after_head_infomation, &rsb_after_rsg_info_list);
            auto packet_before_subgroup_indexing = std::map<std::string, int>{};
            for (auto i : Range<int>(rsb_before_rsg_info_list.size()))
            {
                packet_before_subgroup_indexing.emplace(rsb_before_rsg_info_list[i].name, i);
            }
            for (auto i : Range<int>(rsb_after_rsg_info_list.size()))
            {
                auto packet_after_name = rsb_after_rsg_info_list[i].name;
                auto packet_before = std::vector<uint8_t>{};
                auto packet_after = std::vector<uint8_t>{};
                auto rsg_patch_subgroup_info = RSBPatchSubgroupInfo{};
                read_subgroup_info(&rsg_patch_subgroup_info);
                if (packet_after_name != rsg_patch_subgroup_info.packet_name)
                {
                    throw Exception(fmt::format("{}", Language::get("popcap.rsb_patch.decode.invalid_packet_name")), std::source_location::current(), "process");
                }
                if (packet_before_subgroup_indexing.contains(packet_after_name))
                {
                    auto packet_before_subgroup_index = packet_before_subgroup_indexing[packet_after_name];
                    packet_before = std::move(rsb_before->readBytes(rsb_before_rsg_info_list[packet_before_subgroup_index].rsg_length, rsb_before_rsg_info_list[packet_before_subgroup_index].rsg_pos));
                    if constexpr (use_raw_packet)
                    {
                        // to do
                    }
                }
                if (rsg_patch_subgroup_info.packet_patch_size > 0)
                {
                    auto rsb_patch_section_byte = rsb_patch->readBytes(rsg_patch_subgroup_info.packet_patch_size);
                    packet_after = std::move(VCDiff::decode<std::size_t>(
                        reinterpret_cast<char const *>(packet_before.data()), packet_before.size(),
                        reinterpret_cast<char const *>(rsb_patch_section_byte.data()), rsb_patch_section_byte.size()));
                }
                else
                {
                    packet_after = std::move(packet_before);
                }
                Common::test_hash(packet_after, rsg_patch_subgroup_info.md5_packet);
                result->writeBytes(packet_after);
            }
            if (rsg_patch_head_info.rsb_after_file_size != result->size())
            {
                throw Exception(fmt::format("{}", Language::get("popcap.rsb_patch.decode.unable_to_decode")), std::source_location::current(), "process");
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source_before,
            std::string_view source_patch,
            std::string_view destination
        ) -> void
        {
            auto encode = Decode{source_before, source_patch};
            encode.process<uint32_t, int>();
            encode.result->out_file(destination);
            return;
        }
    };

}