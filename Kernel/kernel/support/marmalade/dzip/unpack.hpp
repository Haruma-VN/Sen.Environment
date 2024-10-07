#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/marmalade/dzip/definition.hpp"
#include "kernel/support/marmalade/dzip/common.hpp"

namespace Sen::Kernel::Support::Marmalade::DZip
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Unpack : Common
    {
    private:
        inline static auto process_package(
            DataStreamView &stream,
            Package &definition,
            std::string const & resource_directory
        ) -> void
        {
            assert_conditional(stream.readUint32() == k_magic_identifier, fmt::format("{}", Language::get("marmalade.dzip.invalid_dzip_header")), "process_package");
            auto information_structure = Information{};
            exchange_archive_setting(stream, information_structure.archive_setting);
            information_structure.resource_file.resize(static_cast<size_t>(information_structure.archive_setting.resource_file_count));
            assert_conditional(information_structure.archive_setting.version == k_version, fmt::format("{}", Language::get("marmalade.dzip.invalid_dzip_version")), "process_package");
            for (auto &element : information_structure.resource_file) {
                element = stream.readStringByEmpty();
            }
            information_structure.resource_directory.resize(static_cast<size_t>(information_structure.archive_setting.resource_directory_count));
            for (auto i : Range(1_size, information_structure.resource_directory.size(), 1_size)) {
                auto &element = information_structure.resource_directory[i];
                element = stream.readStringByEmpty();
            }
            information_structure.resource_information.resize(static_cast<size_t>(information_structure.archive_setting.resource_file_count));
            for (auto &element : information_structure.resource_information) {
                exchange_resource_information(stream, element);
            }   
            exchange_chunk_setting(stream, information_structure.chunk_setting);
            information_structure.chunk_information.resize(static_cast<size_t>(information_structure.chunk_setting.chunk_count));
            for (auto &element : information_structure.chunk_information) {
                exchange_chunk_information(stream, element);
            }
            information_structure.archive_resource_information.resize(static_cast<size_t>(information_structure.chunk_setting.archive_resource_count) - 1_size);
            for (auto &element : information_structure.archive_resource_information) {
                //exchange_archive_resource_information(stream, element);
            }
            definition.resource.resize(information_structure.resource_information.size());
          //  auto package_data_end_position = stream.read_pos;
            for (auto resource_index : Range(information_structure.resource_information.size())) {
                auto & resource_information_structure = information_structure.resource_information[resource_index];
                auto & resource_definition = definition.resource[resource_index];
                resource_definition.path = String::to_posix_style(fmt::format("{}/{}", information_structure.resource_directory[static_cast<size_t>(resource_information_structure.directory_index)], information_structure.resource_file[resource_index]));
                resource_definition.chunk.resize(resource_information_structure.chunk_index.size());
                auto chunk_data_list = std::vector<std::vector<uint8_t>>{resource_information_structure.chunk_index.size()};
                for (auto chunk_index : Range(resource_information_structure.chunk_index.size())) {
                    auto & chunk_information_structure = information_structure.chunk_information[static_cast<size_t>(resource_information_structure.chunk_index[chunk_index])];
                    auto & chunk_definition = resource_definition.chunk[chunk_index];
					auto & chunk_data = chunk_data_list[chunk_index];
                    stream.read_pos = static_cast<size_t>(chunk_information_structure.offset);
                    chunk_data.resize(static_cast<size_t>(chunk_information_structure.size_uncompressed));
                    auto chunk_size_compressed = static_cast<size_t>(chunk_information_structure.size_compressed);
                    auto chunk_flag = std::bitset<ChunkFlag::k_count>{static_cast<u_long>(chunk_information_structure.flag)};
                    assert_conditional(!chunk_flag.test(ChunkFlag::unused_2), fmt::format("{}", Language::get("marmalade.dzip.invalid_bitset_unused_2")), "process_package");
                    auto chunk_ok = false;
                    if (chunk_flag.test(ChunkFlag::combuf)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(false, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_flag")), "process_package");
                    }
                    if (chunk_flag.test(ChunkFlag::dzip)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(false, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_flag")), "process_package");
                    }
                    if (chunk_flag.test(ChunkFlag::zlib)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(chunk_size_compressed == chunk_data.size(), String::format(fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_size")), std::to_string(chunk_size_compressed), std::to_string(chunk_data.size())), "process_package");
                        chunk_definition.flag = ChunkFlagEnum::zlib;
                        stream.read_pos += 10_size;
                        chunk_data = Kernel::Definition::Compression::Zlib::uncompress_deflate(stream.readBytes(chunk_size_compressed - 10_size));
                    }
                    if (chunk_flag.test(ChunkFlag::bzip2)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(chunk_size_compressed == chunk_data.size(), String::format(fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_size")), std::to_string(chunk_size_compressed), std::to_string(chunk_data.size())), "process_package");
                        chunk_definition.flag = ChunkFlagEnum::bzip2;
                        auto chunk_stream = DataStreamView{};
                        chunk_data = Kernel::Definition::Compression::Bzip2::uncompress(stream.readBytes(chunk_size_compressed));
                    }
                    if (chunk_flag.test(ChunkFlag::mp3)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(false, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_flag")), "process_package");
                    }
                    if (chunk_flag.test(ChunkFlag::jpeg)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(false, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_flag")), "process_package");
                    }
                    if (chunk_flag.test(ChunkFlag::zerod_out)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(chunk_size_compressed == k_none_size, String::format(fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_size")), std::to_string(chunk_size_compressed), std::to_string(chunk_data.size())), "process_package");
                        chunk_definition.flag = ChunkFlagEnum::zerod_out;
                    }
                    if (chunk_flag.test(ChunkFlag::copy_coded)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(chunk_size_compressed == chunk_data.size(), String::format(fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_size")), std::to_string(chunk_size_compressed), std::to_string(chunk_data.size())), "process_package");
                        chunk_definition.flag = ChunkFlagEnum::copy_coded;
                        auto chunk_stream = DataStreamView{};
                        chunk_data = stream.readBytes(chunk_size_compressed);
                    }
                    if (chunk_flag.test(ChunkFlag::lzma)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(chunk_size_compressed == chunk_data.size(), String::format(fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_size")), std::to_string(chunk_size_compressed), std::to_string(chunk_data.size())), "process_package");
                        chunk_definition.flag = ChunkFlagEnum::lzma;
                        auto chunk_stream = DataStreamView{};
                        chunk_data = Kernel::Definition::Compression::Lzma::uncompress<false>(stream.readBytes(chunk_size_compressed));
                    }
                    if (chunk_flag.test(ChunkFlag::random_access)) {
                        assert_conditional(!chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                        chunk_ok = true;
                        assert_conditional(false, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_flag")), "process_package");
                    }
                    assert_conditional(chunk_ok, fmt::format("{}", Language::get("marmalade.dzip.invalid_chunk_test")), "process_package");
                   // package_data_end_position = package_data_end_position > stream.read_pos ? package_data_end_position : stream.read_pos;
                }
                write_bytes(fmt::format("{}/{}", resource_directory, resource_definition.path), chunk_data_list.front());
            }
            return;
        }
    public:
        inline static auto process_whole(
            DataStreamView &stream,
            Package &definition,
            std::string_view destination) -> void
        {
            process_package(stream, definition, fmt::format("{}/resource", destination));
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = Package{};
            process_whole(stream, definition, destination);
            write_json(fmt::format("{}/data.json", destination), definition);
            return;
        }
    };
}