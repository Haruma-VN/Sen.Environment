#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/rsb/definition.hpp"
#include "kernel/support/popcap/rsb/unpack.hpp"
#include "kernel/support/popcap/rsb_patch/common.hpp"

namespace Sen::Kernel::Support::PopCap::RSBPatch
{

    using namespace Sen::Kernel::Support::PopCap::RSB;

    using namespace Definition;

    class Encode
    {
    private:
        
        using VCDiff = Diff::VCDiff;
        std::unique_ptr<DataStreamView> rsb_before;
        std::unique_ptr<DataStreamView> rsb_after;
        std::unique_ptr<DataStreamView> result;

    protected:
    public:
        explicit Encode(
            std::string_view source_before,
            std::string_view source_after) : rsb_before(std::make_unique<DataStreamView>(source_before)),
                                             rsb_after(std::make_unique<DataStreamView>(source_after)), result(std::make_unique<DataStreamView>())
        {
        }

        ~Encode(

            ) = default;

         template <typename T, typename D, auto use_raw_packet = false>
        requires std::is_integral<T>::value && std::is_unsigned<T>::value && std::is_integral<D>::value
        auto process(

        )
        {
            static_assert(use_raw_packet == true or use_raw_packet == false, "use_raw_packet is a boolean value");
            auto rsb_before_head_infomation = RSB_HeadInfo<T>{};
            Common::read_head<T>(&rsb_before_head_infomation, rsb_before);
            auto rsb_after_head_infomation = RSB_HeadInfo<T>{};
            Common::read_head<T>(&rsb_after_head_infomation, rsb_after);
            if (rsb_before_head_infomation.version != 4 || rsb_after_head_infomation.version != 4)
            {
                throw Exception(fmt::format("{}", Language::get("popcap.rsb_patch.encode.only_support_pvz2")), std::source_location::current(), "process");
            }
            auto rsb_before_section_byte = rsb_before->readBytes(rsb_before_head_infomation.file_offset, 0ull);
            auto rsb_after_section_byte = rsb_after->readBytes(rsb_after_head_infomation.file_offset, 0ull);
            auto md5_rsb_old = Encryption::MD5::hash(rsb_before_section_byte);
            Common::test_hash(rsb_after_section_byte, md5_rsb_old);
            auto information_section_patch_exist = !std::equal(rsb_before_section_byte.begin(), rsb_before_section_byte.end(), rsb_after_section_byte.begin());

            result->writeString("pbsr"_sv);
            result->writeInt32(1);
            result->writeInt32(2);
            result->writeInt32(rsb_after->size());
            result->writeNull(8);
            result->writeBytes(Common::to_bytes(md5_rsb_old));
            result->writeInt32(rsb_after->readInt32(0x28ull));
            result->writeInt32(0);
            if (information_section_patch_exist)
            {
                auto rsb_head_vcdiff = VCDiff::encode<std::size_t, VCDiff::Flag::VCD_STANDARD_FORMAT>(
                    reinterpret_cast<char const *>(rsb_before_section_byte.data()), rsb_before_section_byte.size(),
                    reinterpret_cast<char const *>(rsb_after_section_byte.data()), rsb_after_section_byte.size());
                auto backup_write_pos = result->write_pos;
                result->writeInt32(rsb_head_vcdiff.size(), 0x14ull);
                result->writeInt32(1, 0x2Cull);
                result->write_pos = backup_write_pos;
                result->writeBytes(rsb_head_vcdiff);
            }
            auto rsb_before_rsg_info_list = std::vector<Common::RSGInfo<D>>{};
            auto rsb_after_rsg_info_list = std::vector<Common::RSGInfo<D>>{};
            Common::read_rsg_info<T, D>(rsb_before, &rsb_before_head_infomation, &rsb_before_rsg_info_list);
            Common::read_rsg_info<T, D>(rsb_after, &rsb_after_head_infomation, &rsb_after_rsg_info_list);
            auto packet_before_subgroup_indexing = std::map<std::string, int>{};
            for (auto i : Range<int>(rsb_before_rsg_info_list.size()))
            {
                packet_before_subgroup_indexing.emplace(rsb_before_rsg_info_list[i].name, i);
            }
            for (auto i : Range<int>(rsb_after_rsg_info_list.size()))
            {
                auto packet_after_name = rsb_after_rsg_info_list[i].name;
                auto packet_before = std::vector<uint8_t>{};
                if (packet_before_subgroup_indexing.contains(packet_after_name))
                {
                    auto packet_before_subgroup_index = packet_before_subgroup_indexing[packet_after_name];
                    if constexpr (use_raw_packet)
                    {
                        // auto rsg_pos = rsb_before_rsg_info_list[packet_before_subgroup_index].rsg_pos;
                        // auto packet_before_raw_size = rsb_before->readInt32(rsg_pos + 0x20ull) + rsb_before->readInt32(rsg_pos + 0x30ull) + rsb_before->readInt32(rsg_pos + 0x48ull);
                    }
                    else
                    {
                        packet_before = rsb_before->readBytes(rsb_before_rsg_info_list[packet_before_subgroup_index].rsg_length, rsb_before_rsg_info_list[packet_before_subgroup_index].rsg_pos);
                    }
                }
                auto packet_after = std::vector<uint8_t>{};
                if constexpr (use_raw_packet)
                {
                }
                else
                {
                    packet_after = rsb_after->readBytes(rsb_after_rsg_info_list[i].rsg_length, rsb_after_rsg_info_list[i].rsg_pos);
                }
                result->writeNull(8);
                result->writeString(packet_after_name);
                result->writeNull(0x80 - packet_after_name.size());
                result->writeBytes(Common::to_bytes(Encryption::MD5::hash(packet_after)));
                if (!std::equal(packet_before.begin(), packet_before.end(), packet_after.begin()))
                {
                    auto subgroup_vcdiff = VCDiff::encode<std::size_t, VCDiff::Flag::VCD_STANDARD_FORMAT>(
                        reinterpret_cast<char const *>(packet_before.data()), packet_before.size(),
                        reinterpret_cast<char const *>(packet_after.data()), packet_after.size());
                    auto pos = result->write_pos;
                    result->writeInt32(subgroup_vcdiff.size(), pos - 148ull);
                    result->writeBytes(subgroup_vcdiff);
                }
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source_before,
            std::string_view source_after,
            std::string_view destination
        ) -> void
        {
            auto encode = Encode{source_before, source_after};
            encode.process<uint32_t, int>();
            encode.result->out_file(destination);
            return;
        }
    };
}