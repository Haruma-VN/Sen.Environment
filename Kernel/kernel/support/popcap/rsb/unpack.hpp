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

        template <typename T>
            requires std::is_integral<T>::value
        inline auto read_description(
            RSB_HeadInfo<T>* rsb_head_info,
            Description* it
        ) const -> void
        {
            sen->read_pos = rsb_head_info->part1_begin;
            auto part2_pos = rsb_head_info->part2_begin;
            auto part3_pos = rsb_head_info->part3_begin;
            auto composite_resources_info = std::vector<CompositeResoucesDescriptionInfo>{};
            auto description_group_key = std::vector<std::string>{};
            for (auto i = 0; sen->read_pos < part2_pos; ++i) {
                auto id_part3_pos = sen->readInt32();
                auto id = sen->getStringByEmpty(part3_pos + id_part3_pos);
                auto rsg_number = sen->readInt32();
                auto subgroup = std::map<std::string, DescriptionSubGroup>{};
                auto subgroup_keys = std::vector<std::string>{};
                if (sen->readInt32() != 0x10) {
                    throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.invalid_rsg_number")), std::source_location::current(), "read_description");
                }
                auto rsg_info_list = std::vector<ResourcesRsgInfo>{};
                for (auto k : Range<int>(rsg_number)) {
                    auto resolution_ratio = sen->readInt32();
                    auto l_temp = sen->readUint32();
                    auto language = std::string{""};
                    if (l_temp != 0) {
                        language = sen->readString(static_cast<std::uint64_t>(4), static_cast<std::uint64_t>(sen->read_pos - 4));
                    }
                    auto rsg_id_part3_pos = sen->readInt32();
                    auto resources_number = sen->readInt32();
                    auto resources_info_list = std::vector<ResourcesInfo>{};
                    for (auto l : Range(resources_number)) {
                        auto info_part2_pos = sen->readInt32();
                        auto resources_info = ResourcesInfo{info_part2_pos};
                        resources_info_list.emplace_back(resources_info);
                    }
                    auto rsg_id = sen->getStringByEmpty(part3_pos + rsg_id_part3_pos);
                    auto des_subgroup = DescriptionSubGroup{ std::to_string(resolution_ratio), language};
                    subgroup.emplace(rsg_id, des_subgroup);
                    subgroup_keys.emplace_back(rsg_id);
                    auto res_rsg_info = ResourcesRsgInfo{
                        resolution_ratio,
                        language,
                        rsg_id,
                        resources_number,
                        resources_info_list
                    };
                    rsg_info_list.emplace_back(res_rsg_info);
                }
                auto des_group = DescriptionGroup{
                    !id.ends_with("_CompositeShell"),
                    subgroup
                };
                it->groups.emplace(id, des_group);
                description_group_key.emplace_back(id);
                auto c_res_info = CompositeResoucesDescriptionInfo{ id, rsg_number,rsg_info_list};
                composite_resources_info.emplace_back(c_res_info);
                auto thiz_pos = sen->read_pos;
                auto resources_rsg_number = composite_resources_info[i].rsg_number;
                for (auto k : Range<int>(resources_rsg_number)) {
                    auto resources_number = composite_resources_info[i].rsg_info_list[k].resources_number;
                    for (auto h : Range<int>(resources_number)) {
                        sen->read_pos = part2_pos + composite_resources_info[i].rsg_info_list[k].resources_info_list[h].info_part2_pos;
                        if (sen->readInt32() != 0x0) {
                            throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.invalid_part2_position")), std::source_location::current(), "read_description");
                        }
                        auto type = sen->readUint16();
                        if (sen->readUint16() != 0x1C) {
                            throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.invalid_head_length")), std::source_location::current(), "read_description");
                        }
                        auto ptx_info_part2_end_pos = sen->readInt32();
                        auto ptx_info_part2_begin_pos = sen->readInt32();
                        auto res_id_part3_pos = sen->readInt32();
                        auto path_part3_pos = sen->readInt32();
                        auto res_id = sen->getStringByEmpty(part3_pos + res_id_part3_pos);
                        auto res_path = sen->getStringByEmpty(part3_pos + path_part3_pos);
                        res_path = std::regex_replace(res_path, std::regex("\\\\"), "/");
                        auto properties_num = sen->readInt32();
                        auto ptx_info_list = PropertiesPtxInfo{};
                        if (ptx_info_part2_end_pos * ptx_info_part2_begin_pos != 0) {
                            ptx_info_list.imagetype = std::to_string(sen->readUint16());
                            ptx_info_list.aflags = std::to_string(sen->readUint16());
                            ptx_info_list.x = std::to_string(sen->readUint16());
                            ptx_info_list.y = std::to_string(sen->readUint16());
                            ptx_info_list.ax = std::to_string(sen->readUint16());
                            ptx_info_list.ay = std::to_string(sen->readUint16());
                            ptx_info_list.aw = std::to_string(sen->readUint16());
                            ptx_info_list.ah = std::to_string(sen->readUint16());
                            ptx_info_list.rows = std::to_string(sen->readUint16());
                            ptx_info_list.cols = std::to_string(sen->readUint16());
                            ptx_info_list.parent = sen->getStringByEmpty(part3_pos + sen->readInt32());
                        }
                        auto properties_info_list = std::map<string, string>{};
                        for (auto l : Range<T>(properties_num)) {
                            auto key_part3_pos = sen->readInt32();
                            if (sen->readInt32() != 0x0) {
                                throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.rsb_is_corrupted")), std::source_location::current(), "read_description");
                            }
                            auto value_part3_pos = sen->readInt32();
                            auto key = sen->getStringByEmpty(part3_pos + key_part3_pos);
                            auto value = sen->getStringByEmpty(part3_pos + value_part3_pos);
                            properties_info_list.emplace(std::move(key), std::move(value));
                        }
                        it->groups[description_group_key[i]].subgroups[subgroup_keys[k]].resources.insert(std::pair{res_id, DescriptionResources{
                            std::move(std::to_string(type)),
                            std::move(res_path),
                            std::move(ptx_info_list),
                            std::move(properties_info_list),
                        }});
                    }
                }
                sen->read_pos = thiz_pos;
            }
            return;
        }

        template <typename T, typename U>
            requires std::is_integral<T>::value &&
                     std::is_integral<U>::value
        inline auto read_rsg_category(
            T version,
            RSG_Info<U> &rsg_info) const -> void
        {
            auto res_1 = sen->readUint32();
            auto res_2 = sen->readUint32();
            if (res_2 == 0)
            {
                rsg_info.category = nlohmann::ordered_json::array({res_1, nullptr});
            }
            else
            {
                sen->read_pos -= 4;
                rsg_info.category = nlohmann::ordered_json::array({res_1, sen->readString(4)});
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
        template <typename T, typename U, auto write_rsg>
            requires std::is_integral<T>::value && std::is_integral<U>::value
        inline auto read_rsg_info(
            T rsg_index,
            const RSB_HeadInfo<U> &rsb_head_info,
            T rsg_info_pos,
            RSG_Info<U> &rsg_info,
            std::string_view rsg_name,
            std::string_view packet_folder
            ) const -> void
        {
            auto packet_pos = sen->readUint32(static_cast<std::uint64_t>(rsg_info_pos + 0x80));
            auto packet_size = sen->readUint32();
            auto packet_sen = DataStreamView{sen->getBytes(packet_pos, static_cast<std::uint64_t>(packet_pos) + packet_size)};
            rsg_info.packet_info = RSG_PacketInfo{packet_sen.readUint32(static_cast<std::uint64_t>(0x10))};
            auto ptx_before = sen->readUint32(static_cast<std::uint64_t>(rsg_info_pos + 0xC8));
            read_res_info<int>(packet_sen, rsb_head_info, ptx_before, &rsg_info.packet_info.res);
            if constexpr (write_rsg)
            {
                packet_sen.out_file(fmt::format("{}/{}.rsg", packet_folder, rsg_name));
            }
            return;
        }

        template <typename T, typename U>
            requires std::is_integral<T>::value && std::is_integral<U>::value
        inline auto read_res_info(
            const DataStreamView &packet_sen,
            const RSB_HeadInfo<U> &rsb_head_info,
            T ptx_before,
            std::vector<nlohmann::ordered_json> *res_list) const -> void
        {
            auto name_dist_list = std::vector<NameDict<std::uint32_t>>{};
            auto name_path = std::string{};
            // read rsg_file_list_length;
            auto file_list_lengh = packet_sen.readUint32(static_cast<std::uint64_t>(0x48));
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
                        name_dist_list.emplace_back(NameDict<std::uint32_t>{
                            name_path,
                            pos,
                        });
                    }
                    auto is_atlas = packet_sen.readUint32() == 1;
                    auto res = nlohmann::ordered_json{
                        {"path", name_path}};
                    packet_sen.read_pos += 8;
                    if (is_atlas)
                    {
                        auto id = packet_sen.readUint32();
                        packet_sen.read_pos += 8;
                        res["ptx_info"] = {
                            {"id", id},
                            {"width", packet_sen.readUint32()},
                            {"height", packet_sen.readUint32()}};
                        auto pitch_pos = (rsb_head_info.ptx_info_begin + (ptx_before - id) * rsb_head_info.ptx_info_each_length) + 0x08;
                        res["ptx_info"]["pitch"] = sen->readUint32(static_cast<std::uint64_t>(pitch_pos));
                        res["ptx_info"]["format"] = sen->readUint32();
                        if (rsb_head_info.ptx_info_each_length >= 0x14) {
                            res["ptx_info"]["alpha_size"] = sen->readUint32();
                            res["ptx_info"]["alpha_format"] = rsb_head_info.ptx_info_each_length == 0x18 ? sen->readUint32() : (res["ptx_info"]["alpha_size"] == 0 ? 0 : 0x64);
                        } 
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
                    if (char_byte == 0x5C)
                    {
                        char_byte = 0x2F;
                    }
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

        template <typename T>
            requires std::is_integral<T>::value && std::is_unsigned<T>::value
        inline auto read_head(
            RSB_HeadInfo<T> *rsb_headinfo
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

        template <typename T, auto write_info = false>
            requires std::is_integral<T>::value
        inline auto process(
            std::string_view destination,
            Manifest<T> &manifest_info
        ) const -> void
        {
            auto rsb_head_info = RSB_HeadInfo<T>{};
            read_head(&rsb_head_info);
            manifest_info.version = rsb_head_info.version;
            manifest_info.ptx_info_size = rsb_head_info.ptx_info_each_length;
            auto packet_folder = fmt::format("{}/{}", destination, "packet");
            FileSystem::create_directory(destination);
            FileSystem::create_directory(packet_folder);
            if (rsb_head_info.version == 3)
            {
                if (rsb_head_info.part1_begin == 0 && rsb_head_info.part2_begin == 0 && rsb_head_info.part3_begin == 0)
                {
                    throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.invalid_version_resources_pos")), std::source_location::current(), "process");
                }
                auto description = Description{};
                read_description(&rsb_head_info, &description);
                FileSystem::write_json(fmt::format("{}/{}", destination, "description.json"), description);
            }
            for (auto i : Range(rsb_head_info.composite_number))
            {
                auto composite_start_pos = i * rsb_head_info.composite_info_each_length + rsb_head_info.composite_info_begin;
                auto composite_name = sen->readStringByEmpty(static_cast<std::uint64_t>(composite_start_pos));
                auto composite_info_pos = composite_start_pos + 0x80;
                auto is_composite = composite_name.ends_with("_CompositeShell");
                if (is_composite)
                {
                    composite_name = composite_name.substr(0, composite_name.size() - 15);
                }
                auto rsg_group = RSG_Group<T>{!is_composite};
                for (auto k : Range(sen->readUint32(static_cast<std::uint64_t>(composite_start_pos + 0x480))))
                {
                    auto rsg_info = RSG_Info<T>{};
                    auto rsg_index = sen->readUint32(static_cast<std::uint64_t>(k * 0x10 + composite_info_pos));
                    read_rsg_category<T, T>(rsb_head_info.version, rsg_info);
                    auto rsg_info_pos = rsg_index * rsb_head_info.rsg_info_each_length + rsb_head_info.rsg_info_begin;
                    auto rsg_name = sen->readStringByEmpty(static_cast<std::uint64_t>(rsg_info_pos));
                    read_rsg_info<std::int32_t, T, true>(rsg_index, rsb_head_info, rsg_info_pos, rsg_info, rsg_name, packet_folder);
                    rsg_group.subgroup.insert(std::pair{std::move(rsg_name), std::move(rsg_info)});
                }
                manifest_info.group.insert(std::pair{std::move(composite_name), std::move(rsg_group)});
            }
            if constexpr (write_info)
            {
                FileSystem::write_json(fmt::format("{}/{}", destination, "manifest.json"), std::move(manifest_info));
            }
            return;
        }

        inline static auto unpack_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto unpack = Unpack{source};
            auto manifest = Manifest<std::uint32_t>{};
            unpack.process<std::uint32_t, true>(destination, manifest);
            return;
        }
    };
}