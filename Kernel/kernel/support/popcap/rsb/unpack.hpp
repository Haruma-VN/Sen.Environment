#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/rsb/definition.hpp"

namespace Sen::Kernel::Support::PopCap::RSB
{
    using namespace Definition;

    class Unpack
    {
    protected:
        std::unique_ptr<DataStreamView> sen;

        inline auto process(
            std::string_view destination
        ) const -> void
        {
            auto rsb_head_info = RSB_HeadInfo{};
            read_head(&rsb_head_info);
            auto manifest_info = Manifest{};
            manifest_info.version = rsb_head_info.version;
            auto packet_folder = fmt::format("{}/{}", destination, "packet");
            FileSystem::create_directory(destination);
            FileSystem::create_directory(packet_folder);
            if (rsb_head_info.version == 3) {
                if (rsb_head_info.part1_begin == 0 && rsb_head_info.part2_begin == 0 && rsb_head_info.part3_begin == 0) {
                    throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.invalid_version_resources_pos")), std::source_location::current(), "process");
                }
            }
            for (auto i : Range(rsb_head_info.composite_number))
            {
                auto composite_start_pos = i * rsb_head_info.composite_info_each_length + rsb_head_info.composite_info_begin;
                auto composite_name = sen->readStringByEmpty(composite_start_pos);
                auto composite_info_pos = composite_start_pos + 0x80;
                auto is_composite = composite_name.ends_with("_CompositeShell");
                auto rsg_group = RSG_Group{is_composite};
                for (auto k : Range(sen->readUint32(composite_start_pos + 0x480)))
                {
                    auto rsg_info = RSG_Info{};
                    auto rsg_index = sen->readUint32(static_cast<size_t>(k) * 0x10 + composite_info_pos);
                    read_rsg_category<int>(rsb_head_info.version, rsg_info);
                    auto rsg_info_pos = rsg_index * rsb_head_info.rsg_info_each_length + rsb_head_info.rsg_info_begin;
                    auto rsg_name = sen->readStringByEmpty(rsg_info_pos);
                    read_rsg_info<int>(rsg_index, rsb_head_info, rsg_info_pos, rsg_info, rsg_name, packet_folder);
                    rsg_group.subgroup.insert(std::pair{rsg_name, rsg_info});
                }
                manifest_info.group.insert(std::pair{composite_name, rsg_group});
            }
            FileSystem::write_json(fmt::format("{}/{}", destination, "manifest.json"), manifest_info);
            return;
        }

        inline auto read_description(
            const RSB_HeadInfo &rsb_head_info
        ) const -> void 
        {
            sen->read_pos = rsb_head_info.part1_begin;
            auto part2_pos = rsb_head_info.part2_begin;
            auto part3_pos = rsb_head_info.part3_begin;
            return;
        }

        template <typename T> requires std::is_integral<T>::value
        inline auto read_rsg_category(
            T version, 
            RSG_Info &rsg_info
        ) const -> void
        {
            if (version == 3)
            {
                auto res_1 = sen->readString(4);
                auto res_2 = sen->readString(4);
                rsg_info.category = nlohmann::ordered_json::array({res_1, res_2});
            }
            else
            {
                auto res = sen->readUint32(4);
                if (res != 0)
                {
                    rsg_info.category = nlohmann::ordered_json::array({res, nullptr});
                }
                else {
                    rsg_info.category = nlohmann::ordered_json::array({nullptr, nullptr});
                }
            }
            return;
        }
        /*
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
        */
        template <typename T> requires std::is_integral<T>::value
        inline auto read_rsg_info(
            T rsg_index,
            const RSB_HeadInfo &rsb_head_info,
            T rsg_info_pos,
            RSG_Info &rsg_info,
            std::string_view rsg_name,
            std::string_view packet_folder
        ) const -> void
        {
            auto packet_pos = sen->readUint32(static_cast<size_t>(rsg_info_pos) + 0x80);
            auto packet_size = sen->readUint32();
            auto packet_sen = DataStreamView{sen->getBytes(packet_pos, static_cast<size_t>(packet_pos) + packet_size)};
            rsg_info.packet_info = RSG_PacketInfo{packet_sen.readUint32(0x10)};
            auto ptx_number_pool = sen->readUint32(static_cast<size_t>(rsg_info_pos) + 0xC4) + sen->readUint32();
            read_res_info<int>(packet_sen, rsb_head_info, ptx_number_pool, &rsg_info.packet_info.res);
            packet_sen.out_file(fmt::format("{}/{}.rsg", packet_folder, rsg_name));
            return;
        }

        template <typename T> requires std::is_integral<T>::value
        inline auto read_res_info(
            const DataStreamView &packet_sen,
            const RSB_HeadInfo &rsb_head_info,
            T ptx_number_pool,
            std::vector<RSG_ResInfo>* res_list
        ) const -> void
        {
            auto name_dist_list = std::vector<NameDict>{};
            auto name_path = std::string{};
            // read rsg_file_list_length;
            auto file_list_lengh = packet_sen.readUint32(0x48);
            // read rsg_file_list_pos;
            auto file_list_pos = packet_sen.readUint32();
            packet_sen.read_pos = file_list_pos;
            auto offset_limit = file_list_pos + file_list_lengh;
            while (packet_sen.read_pos < offset_limit)
            {
                auto char_byte = packet_sen.readUint8();
                auto pos = packet_sen.readUint24() * 4;
                if (char_byte == 0)
                {
                    if (pos != 0)
                    {
                        name_dist_list.emplace_back(NameDict{
                            name_path,
                            pos,
                        });
                    }
                    auto is_atlas = packet_sen.readUint32() == 1;
                    auto res = RSG_ResInfo{name_path};
                    packet_sen.read_pos += 8;
                    if (is_atlas)
                    {
                        auto id = packet_sen.readUint32();
                        packet_sen.read_pos += 8;
                        res.ptx_info = RSG_PTXInfo{id, packet_sen.readUint32(), packet_sen.readUint32()};
                        auto format_pos = (rsb_head_info.ptx_info_begin + (ptx_number_pool - id) * rsb_head_info.ptx_info_each_length) + 0x0C;
                        res.ptx_info.format = sen->readUint32(format_pos);
                    }
                    res_list->emplace_back(res);
                    for (auto i : Range(name_dist_list.size()))
                    {
                        if (name_dist_list[i].pos + static_cast<unsigned long long>(file_list_pos) == packet_sen.read_pos)
                        {
                            name_path = name_dist_list[i].name_path;
                            name_dist_list.erase(name_dist_list.begin() + i);
                            break;
                        }
                    }
                }
                else
                {
                    if (pos != 0)
                    {
                        name_dist_list.emplace_back(NameDict{
                            name_path,
                            pos});
                        name_path += static_cast<char>(char_byte);
                    }
                    else
                    {
                        name_path += static_cast<char>(char_byte);
                    }
                }
            }
            return;
        }

        inline auto read_head(
            RSB_HeadInfo* rsb_headinfo
        ) const -> void
        {
            {
                auto magic = sen->readString(4);
                if (magic != "1bsr")
                {
                    throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.invalid_rsb_magic")), std::source_location::current(), "read_head");
                }
            }
            rsb_headinfo->version = sen->readUint32();
            sen->read_pos += 4;
            rsb_headinfo->file_offset = sen->readUint32();
            rsb_headinfo->file_list_length = sen->readUint32();
            rsb_headinfo->file_list_begin = sen->readUint32();
            sen->read_pos += 8;
            rsb_headinfo->rsg_list_length = sen->readUint32();
            rsb_headinfo->rsg_list_begin = sen->readUint32();
            rsb_headinfo->rsg_number = sen->readUint32();
            rsb_headinfo->rsg_info_begin = sen->readUint32();
            rsb_headinfo->rsg_info_each_length = sen->readUint32();
            rsb_headinfo->composite_number = sen->readUint32();
            rsb_headinfo->composite_info_begin = sen->readUint32();
            rsb_headinfo->composite_info_each_length = sen->readUint32();
            rsb_headinfo->composite_list_length = sen->readUint32();
            rsb_headinfo->composite_list_begin = sen->readUint32();
            rsb_headinfo->autopool_number = sen->readUint32();
            rsb_headinfo->autopool_info_begin = sen->readUint32();
            rsb_headinfo->autopool_info_each_length = sen->readUint32();
            rsb_headinfo->ptx_number = sen->readUint32();
            rsb_headinfo->ptx_info_begin = sen->readUint32();
            rsb_headinfo->ptx_info_each_length = sen->readUint32();
            rsb_headinfo->part1_begin = sen->readUint32();
            rsb_headinfo->part2_begin = sen->readUint32();
            rsb_headinfo->part3_begin = sen->readUint32();
            if (rsb_headinfo->version >= 4)
            {
                rsb_headinfo->file_offset = sen->readUint32();
            }
            return;
        }

    public:
        explicit Unpack(
            std::string_view source
        ) : sen(std::make_unique<DataStreamView>(source))
        {
        }

        explicit Unpack(
            DataStreamView &it
        ) : sen(&it)
        {
        }

        ~Unpack(

        ) = default;

        inline static auto unpack_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto unpack = Unpack{source};
            unpack.process(destination);
            return;
        }
    };
}