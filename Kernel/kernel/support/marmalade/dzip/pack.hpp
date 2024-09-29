#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/marmalade/dzip/definition.hpp"

namespace Sen::Kernel::Support::Marmalade::DZip
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Pack : Common
    {
    private:
        inline static auto process_packages(
            DataStreamView &stream,
            Package const &definition,
            std::string const &resource_directory) -> void
        {
            stream.writeUint32(k_magic_identifier);
            /*
            auto information_structure = Information{};
            information_structure.resource_file.resize(definition.resource.size());
            information_structure.resource_directory.resize(definition.resource.size() + 1_size);
            information_structure.resource_information.resize(definition.resource.size());
            auto global_chunk_index = k_begin_index;
            for (auto resource_index : Range(definition.resource.size())) {
                auto & resource_definition = definition.resource[resource_index];
                auto & resource_information_structure = information_structure.resource_information[resource_index];
                auto   resource_directory_string = String::to_windows_style(Path::getParents(resource_definition.path));
                information_structure.resource_file.emplace_back(Path::getFileName(resource_definition.path));
                if (std::find(information_structure.resource_directory.begin(), information_structure.resource_directory.end(), resource_directory_string) == information_structure.resource_directory.end()) {
                    information_structure.resource_directory.emplace_back(resource_directory_string);
                }
                resource_information_structure.chunk_index.resize(static_cast<size_t>(resource_definition.chunk.size()));
                information_structure.chunk_information.resize(resource_definition.chunk.size());
                global_chunk_index += resource_definition.chunk.size();
            }
            */
            auto information_structure = Information{};
            information_structure.archive_setting.version = static_cast<uint8_t>(k_version);
           // information_structure.resource_file.resize(definition.resource.size());
          //  information_structure.resource_directory.resize(definition.resource.size() + 1_size);
            information_structure.resource_directory.emplace_back(""_sv);
            information_structure.resource_information.resize(definition.resource.size());
            auto global_chunk_index = k_begin_index;
            auto chunk_information = DataStreamView{};
            for (auto resource_index : Range(definition.resource.size()))
            {
                auto &resource_definition = definition.resource[resource_index];
                auto &resource_information_structure = information_structure.resource_information[resource_index];
                auto resource_path = fmt::format("{}/{}", resource_directory, resource_definition.path);
                auto resource_directory_string = String::to_windows_style(Path::getParents(resource_definition.path));
                information_structure.resource_file.emplace_back(Path::getFileName(resource_definition.path));
                if (auto index = std::find(information_structure.resource_directory.begin(), information_structure.resource_directory.end(), resource_directory_string); index != information_structure.resource_directory.end())
                {
                    resource_information_structure.directory_index = static_cast<uint16_t>(std::distance(information_structure.resource_directory.begin(), index));
                }
                else
                {
                    resource_information_structure.directory_index = static_cast<uint16_t>(information_structure.resource_directory.size());
                    information_structure.resource_directory.emplace_back(resource_directory_string);
                }
                resource_information_structure.chunk_index.resize(static_cast<size_t>(resource_definition.chunk.size()));
               // information_structure.chunk_information.resize(static_cast<size_t>(resource_definition.chunk.size()));
                for (auto chunk_index : Range(resource_definition.chunk.size()))
                {
                    resource_information_structure.chunk_index[chunk_index] = static_cast<uint16_t>(global_chunk_index);
                    auto &chunk_definition = resource_definition.chunk[chunk_index];
                    auto chunk_information_structure = ChunkInformation{};
                    auto chunk_flag = std::bitset<ChunkFlag::k_count>{};
                    switch (chunk_definition.flag)
                    {
                    case ChunkFlagEnum::zlib:
                    {
                        chunk_flag.set(ChunkFlag::zlib);
                        break;
                    }
                    case ChunkFlagEnum::bzip2:
                    {
                        chunk_flag.set(ChunkFlag::bzip2);
                        break;
                    }
                    case ChunkFlagEnum::zerod_out:
                    {
                        chunk_flag.set(ChunkFlag::zerod_out);
                        break;
                    }
                    case ChunkFlagEnum::copy_coded:
                    {
                        chunk_flag.set(ChunkFlag::copy_coded);
                        break;
                    }
                    case ChunkFlagEnum::lzma:
                    {
                        chunk_flag.set(ChunkFlag::lzma);
                        break;
                    }
                    default:
                        assert_conditional(false, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_flag")), "process_packages"); 
                        break;
                    }
                    auto chunk_size_uncompressed = 0_size;
                    auto chunk_size_compressed = 0_size;
                    chunk_information_structure.offset = static_cast<uint32_t>(chunk_information.write_pos);
                    if (chunk_flag.test(ChunkFlag::zlib))
                    {
                        auto chunk_data = FileSystem::read_binary<uint8_t>(resource_path);
                        chunk_information.writeBytes(Kernel::Definition::Compression::Zlib::compress_gzip<6>(chunk_data));
                        chunk_size_uncompressed = chunk_data.size();
                        chunk_size_compressed = chunk_size_uncompressed;
                    }
                    if (chunk_flag.test(ChunkFlag::bzip2))
                    {
                        auto chunk_data = FileSystem::read_binary<uint8_t>(resource_path);
                        chunk_information.writeBytes(Kernel::Definition::Compression::Bzip2::compress(chunk_data, 9, 30));
                        chunk_size_uncompressed = chunk_data.size();
                        chunk_size_compressed = chunk_size_uncompressed;
                    }
                    if (chunk_flag.test(ChunkFlag::zerod_out))
                    {
                        auto chunk_data = FileSystem::read_binary<uint8_t>(resource_path);
                        //chunk_information.writeBytes(chunk_data);
                        chunk_size_uncompressed = chunk_data.size();
                        chunk_size_compressed = k_none_size;
                    }
                    if (chunk_flag.test(ChunkFlag::copy_coded))
                    {
                        auto chunk_data = FileSystem::read_binary<uint8_t>(resource_path);
                        chunk_information.writeBytes(chunk_data);
                        chunk_size_uncompressed = chunk_data.size();
                        chunk_size_compressed = chunk_size_uncompressed;
                    }
                    if (chunk_flag.test(ChunkFlag::lzma))
                    {
                        auto chunk_data = FileSystem::read_binary<uint8_t>(resource_path);
                        chunk_information.writeBytes(Kernel::Definition::Compression::Lzma::compress<6>(chunk_data));
                        chunk_size_uncompressed = chunk_data.size();
                        chunk_size_compressed = chunk_size_uncompressed;
                    }
                    chunk_information_structure.size_compressed = static_cast<uint32_t>(chunk_size_compressed);
                    chunk_information_structure.size_uncompressed = static_cast<uint32_t>(chunk_size_uncompressed);
                    chunk_information_structure.flag = static_cast<uint16_t>(chunk_flag.to_ullong());
                    chunk_information_structure.file = 0_us;
                    information_structure.chunk_information.emplace_back(chunk_information_structure);
                    ++global_chunk_index;
                }
            }
            information_structure.chunk_setting.archive_resource_count = 1_us;
            information_structure.chunk_setting.chunk_count = static_cast<uint16_t>(global_chunk_index);
            information_structure.archive_setting.resource_file_count = static_cast<uint16_t>(information_structure.resource_file.size());
            information_structure.archive_setting.resource_directory_count = static_cast<uint16_t>(information_structure.resource_directory.size());
            exchange_archive_setting(information_structure.archive_setting, stream);
            for (auto &element : information_structure.resource_file)
            {
                stream.writeStringByEmpty(element);
            }
            for (auto i : Range(1_size, information_structure.resource_directory.size(), 1_size)) {
                auto &element = information_structure.resource_directory[i];
                stream.writeStringByEmpty(element);
            }
            for (auto &element : information_structure.resource_information)
            {
                exchange_resource_information(element, stream);
            }
            exchange_chunk_setting(information_structure.chunk_setting, stream);
            auto additional_offset = stream.write_pos + (information_structure.chunk_information.size() * 16_size);
            for (auto &element : information_structure.chunk_information)
            {
                element.offset += additional_offset;
                exchange_chunk_information(element, stream);
            }
            for (auto &element : information_structure.archive_resource_information)
            {
                // exchange_archive_resource_information(stream, element);
            }
            stream.writeBytes(chunk_information.toBytes());
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            Package const &definition,
            std::string_view source) -> void
        {
            process_packages(stream, definition, fmt::format("{}/resource", source));
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            auto definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            process_whole(stream, definition, source);
            stream.out_file(destination);
            return;
        }
    };

}