#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pak/definition.hpp"
#include "kernel/definition/compression/zip.hpp"
#include "kernel/support/popcap/zlib/uncompress.hpp"

namespace Sen::Kernel::Support::PopCap::PAK
{
    using namespace Definition;

    class Unpack
    {
    protected:
        std::unique_ptr<DataStreamView> stream;

        inline auto read_file_info(
            FileInfo &file_info, 
            uint8_t compress_byte
        ) -> void
        {
            file_info.file_name = stream->readStringByUint8();
            file_info.size = stream->readUint32();
            if (compress_byte == 1_byte) {
                file_info.zlib_size = stream->readUint32();
            }
            file_info.file_time = stream->readInt64();
            return;
        }

        inline auto jump(

        ) -> bool
        {
            const auto &jump_pos = stream->readUint16();
            stream->read_pos += static_cast<size_t>(jump_pos);
            if (jump_pos > 8) {
                return true;
            }
            return false;
        }

    public:
        explicit Unpack(
            std::string_view source) : stream(std::make_unique<DataStreamView>(source))
        {
        }

        explicit Unpack(
            DataStreamView &it) : stream(&it)
        {
        }

        ~Unpack(

            ) = default;

        inline auto process(
            std::string_view source,
            std::string_view destination,
            PakInfo &pak_info
        ) -> void
        {
            const auto &magic = stream->readInt32();
            stream->read_pos -= 4; // return 4;
            auto pak_platform = "Xbox_360";
            switch (magic)
            {
            case pak_pc_head:
            {
                pak_platform = "PC";
                stream->write_pos = 0;
                for (const auto &i : Range<int>(stream->size()))
                {
                    const auto &p = static_cast<uint8_t>(stream->readUint8() ^ 0xF7);
                    stream->writeUint8(p);
                }
                break;
            }
            case pak_default_head:
            {
                // nothting to do
                break;
            }
            case pak_tv_head:
            {
                pak_platform = "TV";
                break;
            }
            default:
            {
                throw Exception(fmt::format("{}", Language::get("popcap.pak.unpack.invalid_pak_header")), std::source_location::current(), "process"); 
            }
            }
            stream->read_pos = 0;
            if (pak_platform == "TV")
            {
                Compression::Zip::Uncompress::process(source, fmt::format("{}/bundle", destination));
                return;
            }
                const auto &pak_magic = stream->readInt32();
                assert_conditional(pak_magic == pak_default_head, fmt::format("{}", "invaild_magic"), "process");
                const auto &version = stream->readInt32();
                assert_conditional(version == pak_version, fmt::format("{}", "invaild_version"), "process");
                auto compress_type = 2_byte;
                auto file_info_list = std::vector<FileInfo>{};
                try
                {
                    while (true)
                    {
                        if (stream->readUint8() == info_end)
                            break;
                        if (compress_type == 2_byte)
                        {
                            const auto &pos = stream->read_pos;
                            stream->readStringByUint8();
                            stream->read_pos += 0x10;
                            const auto &check_byte = stream->readUint8();
                            if (check_byte == 0x0 or check_byte == info_end)
                            {
                                compress_type = 1_byte;
                            }
                            else
                            {
                                compress_type = 0_byte;
                            }
                            stream->read_pos = pos;
                        }
                        auto file_info = FileInfo{};
                        read_file_info(file_info, compress_type);
                        file_info_list.emplace_back(file_info);
                    }
                }
                catch (Exception ex)
                {
                    stream->read_pos = 0x8;
                    compress_type = 0_byte;
                    file_info_list.clear();
                    auto file_info = FileInfo{};
                    read_file_info(file_info, compress_type);
                    file_info_list.emplace_back(file_info);
                }
                for (const auto &file_info : file_info_list)
                {
                    if (file_info.file_name.find(back_slash) != std::string::npos)
                    {
                        pak_info.windows_path_separate = true;
                        break;
                    }
                    if (file_info.file_name.find(forward_slash) != std::string::npos)
                    {
                        break;
                    }
                }
                pak_info.zlib_compress = compress_type == 1_byte;
                if (file_info_list.empty())
                {
                    return;
                }
                auto pak_360 = false;
                for (const auto &file_info : file_info_list)
                {
                    if (pak_info.pak_platform != "PC")
                    {
                        pak_360 |= jump();
                    }
                    const auto &packet_destination = fmt::format("{}/bundle/{}", destination, file_info.file_name);
                    #if WINDOWS
                    auto filePath = std::filesystem::path(String::utf8_to_utf16(packet_destination.data()).data());
                    #else
                    auto filePath = std::filesystem::path(packet_destination.data());
                    #endif
                    if (filePath.has_parent_path())
                    {
                        std::filesystem::create_directories(filePath.parent_path());
                    }
                    if (file_info.zlib_size != 0)
                    {
                        const auto &data = Definition::Compression::Zlib::uncompress(stream->readBytes(file_info.size));
                        FileSystem::write_binary(packet_destination, data);
                    }
                    else
                    {
                        FileSystem::write_binary(packet_destination, stream->readBytes(file_info.size));
                    }
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto unpack = Unpack{source};
            auto pak_info = PakInfo{};
            unpack.process(source, destination, pak_info);
            FileSystem::write_json(fmt::format("{}/info.json", destination), pak_info);
            return;
        }
    };

}