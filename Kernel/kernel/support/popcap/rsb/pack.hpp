#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/rsb/definition.hpp"

namespace Sen::Kernel::Support::PopCap::RSB
{
    using namespace Definition;

    template <typename T>
    concept BooleanArgument = true;

    struct PathList
    {
    public:
        std::string path;
        int pool_index;

        explicit PathList(
            const std::string &path,
            int pool_index) : path(path), pool_index(pool_index)
        {
        }

        ~PathList(

         ) = default;
    };

    struct PathPosition
    {
        int position;
        int offset;
    };

    struct PathTemp
    {
    public:
        std::string path_slice;
        int key;
        int pool_index;
        std::vector<PathPosition> positions;

        explicit PathTemp(
            const std::string &path_slice,
            int key,
            int pool_index
        ) : path_slice(path_slice), key(key), pool_index(pool_index)
        {
        }

        ~PathTemp(

        ) = default;
    };

    class Pack
    {
    public:

        Pack(

        ) : sen(std::make_shared<DataStreamView>())
        {

        }

        Pack(
            DataStreamView& that
        ) : sen(&that)
        {

        }

        ~Pack(

        ) = default;

        Pack(
            Pack&& that
        ) = delete;

        auto operator =(
            Pack&& that
        )->Pack & = delete;

    private:

        std::shared_ptr<DataStreamView> sen;

    protected:
        inline auto write_head(
            const RSB_HeadInfo<uint32_t> &head_info
        ) const -> void
        {
            sen->writeUint32(head_info.file_offset, 12ull);
            sen->writeUint32(head_info.file_list_length);
            sen->writeUint32(head_info.file_list_begin);
            sen->writeUint32(head_info.rsg_list_length, 32ull);
            sen->writeUint32(head_info.rsg_list_begin);
            sen->writeUint32(head_info.rsg_number);
            sen->writeUint32(head_info.rsg_info_begin);
            sen->writeUint32(head_info.rsg_info_each_length);
            sen->writeUint32(head_info.composite_number);
            sen->writeUint32(head_info.composite_info_begin);
            sen->writeUint32(head_info.composite_info_each_length);
            sen->writeUint32(head_info.composite_list_length);
            sen->writeUint32(head_info.composite_list_begin);
            sen->writeUint32(head_info.autopool_number);
            sen->writeUint32(head_info.autopool_info_begin);
            sen->writeUint32(head_info.autopool_info_each_length);
            sen->writeUint32(head_info.ptx_number);
            sen->writeUint32(head_info.ptx_info_begin);
            sen->writeUint32(head_info.ptx_info_each_length);
            sen->writeUint32(head_info.part1_begin);
            sen->writeUint32(head_info.part2_begin);
            sen->writeUint32(head_info.part3_begin);
            if (head_info.version == 4)
            {
                sen->writeUint32(head_info.file_offset);
            }
            return;
        }

        inline auto write_file_list(
            const PathTemp &path_temp
        ) -> void
        {
            auto begin_pos = sen->write_pos;
            sen->writeStringFourByte(path_temp.path_slice);
            auto backup_pos = sen->write_pos;
            for (auto i : Range<int>(path_temp.positions.size()))
            {
                sen->writeUint24(path_temp.positions[i].position, begin_pos + static_cast<std::uint64_t>(path_temp.positions[i].offset * 4 + 1));
            }
            sen->write_pos = backup_pos;
            sen->writeUint32(path_temp.pool_index);
            return;
        }

        inline auto file_list_pack(
            const std::vector<PathList> &path_list,
            std::vector<PathTemp> &path_temp_list
        ) -> void
        {
            auto list_length = path_list.size() - 1;
            auto w_pos = 0;
            for (auto i : Range(list_length))
            {
                auto& a_path = path_list[i].path;
                auto& b_path = path_list[i + 1].path;
                if (!is_ascii(b_path))
                {
                    throw Exception(fmt::format("{}: {}", Language::get("popcap.rsg.pack.item_path_must_be_ascii"), b_path), std::source_location::current(), "file_list_pack");
                }
                auto str_longest_length = a_path.size() >= b_path.size() ? a_path.size() : b_path.size();
                for (auto k : Range<int>(str_longest_length))
                {
                    if (k >= a_path.size() || k >= b_path.size() || a_path[k] != b_path[k])
                    {
                        for (auto h = path_temp_list.size(); h > 0; h--)
                        {
                            if (k >= path_temp_list[h - 1].key)
                            {
                                path_temp_list[h - 1].positions.emplace_back(
                                    PathPosition{w_pos, (k - path_temp_list[h - 1].key)});
                                break;
                            }
                        }
                        w_pos += (b_path.size() - k + 2);
                        path_temp_list.emplace_back(
                            PathTemp{b_path.substr(k), k, path_list[i + 1].pool_index});
                        break;
                    }
                }
            }
            return;
        }

        inline auto throw_in_pool(
            DataStreamView &part3_data,
            std::map<std::string, int> &string_pool,
            const std::string &pool_key
        ) const -> int
        {
            if (!string_pool.contains(pool_key)) {
                string_pool.emplace(pool_key, static_cast<int>(part3_data.write_pos));
                part3_data.writeStringByEmpty(pool_key);
            }
            return string_pool[pool_key];
        }

        inline auto write_resources_description(
            RSB_HeadInfo<uint32_t> &head_info,
            const Description &description
        ) const -> void
        {
            auto part1_data = DataStreamView{};
            auto part2_data = DataStreamView{};
            auto part3_data = DataStreamView{};
            auto string_pool = std::map<std::string, int>{};
            part3_data.writeNull(1);
            string_pool.emplace("", 0);
            for (const auto &[group_key, group_value] : description.groups) {
                auto id_part3_pos = throw_in_pool(part3_data, string_pool, group_key);
                part1_data.writeInt32(id_part3_pos);
                part1_data.writeInt32(group_value.subgroups.size());
                part1_data.writeInt32(0x10);
                for (const auto &[subgroup_key, subgroup_value] : group_value.subgroups) {
                    part1_data.writeInt32(subgroup_value.res);
                    auto& language = subgroup_value.language;
                    if (language.empty()) {
                        part1_data.writeInt32(0x0);
                    }
                    else {
                        if (language.size() != 4) {
                            throw Exception(fmt::format("{}: {}", Language::get("popcap.rsb.pack.invalid_language"), language.size()), std::source_location::current(), "write_resources_description");
                        }
                        part1_data.writeString(fmt::format("{}", language));
                    }
                    auto rsg_id_part3_pos = throw_in_pool(part3_data, string_pool, subgroup_key);
                    part1_data.writeInt32(rsg_id_part3_pos);
                    part1_data.writeInt32(subgroup_value.resources.size());
                    for (auto &[resources_key, resources_value] : subgroup_value.resources) {
                        auto id_part2_pos = static_cast<int>(part2_data.write_pos);
                        part1_data.writeInt32(id_part2_pos);
                        //
                        part2_data.writeInt32(0x0);
                        auto type = static_cast<uint16_t>(resources_value.type);
                        part2_data.writeUint16(type);
                        part2_data.writeUint16(0x1C);
                        auto backup_pos = part2_data.write_pos;
                        part2_data.write_pos += 0x8;
                        auto new_id_part3_pos = throw_in_pool(part3_data, string_pool, resources_key);
                        auto item_path = std::regex_replace(resources_value.path, std::regex("/"), "\\");
                        auto path_part3_pos = throw_in_pool(part3_data, string_pool, item_path);
                        part2_data.writeInt32(new_id_part3_pos);
                        part2_data.writeInt32(path_part3_pos);
                        auto& properties = resources_value.properties;
                        part2_data.writeInt32(static_cast<std::int32_t>(properties.size()));
                        if (type == 0) {
                            auto ptx_info_begin_pos = static_cast<int>(part2_data.write_pos);
                            auto &ptx_info = resources_value.ptx_info;
                            part2_data.writeUint16(ptx_info.imagetype);
                            part2_data.writeUint16(ptx_info.aflags);
                            part2_data.writeUint16(ptx_info.x);
                            part2_data.writeUint16(ptx_info.y);
                            part2_data.writeUint16(ptx_info.ax);
                            part2_data.writeUint16(ptx_info.ay);
                            part2_data.writeUint16(ptx_info.aw);
                            part2_data.writeUint16(ptx_info.ah);
                            part2_data.writeUint16(ptx_info.rows);
                            part2_data.writeUint16(ptx_info.cols);
                            auto parent_part3_pos = throw_in_pool(part3_data, string_pool, ptx_info.parent);
                            part2_data.writeInt32(parent_part3_pos);
                            //
                            auto ptx_info_end_pos = static_cast<int>(part2_data.write_pos);
                            part2_data.write_pos = backup_pos;
                            part2_data.writeInt32(ptx_info_end_pos);
                            part2_data.writeInt32(ptx_info_begin_pos);
                            part2_data.write_pos = static_cast<std::size_t>(ptx_info_end_pos);
                        }
                        for (const auto &[properties_key, properties_value] : properties) {
                            auto key_pos = throw_in_pool(part3_data, string_pool, properties_key);
                            auto value_pos = throw_in_pool(part3_data, string_pool, properties_value);
                            part2_data.writeInt32(key_pos);
                            part2_data.writeInt32(0x0);
                            part2_data.writeInt32(value_pos);
                        }
                    }
                }
            }
            head_info.part1_begin = static_cast<uint32_t>(sen->write_pos);
            sen->writeBytes(part1_data.toBytes());
            head_info.part2_begin = static_cast<uint32_t>(sen->write_pos);
            sen->writeBytes(part2_data.toBytes());
            head_info.part3_begin = static_cast<uint32_t>(sen->write_pos);
            sen->writeBytes(part3_data.toBytes());
            return;
        }


        inline auto constexpr is_ascii(
            std::string_view str
        ) noexcept -> bool
        {
            for (auto c : str)
            {
                if (static_cast<uint8_t>(c) > 127_byte)
                {
                    return false;
                }
            }
            return true;
        }

        template <typename T> requires std::is_integral<T>::value
        inline auto constexpr beautify_length (
            T size
        ) -> T
        {
            if ((size % 4096) == 0)
            {
                return static_cast<T>(4096);
            }
            return static_cast<T>((4096 - (size % 4096)));
        }

    public:

        template <auto check_packet>
        inline auto process(
            std::string_view source,
            const Manifest<uint32_t> &manifest
        ) -> void
        {
            static_assert(check_packet == true || check_packet == false, "check_packet can only be true or false");
            sen->writeString("1bsr"_sv);
            auto version = manifest.version;
            if (version != 3 and version != 4)
            {
                throw Exception(fmt::format("{}: {}", Language::get("popcap.rsb.pack.invalid_rsb_version"), version), std::source_location::current(), "process");
            }
            auto file_list_begin_pos = version == 4 ? 0x70 : 0x6C;
            sen->writeUint32(version);
            sen->writeNull(static_cast<std::size_t>(file_list_begin_pos - 8));
            auto head_info = RSB_HeadInfo<uint32_t>{};
            auto ptx_info_size = manifest.ptx_info_size;
            if (ptx_info_size != 0x10 and ptx_info_size != 0x14 and ptx_info_size != 0x18)
            {
                throw Exception(fmt::format("{}: {:02x}", Language::get("popcap.rsb.pack.invalid_ptx_info_size"), ptx_info_size), std::source_location::current(), "process");
            }
            head_info.ptx_info_each_length = ptx_info_size;
            auto item_list = std::vector<PathList>{};
            auto packet_list = std::vector<PathList>{};
            auto composite_list = std::vector<PathList>{};
            auto composite_info = DataStreamView{};
            auto rsg_info = DataStreamView{};
            auto autopool_info = DataStreamView{};
            auto stream_ptx_info = DataStreamView{};
            auto packet_data = DataStreamView{};
            auto packet_index = 0;
            auto ptx_before_number = 0;
            auto pool_index = 0;
            for (const auto &[composite_key, composite_packet] : manifest.group)
            {
                auto composite_name = composite_packet.is_composite ? composite_key : fmt::format("{}_CompositeShell", composite_key);
                composite_info.writeString(composite_name);
                std::transform(composite_name.begin(), composite_name.end(), composite_name.begin(), ::toupper);
                composite_list.emplace_back(PathList{composite_name, pool_index});
                ++pool_index;
                composite_info.writeNull(128 - composite_name.size());
                for (const auto &[subgroup_key, subgroup_value] : composite_packet.subgroup)
                {
                    auto packet_name = subgroup_key;
                    std::transform(packet_name.begin(), packet_name.end(), packet_name.begin(), ::toupper);
                    packet_list.emplace_back(PathList{packet_name, packet_index});
                    auto rsg_file = DataStreamView{fmt::format("{}/packet/{}.rsg", source, packet_name)};
                    if constexpr (check_packet)
                    {
                        // compare rsg and manifest packt info;
                    }
                    auto rsg_composite = false;
                    auto ptx_number = 0;
                    for (auto i : Range<std::size_t>(subgroup_value.packet_info.res.size()))
                    {
                        auto item_path = subgroup_value.packet_info.res[i]["path"].get<std::string>();
                        item_path = std::regex_replace(item_path, std::regex("/"), "\\");
                        std::transform(item_path.begin(), item_path.end(), item_path.begin(), ::toupper);
                        item_list.emplace_back(PathList{item_path, packet_index});
                        try
                        {
                            auto& ptx_info = subgroup_value.packet_info.res[i].at("ptx_info");
                            if (!composite_packet.is_composite)
                            {
                                throw Exception(fmt::format("{}: {}", Language::get("popcap.rsb.pack.invalid_packet_composite"), packet_name), std::source_location::current(), "process");
                            }
                            ++ptx_number;
                            rsg_composite = true;
                            auto id = ptx_info["id"].get<uint32_t>();
                            auto ptx_pos = static_cast<uint64_t>((ptx_before_number + id) * ptx_info_size);
                            stream_ptx_info.writeUint32(ptx_info["width"].get<uint32_t>(), ptx_pos);
                            stream_ptx_info.writeUint32(ptx_info["height"].get<uint32_t>());
                            stream_ptx_info.writeUint32(ptx_info["pitch"].get<uint32_t>());
                            stream_ptx_info.writeUint32(ptx_info["format"].get<uint32_t>());
                            if (ptx_info_size != 0x10)
                            {
                                stream_ptx_info.writeUint32(ptx_info["alpha_size"].get<uint32_t>());
                            }
                            if (ptx_info_size == 0x18)
                            {
                                stream_ptx_info.writeUint32(ptx_info["alpha_format"].get<uint32_t>());
                            }
                        }
                        catch (const nlohmann::ordered_json::out_of_range &e)
                        {
                            // nothing to do;
                        }
                    }
                    composite_info.writeUint32(packet_index);
                    composite_info.writeUint32(subgroup_value.category[0]);
                    if (subgroup_value.category[1].is_string())
                    {
                        auto str = subgroup_value.category[1].get<std::string>();
                        if (str.size() != 4)
                        {
                            throw Exception(fmt::format("{}: {}, {}: {}", Language::get("popcap.rsb.pack.invalid_category_string"), str.size(), 
                                Language::get("but_received"), str), std::source_location::current(), "process");
                        }
                        composite_info.writeString(str);
                    }
                    else
                    {
                        composite_info.writeNull(4);
                    }
                    composite_info.writeNull(4);
                    //
                    auto key_size = packet_name.size();
                    rsg_info.writeString(packet_name);
                    rsg_info.writeNull(128 - key_size);
                    rsg_info.writeUint32(static_cast<uint32_t>(packet_data.write_pos));
                    packet_data.writeBytes(rsg_file.toBytes());
                    rsg_info.writeUint32(rsg_file.size());
                    rsg_info.writeUint32(packet_index);
                    rsg_info.writeBytes(rsg_file.getBytes(0x10, 0x48));
                    auto write_pos = rsg_info.write_pos;
                    rsg_info.writeUint32(rsg_file.readUint32(0x20ull), static_cast<std::uint64_t>(write_pos - 36));
                    rsg_info.writeUint32(ptx_number, static_cast<std::uint64_t>(write_pos));
                    rsg_info.writeUint32(ptx_before_number);
                    ptx_before_number += ptx_number;
                    //
                    autopool_info.writeString(fmt::format("{}_AutoPool", packet_name));
                    autopool_info.writeNull(128 - (key_size + 9));
                    if (rsg_composite)
                    {
                        autopool_info.writeUint32(rsg_file.readUint32(0x18ull));
                        autopool_info.writeUint32(rsg_file.readUint32(0x30ull));
                    }
                    else
                    {
                        auto file_size = (rsg_file.readUint32(0x18ull) + rsg_file.readUint32(0x20ull));
                        autopool_info.writeUint32(file_size);
                        autopool_info.writeNull(4);
                    }
                    autopool_info.writeUint32(1);
                    autopool_info.writeNull(12);
                    //
                    ++packet_index;
                    rsg_file.read_pos = 0ull;
                }
                auto subgroup_size = composite_packet.subgroup.size();
                composite_info.writeNull(1024 - (subgroup_size * 16));
                composite_info.writeUint32(subgroup_size);
            }
            auto item_path_temp_list = std::vector<PathTemp>{};
            item_list.emplace_back(PathList{std::string{""}, 0});
            std::sort(item_list.begin(), item_list.end(), [](const PathList &a, const PathList &b) -> int
                      { return a.path < b.path; });
            file_list_pack(item_list, item_path_temp_list);
            auto packet_path_temp_list = std::vector<PathTemp>{};
            packet_list.emplace_back(PathList{std::string{""}, 0});
            std::sort(packet_list.begin(), packet_list.end(), [](const PathList &a, const PathList &b) -> int
                      { return a.path < b.path; });
            file_list_pack(packet_list, packet_path_temp_list);
            auto composite_path_temp_list = std::vector<PathTemp>{};
            composite_list.emplace_back(PathList{std::string{""}, 0});
            std::sort(composite_list.begin(), composite_list.end(), [](const PathList &a, const PathList &b) -> int
                      { return a.path < b.path; });
             file_list_pack(composite_list, composite_path_temp_list);
            //
            head_info.file_list_begin = file_list_begin_pos;
            for (auto i : Range<int>(item_path_temp_list.size()))
            {
                write_file_list(item_path_temp_list[i]);
            }
            head_info.file_list_length = static_cast<uint32_t>(sen->write_pos - file_list_begin_pos);
            //
            head_info.rsg_list_begin = static_cast<uint32_t>(sen->write_pos);
            for (auto i : Range<int>(packet_path_temp_list.size()))
            {
                write_file_list(packet_path_temp_list[i]);
            }
            head_info.rsg_list_length = static_cast<uint32_t>(sen->write_pos - head_info.rsg_list_begin);
            head_info.composite_number = manifest.group.size();
            head_info.composite_info_begin = static_cast<uint32_t>(sen->write_pos);
            head_info.composite_info_each_length = 1156;
            sen->writeBytes(composite_info.toBytes());
            //
            head_info.composite_list_begin = static_cast<uint32_t>(sen->write_pos);
            for (auto i : Range<int>(composite_path_temp_list.size()))
            {
                write_file_list(composite_path_temp_list[i]);
            }
            head_info.composite_list_length = static_cast<uint32_t>(sen->write_pos - head_info.composite_list_begin);
            //
            head_info.rsg_info_begin = static_cast<uint32_t>(sen->write_pos);
            head_info.rsg_number = packet_index;
            sen->writeBytes(rsg_info.toBytes());
            //
            head_info.autopool_info_each_length = 152;
            head_info.autopool_info_begin = static_cast<uint32_t>(sen->write_pos);
            head_info.autopool_number = packet_index;
            sen->writeBytes(autopool_info.toBytes());
            //
            head_info.ptx_info_begin = static_cast<uint32_t>(sen->write_pos);
            head_info.ptx_number = ptx_before_number;
            sen->writeBytes(stream_ptx_info.toBytes());
            //
            if (version == 3)
            {
                auto description = FileSystem::read_json(fmt::format("{}/description.json", source));
                write_resources_description(head_info, description);
            }
            sen->writeNull(beautify_length<std::uint32_t>(static_cast<std::uint32_t>(sen->write_pos)));
            //
            auto file_offset = static_cast<uint32_t>(sen->write_pos);
            head_info.file_offset = file_offset;
            sen->writeBytes(packet_data.toBytes());
            //
            sen->read_pos = head_info.rsg_info_begin;
            head_info.rsg_info_each_length = 0xCC;
            for (auto i : Range<int>(packet_index))
            {
                auto rsg_info_file_pos = static_cast<uint64_t>((head_info.rsg_info_begin + i * head_info.rsg_info_each_length) + 128);
                auto packet_pos = sen->readUint32(rsg_info_file_pos);
                sen->writeUint32((file_offset + packet_pos), rsg_info_file_pos);
            }
            head_info.version = version;
            write_head(head_info);
            return;
        }

        template <auto check_packet, typename ...Args> requires (std::is_same<Args, std::string_view>::value && ...)
        inline static auto pack_fs(
            std::string_view source,
            std::string_view destination,
            Args... path
        ) -> void
        {
            static_assert(check_packet == true or check_packet == false, "check_packet must be boolean value");
            static_assert(sizeof...(Args) == 1 or sizeof...(Args) == 0, "manifest_path can only have one");
            auto pack = Pack{};
            auto manifest_path = std::string_view{};
            if constexpr (sizeof...(Args) == 1) {
                manifest_path = std::get<0>(std::make_tuple(path...));
            }
            if constexpr (sizeof...(Args) == 0) {
                manifest_path = String::view(fmt::format("{}/manifest.json", source));
            }
            auto manifest = FileSystem::read_json(manifest_path);
            pack.process<check_packet>(source, manifest);
            pack.sen->out_file(destination);
            return;
        }
    };
}