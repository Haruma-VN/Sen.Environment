#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pak/definition.hpp"
#include "kernel/definition/compression/zip.hpp"

namespace Sen::Kernel::Support::PopCap::PAK
{
    using namespace Definition;

    class Pack
    {
    protected:

        std::unique_ptr<DataStreamView> stream;

        inline auto format_path(
            const std::string &file_path,
            bool windows_path_separate
        ) -> std::string
        {

            auto path = file_path;
            if (windows_path_separate)
            {
                if (path.find(forward_slash) != std::string::npos)
                {
                    path = std::regex_replace(path, std::regex("/"), "\\\\");
                }
                if (path.find(double_back_slash) != std::string::npos)
                {
                    path = std::regex_replace(path, std::regex("\\\\"), "\\");
                }
                if (path.find(empty_back_slash) != std::string::npos)
                {
                    path = std::regex_replace(path, std::regex(" \\"), "\\");
                }
            }
            else
            {
                if (path.find(double_back_slash) != std::string::npos)
                {
                    path = std::regex_replace(path, std::regex("\\\\"), "/");
                }
                if (path.find(double_forward_slash) != std::string::npos)
                {
                    path = std::regex_replace(path, std::regex("//"), "/");
                }
                if (path.find(empty_forward_slash) != std::string::npos)
                {
                    path = std::regex_replace(path, std::regex(" /"), "/");
                }
            }
            return path;
        }

        inline auto write_file_info(
            const FileInfo &file_info,
            bool compress_zlib
        ) -> void 
        {
            stream->writeStringByUint8(file_info.file_name);
            stream->writeUint32(file_info.zlib_size);
            if (compress_zlib) {
                stream->writeUint32(file_info.size);
            }
            stream->writeInt64(file_info.file_time);
            return;
        }

        inline auto write_file(
            const std::vector<FileInfo>& file_info_list,
            bool compress_zlib
        ) -> void
        {
            stream->writeInt32(pak_default_head);
            stream->writeInt32(pak_version);
            if (file_info_list.empty()) return;
            for (const auto &file_info : file_info_list) {
                stream->writeUint8(0x0);
                write_file_info(file_info, compress_zlib);
            }
            stream->writeUint8(info_end);
            return;
        }

        inline auto fill_0x1000(

        ) -> void 
        {
            const auto &pos = static_cast<int>(stream->read_pos & (0x1000 - 1));
            if (pos == 0) {
                stream->writeUint16(0x1000 - 2);
                stream->writeNull(0x1000 - 2);
            }
            else if (pos > (0x1000 - 2))
            {
                const auto & wide = 0x2000 - 2 - pos;
                stream->writeUint16(static_cast<uint16_t>(wide));
                stream->writeNull(wide);
            }
            else {
                const auto & wide = 0x1000 - 2 - pos;
                stream->writeUint16(static_cast<uint16_t>(wide));
                stream->writeNull(wide);
            }
            return;
        }

        inline auto fill(

        ) -> void 
        {
            const auto &pos = static_cast<int>(stream->read_pos & 0b111);
            if (pos == 0) {
                stream->writeUint16(0x6);
                stream->writeNull(6);
            }
            else if (pos > 5)
            {
                const auto & wide = 14 - pos;
                stream->writeUint16(static_cast<uint16_t>(wide));
                stream->writeNull(wide);
            }
            else {
                const auto & wide = 6 - pos;
                stream->writeUint16(static_cast<uint16_t>(wide));
                stream->writeNull(wide);
            }
            return;
        }

    public:
        explicit Pack(

        ) = default;

        ~Pack(

            ) = default;

        inline auto process(
            std::string_view source,
            std::string_view destination,
            const PakInfo &pak_info
        ) -> void
        {
            const auto &folder = fmt::format("{}/bundle", source);
            const auto &pak_platform = pak_info.pak_platform;
            if (pak_platform != "PC" && pak_platform != "Xbox_360" && pak_platform != "TV")
            {
                throw Exception(String::format(fmt::format("{}", Language::get("popcap.pak.pack.invalid_pak_platform")), pak_platform), std::source_location::current(), "process"); 
            }
            const auto &compress_zlib = pak_info.zlib_compress;
            if (pak_platform == "TV")
            {
                Compression::Zip::Compress::directory(folder, destination);
                return;
            }
            const auto &file_list = FileSystem::read_directory(folder);
            auto file_info_list = std::vector<FileInfo>{};
            const auto &folder_length = folder.size() + 1;
            for (const auto &path : file_list)
            {
                auto file_info = FileInfo {
                    .file_name = format_path(path.substr(folder_length), pak_info.windows_path_separate)
                };
                file_info_list.emplace_back(file_info);
            }
            write_file(file_info_list, compress_zlib);
            for (const auto &i : Range<int>(file_list.size())) {
                const auto &path = file_list[i];
                if (pak_platform != "PC") {
                    if (pak_platform != "Xbox_360" && std::regex_search(path, std::regex("\\.ptx$", std::regex_constants::icase))) {
                        fill_0x1000();
                    }
                    else {
                        fill();
                    }
                }
                auto &file_info = file_info_list.at(i);
                const auto &data = FileSystem::read_binary<uint8_t>(path);
                file_info.size = data.size();
                if (compress_zlib) {
                    const auto &zlib_data = Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_9>(data);
                    file_info.zlib_size = zlib_data.size();
                    stream->writeBytes(zlib_data);
                }
                else {
                    stream->writeBytes(data);
                }
            }
            stream->write_pos = 0;
            write_file(file_info_list, compress_zlib);
            if (pak_platform == "PC") {
                stream->read_pos = 0;
                stream->write_pos = 0;
                for (const auto &i : Range<int>(stream->size())) {
                    const auto &p = stream->readUint8();
                    stream->writeUint8(static_cast<uint8_t>(p ^ 0xF7));
                } 
            }
            stream->out_file(destination);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto pack = Pack{};
            auto pak_info = *FileSystem::read_json(fmt::format("{}/info.json", destination));
            pack.process(source, destination, pak_info);
            return;
        }
    };

}