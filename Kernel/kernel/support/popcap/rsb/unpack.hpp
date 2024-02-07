#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/rsb/definition.hpp"

namespace Sen::Kernel::Support::PopCap::RSB
{
    using namespace Definition;

    class Unpack
    {
    protected:
        DataStreamView sen;

        inline auto unpack_rsb(
            std::string_view destination
        ) const -> void
        {
            auto rsb_headinfo = read_head();
            auto rsb_data_list = std::vector<RSB_Data>{};
            rsb_data_list.reserve(rsb_headinfo.composite_number);
            for (auto i : Range(rsb_headinfo.composite_number))
            {
                auto composite_start_pos = i * rsb_headinfo.composite_info_each_length + rsb_headinfo.composite_info_begin;
                auto composite_name = sen.readStringByEmpty(composite_start_pos);
                auto composite_info_pos = composite_start_pos + 0x80;
                for (auto k : Range(sen.readUint32(composite_start_pos + 0x480)))
                {
                    auto rsg_index = sen.readUint32(static_cast<size_t>(k) * 0x10 + composite_info_pos);
                    auto rsg_res = sen.readUint32();
                    auto rsb_data = read_rsg_info(rsg_index, rsb_headinfo);
                    rsb_data.rsg_res = rsg_res;
                    rsb_data.composite_name = composite_name;
                    rsb_data_list.emplace_back(std::move(rsb_data));
                }
            }
            write_file<true>(destination, rsb_data_list);
            return;
        }

        template <auto use_async>
        inline auto write_file(
            std::string_view destination,
            std::vector<RSB_Data> &rsb_data_list
        ) const -> void {
            if constexpr (use_async) {
                auto process = std::vector<std::future<void>>{};
                for (auto& rsb_data : rsb_data_list) {
                    process.push_back(std::async(std::launch::async, [&, destination] () {
                        FileSystem::write_binary(fmt::format("{}/{}.rsg", destination, rsb_data.name), rsb_data.data);
                    }));
                }
                for (auto& p : process) {
                    p.get(); 
                }
            }
            else {
                for (auto& rsb_data : rsb_data_list) {
                    FileSystem::write_binary(fmt::format("{}/{}.rsg", destination, rsb_data.name), rsb_data.data);
                }
            }
            return;
        }

        inline auto read_rsg_info(
            int rsg_index,
            const RSB_HeadInfo &rsb_headinfo) const -> RSB_Data
        {
            auto rsg_info_pos = rsg_index * rsb_headinfo.rsg_info_each_length + rsb_headinfo.rsg_info_begin;
            auto rsg_name = sen.readStringByEmpty(rsg_info_pos);
            auto packet_pos = sen.readUint32(static_cast<size_t>(rsg_info_pos) + 0x80);
            auto packet_size = sen.readUint32();
            auto packet_data = sen.getBytes(packet_pos, static_cast<size_t>(packet_pos) + packet_size);
            auto ptx_number = sen.readUint32(static_cast<size_t>(rsg_info_pos) + 0xC4);
            if (ptx_number > 0)
            {
                auto ptx_before = sen.readUint32();
                auto temp_sen = DataStreamView{std::move(packet_data)};
                temp_sen.writeUint32(ptx_number, 0x08);
                auto write_pos = temp_sen.readUint32(0x28) + temp_sen.readUint32();
                temp_sen.writeUint32(write_pos);
                auto ptx_info_pos = ptx_before * rsb_headinfo.ptx_info_each_length + rsb_headinfo.ptx_info_begin;
                temp_sen.writeBytes(sen.getBytes(ptx_info_pos, ptx_info_pos + static_cast<size_t>(ptx_number) * rsb_headinfo.ptx_info_each_length), write_pos);
                packet_data = temp_sen.toBytes();
            }
            auto rsb_data = RSB_Data{};
            rsb_data.name = rsg_name;
            rsb_data.data = std::move(packet_data);
            return rsb_data;
        }

        inline auto read_head() const -> RSB_HeadInfo
        {
            auto rsb_headinfo = RSB_HeadInfo{};
            {
                auto magic = sen.readString(4);
                if (magic != "1bsr")
                {
                    throw Exception("Invalid RSB head", std::source_location::current(), "read_head");
                }
            }
            rsb_headinfo.version = sen.readUint32();
            if (rsb_headinfo.version == 3)
            {
                throw Exception("Unsupported");
            }
            sen.readString(4);
            rsb_headinfo.file_offset = sen.readUint32();
            rsb_headinfo.file_list_length = sen.readUint32();
            rsb_headinfo.file_list_begin = sen.readUint32();
            sen.readString(8);
            rsb_headinfo.rsg_list_length = sen.readUint32();
            rsb_headinfo.rsg_list_begin = sen.readUint32();
            rsb_headinfo.rsg_number = sen.readUint32();
            rsb_headinfo.rsg_info_begin = sen.readUint32();
            rsb_headinfo.rsg_info_each_length = sen.readUint32();
            rsb_headinfo.composite_number = sen.readUint32();
            rsb_headinfo.composite_info_begin = sen.readUint32();
            rsb_headinfo.composite_info_each_length = sen.readUint32();
            rsb_headinfo.composite_list_length = sen.readUint32();
            rsb_headinfo.composite_list_begin = sen.readUint32();
            rsb_headinfo.autopool_number = sen.readUint32();
            rsb_headinfo.autopool_info_begin = sen.readUint32();
            rsb_headinfo.autopool_info_each_length = sen.readUint32();
            rsb_headinfo.ptx_number = sen.readUint32();
            rsb_headinfo.ptx_info_begin = sen.readUint32();
            rsb_headinfo.ptx_info_each_length = sen.readUint32();
            rsb_headinfo.part1_begin = sen.readUint32();
            rsb_headinfo.part2_begin = sen.readUint32();
            rsb_headinfo.part3_begin = sen.readUint32();
            if (rsb_headinfo.version >= 4)
            {
                rsb_headinfo.file_offset = sen.readUint32();
            }
            return rsb_headinfo;
        }

    public:
        explicit Unpack(
            std::string_view source) : sen(source)
        {
        }

        explicit Unpack(
            DataStreamView &it) : sen(it)
        {
        }

        ~Unpack(

            ) = default;

        inline static auto unpack_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto unpack = Unpack{ source };
            FileSystem::create_directory(destination);
            auto packet = fmt::format("{}/{}", destination, "packet"_sv);
            FileSystem::create_directory(packet);
            unpack.unpack_rsb(packet);
            return;
        }
    };
}