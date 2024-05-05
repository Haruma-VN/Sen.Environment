#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/pvz2/common.hpp"
#include "kernel/support/popcap/newton/decode.hpp"
#include "kernel/support/popcap/rton/decode.hpp"
#include "kernel/support/popcap/resource_group/convert.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2
{
    using namespace Definition;

    class Unpack
    {
    protected:
        std::unique_ptr<DataStreamView> stream;

        inline static auto packages_regex = std::regex("packages", std::regex_constants::icase);

        inline static auto manifestgroup_regex = std::regex("manifestgroup", std::regex_constants::icase);

    protected:
        inline auto read_head(
            RSB_HeadInfo<uint32_t> *rsb_headinfo) const -> void
        {
            {
                auto magic = stream->readString(4);
                if (magic != "1bsr"_sv)
                {
                    throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rsb.unpack.invalid_rsb_magic")), std::source_location::current(), "read_head");
                }
            }
            const auto version = stream->readUint32();
            if (version != 4)
            {
                throw Exception("rsb_must_be_version_4"); // TODO add localization.
            }
            rsb_headinfo->version = version;
            stream->read_pos += 4;
            rsb_headinfo->file_offset = stream->readUint32();
            rsb_headinfo->file_list_length = stream->readUint32();
            rsb_headinfo->file_list_begin = stream->readUint32();
            stream->read_pos += 8;
            rsb_headinfo->rsg_list_length = stream->readUint32();
            rsb_headinfo->rsg_list_begin = stream->readUint32();
            rsb_headinfo->rsg_number = stream->readUint32();
            rsb_headinfo->rsg_info_begin = stream->readUint32();
            rsb_headinfo->rsg_info_each_length = stream->readUint32();
            rsb_headinfo->composite_number = stream->readUint32();
            rsb_headinfo->composite_info_begin = stream->readUint32();
            rsb_headinfo->composite_info_each_length = stream->readUint32();
            rsb_headinfo->composite_list_length = stream->readUint32();
            rsb_headinfo->composite_list_begin = stream->readUint32();
            rsb_headinfo->autopool_number = stream->readUint32();
            rsb_headinfo->autopool_info_begin = stream->readUint32();
            rsb_headinfo->autopool_info_each_length = stream->readUint32();
            rsb_headinfo->ptx_number = stream->readUint32();
            rsb_headinfo->ptx_info_begin = stream->readUint32();
            rsb_headinfo->ptx_info_each_length = stream->readUint32();
            rsb_headinfo->part1_begin = stream->readUint32();
            rsb_headinfo->part2_begin = stream->readUint32();
            rsb_headinfo->part3_begin = stream->readUint32();
            if (rsb_headinfo->version >= 4)
            {
                rsb_headinfo->file_offset = stream->readUint32();
            }
            return;
        }

        inline auto process_packages(
            std::string_view destination,
            const SubgroupInfo<uint32_t> &group) const -> void
        {
            auto file_data = stream->readBytes(group.size, static_cast<std::size_t>(group.pos));
            auto packet_info = PacketInfo<uint32_t>{};
            Common::rsg_unpack(file_data, &packet_info);
            for (const auto &data : packet_info.res)
            {
                FileSystem::write_binary(fmt::format("{}/Resources/{}", destination, data.path), data.data);
            }
            return;
        }

        inline auto process_composite(
            std::string_view destination,
            const RSB_HeadInfo<uint32_t> &head_info,
            std::map<std::string, GroupInfo<uint32_t>> &group_list,
            bool &use_argb8888_for_ios) const -> std::string
        {
            auto manifest_name = std::string{};
            for (const auto &i : Range(head_info.composite_number))
            {
                const auto composite_start_pos = i * head_info.composite_info_each_length + head_info.composite_info_begin;
                auto composite_name = stream->readStringByEmpty(static_cast<std::size_t>(composite_start_pos));
                const auto composite_info_pos = composite_start_pos + 0x80;
                auto is_composite = !composite_name.ends_with("_CompositeShell"_sv);
                if (!is_composite)
                {
                    composite_name = composite_name.substr(0, composite_name.size() - 15);
                }
                
                for (const auto &k : Range(stream->readUint32(static_cast<std::size_t>(composite_start_pos + 0x480))))
                {
                    const auto rsg_index = stream->readUint32(static_cast<std::size_t>(k * 0x10 + composite_info_pos));
                    const auto rsg_info_pos = rsg_index * head_info.rsg_info_each_length + head_info.rsg_info_begin;
                    const auto rsg_name = stream->readStringByEmpty(static_cast<std::size_t>(rsg_info_pos));
                    //

                    const auto packet_pos = stream->readUint32(static_cast<std::size_t>(rsg_info_pos + 0x80));
                    const auto packet_size = stream->readUint32();
                    auto subgroup = SubgroupInfo<uint32_t>{
                        .pos = packet_pos,
                        .size = packet_size};
                    if (is_composite)
                    {
                        if (rsg_name.ends_with("_1536"_sv))
                        {
                            const auto ptx_before = stream->readUint32(static_cast<std::size_t>(rsg_info_pos + 0xC8));
                            const auto format_pos = (head_info.ptx_info_begin + (ptx_before)*head_info.ptx_info_each_length) + 0xC;
                            const auto format = stream->readUint32(static_cast<std::size_t>(format_pos));
                            if (format == 30 && !use_argb8888_for_ios || format == 148 && !use_argb8888_for_ios)
                            {
                                use_argb8888_for_ios = true;
                            }
                            subgroup.format = format;
                        }
                    }
                    group_list[composite_name].subgroup[rsg_name] = subgroup;
                }
                if (std::regex_match(composite_name, packages_regex))
                {
                    process_packages(destination, group_list[composite_name].subgroup[composite_name]);
                    group_list.erase(composite_name);
                    continue;
                }
                if (std::regex_search(composite_name, manifestgroup_regex))
                {
                    manifest_name = composite_name;
                    continue;
                }
            }
            return manifest_name;
        }

        inline auto process_manifest(
            const SubgroupInfo<uint32_t> &group) const -> nlohmann::ordered_json
        {

            auto file_data = stream->readBytes(group.size, static_cast<std::size_t>(group.pos));
            auto packet_info = PacketInfo<uint32_t>{};
            Common::rsg_unpack(file_data, &packet_info);
            auto use_newton = -1;
            for (const auto &i : Range(packet_info.res.size()))
            {
                if (packet_info.res[i].path.ends_with(".NEWTON"_sv))
                {
                    use_newton = i;
                    break;
                }
            }
            auto result = nlohmann::ordered_json{};
            if (use_newton != -1)
            {
                auto converter = ResourceGroup::Convert<true>{};
                auto view = Newton::Decode{packet_info.res.at(use_newton).data};
                result = converter.convert_whole(view.process());
            }
            else
            {
                auto converter = ResourceGroup::Convert<false>{};
                auto view = RTON::Decode{packet_info.res.at(0).data};
                result = converter.convert_whole(nlohmann::ordered_json::parse(view.decode_rton()));
            }
            return result;
        }

        inline auto process_group(const std::string &packet_folder,
                                  const std::map<std::string, GroupInfo<uint32_t>> &group_list,
                                  const std::string &manifest_name,
                                  bool use_argb8888_for_ios) const -> void
        {
            auto res_info = process_manifest(group_list.at(manifest_name).subgroup.at(manifest_name));
            for (const auto &[name, group] : res_info["groups"].items())
            {
                if (name == manifest_name) {
                    continue;
                }
                if (std::regex_match(name, packages_regex))
                {
                    continue;
                }
                auto data = DataStreamView{};
                data.writeString(data_magic);
                data.writeUint32(1);
                data.writeNull(12);
                auto num_subgroup = 0;
                for (const auto &[s_name, subgroup] : group["subgroup"].items())
                {
                    const auto &map_subgroup = group_list.at(name).subgroup.at(s_name);
                    if (group["is_composite"].get<bool>())
                    {
                        if (s_name.ends_with("_1536"_sv) || s_name.ends_with("_Common"_sv))
                        {
                            data.writeUint32(1);
                            data.writeUint32(map_subgroup.size);
                            data.writeUint32(data.write_pos + 16);
                            if (use_argb8888_for_ios && map_subgroup.format == 0)
                            {
                                data.writeUint32(0xFFFFFFFF); // argb8888.
                            }
                            else
                            {
                                data.writeUint32(map_subgroup.format);
                            }
                            const auto before_pos = data.write_pos;
                            data.writeNull(8);
                            data.writeBytes(stream->getBytes(map_subgroup.pos, map_subgroup.pos + map_subgroup.size));
                            const auto current_pos = data.write_pos;
                            auto subgroup_info = subgroup.dump();
                            data.writeUint32(subgroup_info.size(), before_pos);
                            data.writeUint32(current_pos);
                            data.writeString(subgroup_info, current_pos);
                            data.writeStringByEmpty(s_name);
                            num_subgroup++;
                        }
                    }
                    else
                    {
                        data.writeUint32(0);
                        data.writeUint32(map_subgroup.size);
                        data.writeUint32(data.write_pos + 12);
                        const auto before_pos = data.write_pos;
                        data.writeNull(8);
                        data.writeBytes(stream->getBytes(map_subgroup.pos, map_subgroup.pos + map_subgroup.size));
                        const auto current_pos = data.write_pos;
                        auto subgroup_info = subgroup.dump();
                        data.writeUint32(subgroup_info.size(), before_pos);
                        data.writeUint32(current_pos);
                        data.writeString(subgroup_info, current_pos);
                        data.writeStringByEmpty(s_name);
                        num_subgroup++;
                    }
                }
                data.writeUint32(num_subgroup, static_cast<std::size_t>(0x10));
                data.out_file(fmt::format("{}/{}.data", packet_folder, name)); // TODO change ext
            }
            return;
        }

        inline auto process(std::string_view destination) const -> void
        {
            auto head_info = RSB_HeadInfo<uint32_t>{};
            read_head(&head_info);
            auto group_list = std::map<std::string, GroupInfo<uint32_t>>{};
            auto use_argb8888_for_ios = false;
            const auto manifest_name = process_composite(destination, head_info, group_list, use_argb8888_for_ios);
            if (manifest_name.empty())
            {
                throw Exception("cannot_find_manifest"); // TODO add localization;
            }
            const auto packet_folder = fmt::format("{}/{}", destination, "Packet");
            FileSystem::create_directory(packet_folder);
            process_group(packet_folder, group_list, manifest_name, use_argb8888_for_ios);
            return;
        }

    public:
        explicit Unpack(
            std::string_view source) : stream(std::make_unique<DataStreamView>(source))
        {
        }

        explicit Unpack(
            const std::vector<uint8_t> &data) : stream(std::make_unique<DataStreamView>(data))
        {
        }

        ~Unpack(

            ) = default;

        inline auto process_whole(
            std::string_view destination
        ) const -> void
        {

            FileSystem::create_directory(destination);
            process(destination);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto unpack = Unpack{source};
            unpack.process_whole(destination);
            return;
        }
    };
}