#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/rsb/definition.hpp"
#include "kernel/support/popcap/rsb/unpack.hpp"

namespace Sen::Kernel::Support::PopCap::RSBPatch
{
    using namespace Sen::Kernel::Support::PopCap::RSB;
    using namespace Definition;

    struct Common
    {
    public:
        Common(

            ) = default;

        /**
         * Destructor
         */

        ~Common(

            ) = default;

        template <typename T>
            requires std::is_integral<T>::value
        struct RSGInfo
        {
        public:
            std::string name;
            uint64_t rsg_pos;
            T rsg_length;
            T pool_index;
            T ptx_number;
            T ptx_before_number;

            explicit RSGInfo(
                const std::string &name,
                uint64_t rsg_pos,
                T rsg_length,
                T pool_index,
                T ptx_number,
                T ptx_before_number) : name(name), rsg_pos(rsg_pos), rsg_length(rsg_length), pool_index(pool_index),
                                       ptx_number(ptx_number), ptx_before_number(ptx_before_number)
            {
            }

            ~RSGInfo() = default;
        };
        template <typename T>
            requires std::is_integral<T>::value && std::is_unsigned<T>::value
        auto static read_head(
            RSB_HeadInfo<T> *rsb_headinfo,
            DataStreamView* data_stream
        ) -> void
        {
            {
                auto magic = data_stream->readString(4);
                if (magic != "1bsr")
                {
                    throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.invalid_rsb_magic")), std::source_location::current(), "read_head");
                }
            }
            rsb_headinfo->version = data_stream->readUint32();
            data_stream->read_pos += 4;
            rsb_headinfo->file_offset = data_stream->readUint32();
            rsb_headinfo->file_list_length = data_stream->readUint32();
            rsb_headinfo->file_list_begin = data_stream->readUint32();
            data_stream->read_pos += 8;
            rsb_headinfo->rsg_list_length = data_stream->readUint32();
            rsb_headinfo->rsg_list_begin = data_stream->readUint32();
            rsb_headinfo->rsg_number = data_stream->readUint32();
            rsb_headinfo->rsg_info_begin = data_stream->readUint32();
            rsb_headinfo->rsg_info_each_length = data_stream->readUint32();
            rsb_headinfo->composite_number = data_stream->readUint32();
            rsb_headinfo->composite_info_begin = data_stream->readUint32();
            rsb_headinfo->composite_info_each_length = data_stream->readUint32();
            rsb_headinfo->composite_list_length = data_stream->readUint32();
            rsb_headinfo->composite_list_begin = data_stream->readUint32();
            rsb_headinfo->autopool_number = data_stream->readUint32();
            rsb_headinfo->autopool_info_begin = data_stream->readUint32();
            rsb_headinfo->autopool_info_each_length = data_stream->readUint32();
            rsb_headinfo->ptx_number = data_stream->readUint32();
            rsb_headinfo->ptx_info_begin = data_stream->readUint32();
            rsb_headinfo->ptx_info_each_length = data_stream->readUint32();
            rsb_headinfo->part1_begin = data_stream->readUint32();
            rsb_headinfo->part2_begin = data_stream->readUint32();
            rsb_headinfo->part3_begin = data_stream->readUint32();
            if (rsb_headinfo->version >= 4)
            {
                rsb_headinfo->file_offset = data_stream->readUint32();
            }
            return;
        }

        template <typename T, typename D>
            requires std::is_integral<T>::value && std::is_unsigned<T>::value && std::is_integral<D>::value
         auto static read_rsg_info(
            DataStreamView* data_stream,
            RSB_HeadInfo<T>* rsb_head,
            std::vector<RSGInfo<D>> *info
        ) -> void
        {
            data_stream->read_pos = rsb_head->rsg_info_begin;
            for (auto i : Range<int>(rsb_head->rsg_number))
            {
                auto start_pos = data_stream->read_pos;
                auto packet_name = data_stream->readStringByEmpty();
                data_stream->read_pos = start_pos + 128;
                auto rsg_pos = static_cast<std::uint64_t>(data_stream->readInt32());
                auto rsg_length = data_stream->readInt32();
                auto rsg_index = data_stream->readInt32();
                auto ptx_number = data_stream->readInt32(static_cast<std::size_t>(start_pos + rsb_head->rsg_info_each_length));
                auto ptx_before_number = data_stream->readInt32();
                info->emplace_back(RSGInfo<D>{packet_name, rsg_pos, rsg_length, rsg_index, ptx_number, ptx_before_number});
            }
            return;
        }

        inline auto static test_hash(
            const std::vector<uint8_t> &bytes_test,
            const std::string &md5_str
        ) -> void
        {
            auto md5_rsb_new = Encryption::MD5::hash(bytes_test);
            assert_conditional(md5_rsb_new == md5_str, fmt::format("{}", Language::get("popcap.rsb_patch.invalid_md5")), "test_hash");
            return;
        }

        inline auto static to_bytes(
            const std::string &md5
        ) -> std::vector<std::uint8_t>
        {
            auto byte_array = std::vector<uint8_t>{};
            for (auto i : Range<int>(0, md5.size(), 2))
            {
                auto byte_str = md5.substr(i, 2);
                byte_array.push_back(static_cast<std::uint8_t>(std::stoi(byte_str, nullptr, 16)));
            }
            return byte_array;
        }

        inline auto static to_string(
            const std::vector<uint8_t> &byte_array
        ) -> std::string
        {
            auto ss = std::stringstream{};
            ss << std::hex << std::setfill('0');
            for (auto i : Range<int>(byte_array.size()))
            {
                ss << std::hex << std::setw(2) << static_cast<int>(byte_array[i]);
            }

            return ss.str();
        }
    };
}