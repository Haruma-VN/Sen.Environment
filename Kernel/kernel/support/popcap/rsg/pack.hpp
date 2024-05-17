#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/rsg/definition.hpp"

namespace Sen::Kernel::Support::PopCap::RSG
{

    using namespace Definition;

    struct PathList
    {
    public:
        std::string path;
        int res_pool;

        explicit PathList(
            const std::string &path,
            int res_pool) : path(path), res_pool(res_pool)
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
        std::string path_slice;
        int key;
        bool is_atlas;
        ResInfo res_info;
        std::vector<PathPosition> positions;
    };

    class Pack
    {

    protected:
        using Zlib = Definition::Compression::Zlib;

    public:
        DataStreamView atlas_group;

        int atlas_pos;

        DataStreamView data_group;

        int data_pos;

    protected:
        inline auto rewrite_path_list(
            std::vector<PathList> &path_list,
            const std::vector<ResInfo> &res_info) const -> void
        {
            for (auto i : Range<int>(res_info.size()))
            {
                auto new_path = res_info[i].path;
                new_path = std::regex_replace(new_path, std::regex("/"), "\\");
                std::transform(new_path.begin(), new_path.end(), new_path.begin(), ::toupper);
                path_list.emplace_back(PathList{new_path, i});
            }
            path_list.emplace_back(PathList{std::string{""}, 0});
            std::sort(path_list.begin(), path_list.end(), [](const PathList &a, const PathList &b) -> int
                      { return a.path < b.path; });
            return;
        }

        inline auto file_list_pack(
            const std::vector<ResInfo> &res_info,
            std::vector<PathTemp> &path_temp_list) -> void
        {
            auto path_list = std::vector<PathList>{};
            rewrite_path_list(path_list, res_info);
            auto list_length = path_list.size() - 1;
            auto w_pos = 0;
            for (auto i : Range(list_length))
            {
                auto a_path = path_list[i].path;
                auto b_path = path_list[i + 1].path;
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
                        auto is_atlas = b_path.ends_with(".PTX");
                        w_pos += is_atlas ? (b_path.size() - k + 9) : (b_path.size() - k + 4);
                        path_temp_list.emplace_back(
                            PathTemp{
                                b_path.substr(k),
                                k,
                                is_atlas,
                                res_info[path_list[i + 1].res_pool]});
                        break;
                    }
                }
            }
            return;
        }

        template <auto use_res_folder, typename... Args>
            requires(std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value && ...)
        inline auto write_rsg(
            const std::vector<PathTemp> &path_temp_list,
            CompressionFlag compression_flags,
            std::string_view source,
            Args ...bank) -> void
        {
            static_assert(use_res_folder == true || use_res_folder == false, "use_res_folder can only be true or false");
            static_assert(sizeof...(Args) == 1 or sizeof...(Args) == 0, "rsg bank can only have one");
            auto path_temp_length = path_temp_list.size();
            auto file_list_begin = sen.write_pos;
            if (file_list_begin != 0x5C)
            {
                throw Exception(fmt::format("{} 0x{:X}", Language::get("popcap.rsg.pack.invalid_file_list"), 0x5C), std::source_location::current(), "write_rsg");
            }
            auto packet_bank = std::map<std::string, std::vector<uint8_t>>{};
            if constexpr (sizeof...(Args) == 1)
            {
                packet_bank = std::get<0>(std::make_tuple(bank...));
            }
            for (auto i : Range<int>(path_temp_length))
            {
                auto begin_pos = sen.write_pos;
                auto packet_res_info = path_temp_list[i].res_info;
                sen.writeStringFourByte(path_temp_list[i].path_slice);
                auto backup_pos = sen.write_pos;
                for (auto h : Range<int>(path_temp_list[i].positions.size()))
                {
                    sen.writeUint24(path_temp_list[i].positions[h].position, static_cast<std::size_t>((begin_pos + path_temp_list[i].positions[h].offset * 4 + 1)));
                }
                auto item_data = std::vector<uint8_t>{};
                if constexpr (sizeof...(Args) == 1)
                {
                    debug(packet_res_info.path);
                    item_data = packet_bank.at(packet_res_info.path);
                }
                if constexpr (sizeof...(Args) == 0)
                {
                    if constexpr (use_res_folder)
                    {
                        item_data = FileSystem::read_binary<uint8_t>(fmt::format("{}/res/{}", source, packet_res_info.path));
                    }
                    else
                    {
                        item_data = FileSystem::read_binary<uint8_t>(fmt::format("{}/{}", source, packet_res_info.path));
                    }
                }

                auto append_size = beautify_length<std::size_t, true>(item_data.size());
                if (path_temp_list[i].is_atlas)
                {
                    atlas_group.writeBytes(item_data);
                    atlas_group.writeNull(append_size);
                    sen.write_pos = backup_pos;
                    sen.writeUint32(1);
                    sen.writeUint32(atlas_pos);
                    sen.writeUint32(item_data.size());
                    sen.writeUint32(packet_res_info.ptx_info.id);
                    sen.writeNull(8);
                    sen.writeUint32(packet_res_info.ptx_info.width);
                    sen.writeUint32(packet_res_info.ptx_info.height);
                    atlas_pos += (item_data.size() + append_size);
                }
                else
                {
                    data_group.writeBytes(item_data);
                    data_group.writeNull(append_size);
                    sen.write_pos = backup_pos;
                    sen.writeUint32(0);
                    sen.writeUint32(data_pos);
                    sen.writeUint32(item_data.size());
                    data_pos += (item_data.size() + append_size);
                }
            }
            auto file_list_length = sen.write_pos - file_list_begin;
            auto append_size = beautify_length<std::size_t, false>(sen.write_pos);
            sen.writeNull(append_size);
            auto backup_pos = sen.write_pos;
            sen.writeUint32(sen.write_pos, 0x14_size);
            sen.writeUint32(file_list_length, 0x48_size);
            sen.writeUint32(file_list_begin);
            sen.write_pos = backup_pos;
            compressor(compression_flags);
            return;
        }

        inline auto write_data(
            const std::vector<uint8_t> &data_bytes,
            CompressionFlag compression_flags,
            bool is_atlas) -> void
        {
            auto part0_pos = sen.write_pos;
            auto part0_size = data_bytes.size();
            if (compression_flags == CompressionFlag::NO_COMPRESSION || compression_flags == CompressionFlag::DEFAULT_COMPRESSION)
            {
                sen.writeBytes(data_bytes);
                auto backup_pos = sen.write_pos;
                sen.writeUint32(part0_pos, 0x18_size);
                sen.writeUint32(part0_size);
                if (is_atlas)
                {
                    sen.writeUint32(0);
                }
                else
                {
                    sen.writeUint32(part0_size);
                }
                sen.write_pos = backup_pos;
            }
            else
            {
                auto zlib_bytes = compression_flags == CompressionFlag::BEST_COMPRESSION ? Zlib::compress<Zlib::Level::LEVEL_9>(data_bytes) : Zlib::compress<Zlib::Level::LEVEL_6>(data_bytes);
                auto append_size = beautify_length<std::size_t, false>(zlib_bytes.size());
                sen.writeBytes(zlib_bytes);
                sen.writeNull(append_size);
                auto part0_zlib = zlib_bytes.size() + append_size;
                auto backup_pos = sen.write_pos;
                sen.writeUint32(part0_pos, 0x18_size);
                sen.writeUint32(part0_zlib);
                sen.writeUint32(part0_size);
            }
            return;
        }

        inline auto compressor(
            CompressionFlag compression_flags) -> void
        {
            if (data_group.size() != 0)
            {
                write_data(data_group.toBytes(), compression_flags, false);
            }
            if (atlas_group.size() != 0)
            {
                auto atlas_bytes = atlas_group.toBytes();
                auto part1_pos = 0;
                auto part1_size = atlas_bytes.size();
                auto data_empty = DataStreamView{};
                data_empty.writeUint32(252536);
                data_empty.writeUint32(16777216);
                data_empty.writeNull(4088);
                if (compression_flags == CompressionFlag::NO_COMPRESSION || compression_flags == CompressionFlag::ATLAS_COMPRESSION)
                {
                    if (compression_flags == CompressionFlag::ATLAS_COMPRESSION && data_group.size() == 0)
                    {
                        write_data(data_empty.toBytes(), CompressionFlag::DEFAULT_COMPRESSION, true);
                    }
                    else
                    {
                        write_data(std::vector<uint8_t>{}, CompressionFlag::DEFAULT_COMPRESSION, true);
                    }
                    part1_pos = sen.write_pos;
                    sen.writeBytes(atlas_bytes);
                    auto backup_pos = sen.write_pos;
                    sen.writeUint32(part1_pos, 0x28_size);
                    sen.writeUint32(part1_size);
                    sen.writeUint32(part1_size);
                    sen.write_pos = backup_pos;
                }
                else
                {
                    if (compression_flags == CompressionFlag::BEST_COMPRESSION && data_group.size() == 0)
                    {
                        write_data(data_empty.toBytes(), CompressionFlag::DEFAULT_COMPRESSION, true);
                    }
                    else
                    {
                        write_data(std::vector<uint8_t>{}, CompressionFlag::DEFAULT_COMPRESSION, true);
                    }
                    part1_pos = sen.write_pos;
                    auto zlib_bytes = compression_flags == CompressionFlag::BEST_COMPRESSION ? Zlib::compress<Zlib::Level::LEVEL_9>(atlas_bytes) : Zlib::compress<Zlib::Level::LEVEL_6>(atlas_bytes);
                    auto append_size = beautify_length<std::size_t, false>(zlib_bytes.size());
                    sen.writeBytes(zlib_bytes);
                    sen.writeNull(append_size);
                    auto part1_zlib = zlib_bytes.size() + append_size;
                    auto backup_pos = sen.write_pos;
                    sen.writeUint32(part1_pos, 0x28_size);
                    sen.writeUint32(part1_zlib);
                    sen.writeUint32(part1_size);
                    sen.write_pos = backup_pos;
                }
            }
            else
            {
                sen.writeUint32(sen.size(), 0x28_size);
            }
            return;
        }

        template <typename T, auto for_file = false>
            requires std::is_integral<T>::value
        inline auto beautify_length(
            T size) const noexcept -> T
        {
            static_assert(for_file == true || for_file == false, "for_file can only be true or false");
            if ((size % 4096) == 0)
            {
                if constexpr (for_file)
                {
                    return 0;
                }
                else
                {
                    return 4096;
                }
            }
            return (4096 - (size % 4096));
        }

        inline auto constexpr is_ascii(
            std::string_view str) noexcept -> bool
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

    public:
        Pack() = default;

        ~Pack(

            ) = default;

        Pack(
            Pack &&that) = delete;

        auto operator=(
            Pack &&that) -> Pack & = delete;

        DataStreamView sen;

        template <auto use_res_folder, typename... Args>
            requires(std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value && ...)
        inline auto process(
            std::string_view source,
            const PacketInfo &packet_info,
            Args... bank) -> void
        {
            static_assert(use_res_folder == true || use_res_folder == false, "use_res_folder can only be true or false");
            static_assert(sizeof...(Args) == 1 or sizeof...(Args) == 0, "rsg bank can only have one");
            auto version = packet_info.version;
            if (version != 3 && version != 4)
            {
                throw Exception(fmt::format("{}: {}", Language::get("popcap.rsg.pack.invalid_rsg_version"), version), std::source_location::current(), "process");
            }
            auto compression_flags = CompressionFlag(packet_info.compression_flags);
            sen.writeString("pgsr"_sv);
            sen.writeUint32(version);
            sen.writeNull(8);
            sen.writeUint32(packet_info.compression_flags);
            sen.writeNull(72);
            auto path_temp_list = std::vector<PathTemp>{};
            auto res_list = packet_info.res;
            file_list_pack(res_list, path_temp_list);
            if constexpr (sizeof...(Args) == 1)
            {
                write_rsg<false>(path_temp_list, compression_flags, source, bank...);
            }
            if constexpr (sizeof...(Args) == 0)
            {
                if constexpr (use_res_folder)
                {
                    write_rsg<true>(path_temp_list, compression_flags, source);
                }
                else
                {
                    write_rsg<false>(path_temp_list, compression_flags, source);
                }
            }
            return;
        }

        inline static auto pack_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto pack = Pack{};
            auto packet_info = *FileSystem::read_json(fmt::format("{}/packet.json", source));
            pack.process<true>(source, packet_info);
            pack.sen.out_file(destination);
            return;
        }
    };

}