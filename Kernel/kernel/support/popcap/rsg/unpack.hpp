#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/rsg/definition.hpp"
#include "kernel/support/popcap/zlib/uncompress.hpp"

namespace Sen::Kernel::Support::PopCap::RSG
{
    using namespace Definition;
    template <bool use_res_folder>
    class Unpack
    {
    protected:
        std::unique_ptr<DataStreamView> sen;

        std::unique_ptr<DataStreamView> raw_data;

        mutable int atlas_pos;

        inline auto read_head(
            RSG_HeadInfo *rsg_head_info) const -> void
        {
            auto magic = sen->readString(4);
            auto version = sen->readUint32();
            if (version > 4)
            {
                throw Exception("invaild_rsg_version");
            }
            rsg_head_info->version = version;
            sen->read_pos += 8;
            auto flags = sen->readUint32();
            switch (flags)
            {
            case 0:
                rsg_head_info->flags = RSG_Compression_Flags::NO_COMPRESSION;
                break;
            case 1:
                rsg_head_info->flags = RSG_Compression_Flags::DEFAULT_COMPRESSION;
                break;
            case 2:
                rsg_head_info->flags = RSG_Compression_Flags::ATLAS_COMPRESSION;
                break;
            case 3:
                rsg_head_info->flags = RSG_Compression_Flags::BEST_COMPRESSION;
                break;
            default:
                throw Exception("invaild_compression_flags");
            }
            rsg_head_info->file_pos = sen->readUint32();
            rsg_head_info->part0_pos = sen->readUint32();
            rsg_head_info->part0_zlib = sen->readUint32();
            rsg_head_info->part0_size = sen->readUint32();
            sen->read_pos += 4;
            rsg_head_info->part1_pos = sen->readUint32();
            rsg_head_info->part1_zlib = sen->readUint32();
            rsg_head_info->part1_size = sen->readUint32();
            sen->read_pos += 20;
            rsg_head_info->file_list_length = sen->readUint32();
            rsg_head_info->file_list_pos = sen->readUint32();
            return;
        }

        inline auto uncompress_zlib(
            const RSG_HeadInfo &rsg_head_info) const -> void
        {
            auto part0_length = rsg_head_info.part0_pos + rsg_head_info.part0_size;
            auto part0_zlib_length = rsg_head_info.part0_pos + rsg_head_info.part0_zlib;
            if (part0_length > 0 || part0_zlib_length > 0)
            {
                if (rsg_head_info.flags == RSG_Compression_Flags::NO_COMPRESSION || rsg_head_info.flags == RSG_Compression_Flags::DEFAULT_COMPRESSION)
                {
                    raw_data->writeBytes(sen->getBytes(rsg_head_info.part0_pos, part0_length));
                }
                else
                {
                    auto uncompress_zlib = std::make_unique<PopCap::Zlib::Uncompress>(false);
                    raw_data->writeBytes(uncompress_zlib->uncompress(sen->getBytes(rsg_head_info.part0_pos, part0_zlib_length)));
                }
            }

            auto use_atlas = (rsg_head_info.part1_pos == 0 and rsg_head_info.part1_size == 0 and rsg_head_info.part1_zlib == 0);
            if (use_atlas)
            {
                atlas_pos = raw_data->write_pos;
                if (rsg_head_info.flags == RSG_Compression_Flags::NO_COMPRESSION || rsg_head_info.flags == RSG_Compression_Flags::ATLAS_COMPRESSION)
                {
                    raw_data->writeBytes(sen->getBytes(rsg_head_info.part1_pos, rsg_head_info.part1_pos + rsg_head_info.part1_size));
                }
                else
                {
                    auto uncompress_zlib = std::make_unique<PopCap::Zlib::Uncompress>(false);
                    raw_data->writeBytes(uncompress_zlib->uncompress(sen->getBytes(rsg_head_info.part1_pos, rsg_head_info.part1_pos + rsg_head_info.part1_zlib)));
                }
            }
            return;
        }

        inline auto write_file(
            const std::string &packet_destination,
            const RSG_HeadInfo &rsg_head_info,
            bool is_atlas) const -> void
        {
            auto pos = sen->readUint32() + is_atlas ? atlas_pos : 0;
            auto size = sen->readInt32();
            auto filePath = std::filesystem::path(packet_destination);
            if (filePath.has_parent_path())
            {
                std::filesystem::create_directories(filePath.parent_path());
            }
            FileSystem::write_binary(packet_destination, sen->getBytes(pos, pos + size));
            return;
        }

    public:
        explicit Unpack(
            std::string_view source) : sen(std::make_unique<DataStreamView>(source))
        {
        }

        explicit Unpack(
            DataStreamView &it) : sen(&it)
        {
        }

        ~Unpack(

            ) = default;

        template <bool T>
        inline auto process(
            std::string_view destination) -> RSG_PacketInfo
        {
            auto rsg_head_info = RSG_HeadInfo{};
            read_head(&rsg_head_info);
            auto packet_destination = std::string_view{};
            if constexpr (use_res_folder) {
                packet_destination = fmt::format("{}/res", destination);
            }
            else {
                packet_destination = destination;
            }
            FileSystem::create_directory(packet_destination);
            uncompress_zlib(rsg_head_info);
            auto packet_info = RSG_PacketInfo{static_cast<std::uint32_t>(rsg_head_info.flags)};
            auto file_list_pos = rsg_head_info.file_list_pos;
            auto file_list_length = rsg_head_info.file_list_length;
            auto offset_limit = file_list_pos + file_list_length;
            auto name_dist_list = std::vector<NameDict>{};
            auto name_path = std::string{};
            sen->read_pos = file_list_pos;
            while (sen->read_pos < offset_limit)
            {
                auto char_byte = sen->readUint8();
                auto pos = sen->readUint24() * 4;
                if (char_byte == 0)
                {
                    if (pos != 0)
                    {
                        name_dist_list.emplace_back(NameDict{
                            name_path,
                            pos,
                        });
                    }
                    auto is_atlas = sen->readUint32() == 1;
                    auto res = nlohmann::ordered_json{{"path", name_path}};
                    write_file(fmt::format("{}/{}", packet_destination, name_path), rsg_head_info, is_atlas);
                    if (is_atlas)
                    {
                        auto id = sen->readUint32();
                        sen->read_pos += 8;
                        res["ptx_info"] = {
                            {"id", id},
                            {"width", sen->readUint32()},
                            {"height", sen->readUint32()}};
                    }
                    packet_info.res.emplace_back(res);
                    for (auto i : Range(name_dist_list.size()))
                    {
                        if (name_dist_list[i].pos + static_cast<unsigned long long>(file_list_pos) == sen->read_pos)
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
        }

        inline static auto unpack_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto unpack = Unpack{source};
            auto packet_info = unpack.process<false>(destination);
            FileSystem::write_json(fmt::format("{}/packet.json", destination), packet_info);
            return;
        }
    };
}
