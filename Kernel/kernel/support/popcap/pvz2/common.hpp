#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/rsg/definition.hpp"
#include "kernel/support/popcap/rsg/unpack.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2
{

    using namespace Definition;
    class Common
    {

    protected:
        // RSGUnpack

    protected:
    public:
        inline static auto write_image(
            const std::string &path, 
            const Definition::Image<int> &data
        ) -> void
        {
            FileSystem::create_directory(Path::getParents(path));
            ImageIO::write_png(path, data);
            return;
        }

        inline static auto write_bytes(
            const std::string &path,
            const std::vector<uint8_t> data
        ) -> void
        {
            FileSystem::create_directory(Path::getParents(path));
            FileSystem::write_binary(path, data);
            return;
        }

        inline static auto write_text_file(
            const std::string &path,
            const std::string &content
        ) -> void
        {
            FileSystem::create_directory(Path::getParents(path));
            FileSystem::write_file(path, content);
            return;
        }

        inline static auto write_json(
            const std::string &path,
            const nlohmann::ordered_json & content
        ) -> void
        {
            FileSystem::create_directory(Path::getParents(path));
            FileSystem::write_json(path, content);
            return;
        }

        inline static auto rsg_unpack(
            const std::vector<uint8_t> data,
            PacketInfo<uint32_t> *packet_info
        ) -> void
        {
            auto stream = DataStreamView{data};
            auto rsg_head_info = RSG::RSG_HeadInfo{};
            {
                auto magic = stream.readString(4);
                assert_conditional(magic == RSG::RSG_HeadInfo::magic, fmt::format("{}: {}", Kernel::Language::get("popcap.rsg.unpack.invalid_rsg_magic"), magic), "rsg_unpack");
                auto version = stream.readUint32();
                assert_conditional(version <= 4, fmt::format("{}: {}", Kernel::Language::get("popcap.rsg.unpack.invalid_rsg_version"), version), "rsg_unpack");
                rsg_head_info.version = version;
                stream.read_pos += 8;
                auto flags = stream.readUint32();
                if (flags < 0 || flags > 3)
                {
                    throw Exception(fmt::format("{}: {}", Kernel::Language::get("popcap.rsg.unpack.unknown_compression_flag"), flags), std::source_location::current(), "rsg_unpack");
                }
                rsg_head_info.flags = RSG::CompressionFlag(flags);
                rsg_head_info.file_pos = stream.readUint32();
                rsg_head_info.part0_pos = stream.readUint32();
                rsg_head_info.part0_zlib = stream.readUint32();
                rsg_head_info.part0_size = stream.readUint32();
                stream.read_pos += 4;
                rsg_head_info.part1_pos = stream.readUint32();
                rsg_head_info.part1_zlib = stream.readUint32();
                rsg_head_info.part1_size = stream.readUint32();
                stream.read_pos += 20;
                rsg_head_info.file_list_length = stream.readUint32();
                rsg_head_info.file_list_pos = stream.readUint32();
            }
            auto raw_data = DataStreamView{};
            auto atlas_pos = 0;
            {
                auto part0_length = rsg_head_info.part0_pos + rsg_head_info.part0_size;
                auto part0_zlib_length = rsg_head_info.part0_pos + rsg_head_info.part0_zlib;
                if (part0_length > 0 || part0_zlib_length > 0)
                {
                    if (rsg_head_info.flags == RSG::CompressionFlag::NO_COMPRESSION || rsg_head_info.flags == RSG::CompressionFlag::DEFAULT_COMPRESSION)
                    {
                        raw_data.writeBytes(stream.getBytes(rsg_head_info.part0_pos, part0_length));
                    }
                    else
                    {
                        auto rsg_data = stream.getBytes(rsg_head_info.part0_pos, part0_zlib_length);
                        auto data = Definition::Compression::Zlib::uncompress(rsg_data);
                        raw_data.writeBytes(data);
                    }
                }
                auto use_atlas = (rsg_head_info.part1_pos != 0 and rsg_head_info.part1_size != 0 and rsg_head_info.part1_zlib != 0);
                if (use_atlas)
                {
                    atlas_pos = raw_data.write_pos;
                    if (rsg_head_info.flags == RSG::CompressionFlag::NO_COMPRESSION || rsg_head_info.flags == RSG::CompressionFlag::ATLAS_COMPRESSION)
                    {
                        raw_data.writeBytes(stream.getBytes(rsg_head_info.part1_pos, rsg_head_info.part1_pos + rsg_head_info.part1_size));
                    }
                    else
                    {
                        auto rsg_data = stream.getBytes(rsg_head_info.part1_pos, rsg_head_info.part1_pos + rsg_head_info.part1_zlib);
                        auto data = Definition::Compression::Zlib::uncompress(rsg_data);
                        raw_data.writeBytes(data);
                    }
                }
                packet_info->flags = static_cast<std::uint32_t>(rsg_head_info.flags);
                auto file_list_pos = rsg_head_info.file_list_pos;
                auto file_list_length = rsg_head_info.file_list_length;
                auto offset_limit = file_list_pos + file_list_length;
                auto name_dist_list = std::vector<RSG::NameDict>{};
                auto name_path = std::string{};
                stream.read_pos = file_list_pos;
                auto parent_string = std::map<int, std::string>{};
                while (stream.read_pos < offset_limit)
                {
                    auto key = std::string{};
                    auto position = static_cast<int>(stream.read_pos - file_list_pos) / 4;
                    if (parent_string.contains(position))
                    {
                        key += parent_string[position];
                        parent_string.erase(position);
                    }
                    while (true)
                    {
                        auto current_character = stream.readUint8();
                        auto child_string_offset = stream.readUint24();
                        if (child_string_offset != 0)
                        {
                            parent_string[child_string_offset] = key;
                        }
                        if (current_character == 0)
                            break;
                        if (current_character == 0x5C)
                            current_character = 0x2F;
                        key += static_cast<char>(current_character);
                    }
                    if (key.empty())
                    {
                        break;
                    }
                    const auto is_atlas = stream.readUint32() == 1;
                    auto res = ResInfo<uint32_t>{key, is_atlas};
                    auto pos = stream.readUint32();
                    if (is_atlas)
                    {
                        pos += atlas_pos;
                    }
                    const auto size = stream.readUint32();
                    res.data = std::move(raw_data.getBytes(pos, pos + size));
                    if (is_atlas)
                    {
                        auto id = stream.readUint32();
                        stream.read_pos += 8;
                        auto width = stream.readUint32();
                        auto height = stream.readUint32();
                        res.ptx_info = PTXInfo{id, width, height};
                    }
                    packet_info->res.emplace_back(res);
                }
            }
            return;
        }
    };
}