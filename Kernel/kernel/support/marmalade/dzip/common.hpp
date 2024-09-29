#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"

namespace Sen::Kernel::Support::Marmalade::DZip
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Common {
        inline static constexpr auto k_magic_identifier = 1515344964_ui;

        inline static constexpr auto k_version = 0_ui;

        struct ChunkFlag
        {
            inline static constexpr auto k_count = 11_size;

            //--------------------

            inline static constexpr auto combuf = 0_size;
            
            inline static constexpr auto unused_2 = 1_size;
            
            inline static constexpr auto dzip = 2_size;
            
            inline static constexpr auto zlib = 3_size;
            
            inline static constexpr auto bzip2 = 4_size;
            
            inline static constexpr auto mp3 = 5_size;
            
            inline static constexpr auto jpeg = 6_size;

            inline static constexpr auto zerod_out = 7_size;

            inline static constexpr auto copy_coded = 8_size;

            inline static constexpr auto lzma = 9_size;

            inline static constexpr auto random_access = 10_size;
        };


        struct ArchiveSetting {
            uint16_t resource_file_count;
            uint16_t resource_directory_count;
            uint8_t version;
        };

        inline static auto exchange_archive_setting(
            DataStreamView &stream, 
            ArchiveSetting &value
        ) -> void
        {
            value.resource_file_count = stream.readUint16();
            value.resource_directory_count = stream.readUint16();
            value.version = stream.readUint8();
            return;
        }

        inline static auto exchange_archive_setting(
            ArchiveSetting const &data,
            DataStreamView &stream
        ) -> void
        {
            stream.writeUint16(data.resource_file_count);
            stream.writeUint16(data.resource_directory_count);
            stream.writeUint8(data.version);
            return;
        }

        struct ResourceInformation {
            uint16_t directory_index;
            std::vector<uint16_t> chunk_index;
        };


        inline static auto exchange_resource_information(
            DataStreamView &stream, 
            ResourceInformation &value
        ) -> void
        {
            value.directory_index = stream.readUint16();
            while (true) {
                auto chunk_index = stream.readUint16();
                if (chunk_index == 0xFFFF_us) {
                    break;
                }
                value.chunk_index.emplace_back(chunk_index);
            }
            return;
        }

        inline static auto exchange_resource_information(
            ResourceInformation const &value,
            DataStreamView &stream
        ) -> void
        {
            stream.writeUint16(value.directory_index);
            for (auto &chunk_index : value.chunk_index) {
              stream.writeUint16(chunk_index);  
            }
            stream.writeUint16(0xFFFF_us);  
            return;
        }

        struct ChunkSetting {
            uint16_t archive_resource_count;
            uint16_t chunk_count;
        };
        

        inline static auto exchange_chunk_setting(
            DataStreamView &stream, 
            ChunkSetting &value
        ) -> void
        {
            value.archive_resource_count = stream.readUint16();
            value.chunk_count = stream.readUint16();
            return;
        }

        inline static auto exchange_chunk_setting(
            ChunkSetting &value,
            DataStreamView &stream
        ) -> void
        {
            stream.writeUint16(value.archive_resource_count);
            stream.writeUint16(value.chunk_count);
            return;
        }


        struct ChunkInformation {
            uint32_t offset;
            uint32_t size_compressed;
            uint32_t size_uncompressed;
            uint16_t flag;
            uint16_t file;
        };

        inline static auto exchange_chunk_information(
            DataStreamView &stream, 
            ChunkInformation &value
        ) -> void
        {
            value.offset = stream.readUint32();
            value.size_compressed = stream.readUint32();
            value.size_uncompressed = stream.readUint32();
            value.flag = stream.readUint16();
            value.file = stream.readUint16();
            return;
        }

        inline static auto exchange_chunk_information(
            ChunkInformation &value,
            DataStreamView &stream
        ) -> void
        {
            stream.writeUint32(value.offset);
            stream.writeUint32(value.size_compressed);
            stream.writeUint32(value.size_uncompressed);
            stream.writeUint16(value.flag);
            stream.writeUint16(value.file);
            return;
        }

        struct ArchiveResourceInformation {

        };
        

        struct Information {
            ArchiveSetting archive_setting;
            std::vector<std::string> resource_file;
            std::vector<std::string> resource_directory;
            std::vector<ResourceInformation> resource_information;
            ChunkSetting chunk_setting;
            std::vector<ChunkInformation> chunk_information;
            std::vector<ArchiveResourceInformation> archive_resource_information;
        };
    };
}