#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/package/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Package
{
    using namespace Definition;

    struct Pack : Common
    {
    protected:
        inline static auto process_package(
            DataStreamView &stream,
            typename PackageInfomartion const &definition,
            std::string_view resource_directory) -> void
        {
            if (definition.package_platform == PackagePlatformEnumeration::Type::tv_package_platform_version)
            {
                zip_package_compress(stream, resource_directory, definition.resource);
                return;
            }
            stream.writeInt32(Common::k_magic_package_identifier);
            stream.writeUint32(Common::k_version);
            auto resource_information_list = std::vector<ResourceInformation>{};
            auto resource_data_section_view_stored_list = std::vector<std::vector<uint8_t>>{};
            auto resource_texture_use_index_list = std::vector<size_t>{};
            for (auto &resource : definition.resource) {
                stream.writeUint8(ResourceInformationListStateFlag::next);
                stream.writeStringByUint8(resource.path);
                auto resource_data = FileSystem::read_binary<uint8_t>(fmt::format("{}/{}", resource_directory, resource.path));
                stream.writeUint32(resource_data.size());
                if (definition.package_platform == PackagePlatformEnumeration::Type::xbox_360_package_platform_version) {
                    if (compare_string(Path::getExtension(resource.path), ".ptx"_sv)) {
                        resource_texture_use_index_list.emplace_back(resource_data_section_view_stored_list.size());
                    }
                }
                if (definition.zlib_compression) {
                    auto compressed_resource_data = Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_6>(resource_data);
                    stream.writeUint32(compressed_resource_data.size());
                    resource_data_section_view_stored_list.emplace_back(std::move(compressed_resource_data));
                }
                else {
                    resource_data_section_view_stored_list.emplace_back(std::move(resource_data));
                }
                stream.writeUint64(resource.time);
            }
            stream.writeUint8(ResourceInformationListStateFlag::done);
            for (const auto & resource_index : Range(resource_data_section_view_stored_list.size())) {
                auto &resource_data = resource_data_section_view_stored_list[resource_index];
                auto padding_size = 0_us;
                if (definition.package_platform == PackagePlatformEnumeration::Type::xbox_360_package_platform_version && std::find(resource_texture_use_index_list.begin(), resource_texture_use_index_list.end(), resource_index) != resource_texture_use_index_list.end()) {
                    padding_size = static_cast<uint16_t>(compute_padding_size_fill_block(stream.write_pos));
                }
                else {
                    padding_size = static_cast<uint16_t>(compute_padding_size(stream.write_pos));
                }
                stream.writeUint16(padding_size);
                stream.writeNull(padding_size);
                stream.writeBytes(resource_data);
            }
            stream.write_pos = Common::k_none_size;
            if (definition.package_platform == PackagePlatformEnumeration::Type::pc_package_platform_version) {
                while (true)
                {
                    if (stream.write_pos >= stream.size()) {
                        break;
                    }
                    stream.writeUint8(static_cast<uint8_t>(stream.readUint8() ^ Common::xor_point));
                }
            }
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            typename PackageInfomartion const &definition,
            std::string_view source,
            std::string_view destination) -> void
        {
            auto resource_directory = fmt::format("{}/resource", source);
            process_package(stream, definition, resource_directory);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            auto definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            process_whole(stream, definition, source, destination);
            stream.out_file(destination);
            return;
        }
    };
}