#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/package/definition.hpp"
#include "kernel/support/popcap/package/common.hpp"

namespace Sen::Kernel::Support::PopCap::Package
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Unpack : Common
    {
    protected:

        inline static auto process_package(
            DataStreamView &stream,
            typename PackageInfomartion &definition,
            std::string_view resource_directory) -> void
        {
            auto k_magic_package = stream.readInt32();
            stream.read_pos = k_none_size;
            switch (k_magic_package)
            {
            case k_magic_package_identifier:
            {
                definition.package_platform = PackagePlatformEnumeration::Type::raw_package_platform_version;
                break;
            }
            case k_magic_package_pc_version_identifier:
            {
                definition.package_platform = PackagePlatformEnumeration::Type::pc_package_platform_version;
                stream.write_pos = k_begin_index;
                while (true)
                {
                    if (stream.read_pos >= stream.size())
                    {
                        stream.read_pos = k_none_size;
                        break;
                    }
                    stream.writeUint8(static_cast<uint8_t>(stream.readUint8() ^ xor_point));
                }
                break;
            }
            case k_magic_package_tv_version_identifier:
            {
                definition.package_platform = PackagePlatformEnumeration::Type::tv_package_platform_version;
                zip_package_uncompress(stream.toBytes(), resource_directory, definition.resource);
                return;
            }
            case k_magic_package_xmem_compression_identifier: {
                assert_conditional(false, fmt::format("{}", Language::get("popcap.package.invalid_xbfile")), "process_package"); 
            }
            default:
                assert_conditional(false, String::format(fmt::format("{}", Language::get("popcap.package.invalid_package_platform")), std::to_string(k_magic_package)), "process_package");
            }
            assert_conditional(stream.readInt32() == k_magic_package_identifier, fmt::format("{}", Language::get("popcap.pak.unpack.invalid_pak_header")), "process_package");
            {
                auto version = stream.readUint32();
                assert_conditional(version == k_version, String::format(fmt::format("{}", Language::get("popcap.package.invalid_version")), std::to_string(version)), "process_package");
            }
            auto resource_information_list = std::vector<ResourceInformation>{};
            while (true)
            {
                auto flag = stream.readUint8();
                if (flag == ResourceInformationListStateFlag::done)
                {
                    break;
                }
                if (flag == ResourceInformationListStateFlag::next)
                {
                    auto resource_information = ResourceInformation{
                        .path = stream.readStringByUint8(),
                        .size = stream.readUint32(),
                        .time = stream.readUint64()};
                    resource_information_list.emplace_back(resource_information);
                    continue;
                }
                resource_information_list.clear();
                definition.zlib_compression = true;
                stream.read_pos = size_t{0x08};
                break;
            }
            if (resource_information_list.empty())
            {
                while (true)
                {
                    auto flag = stream.readUint8();
                    if (flag == ResourceInformationListStateFlag::done)
                    {
                        break;
                    }
                    if (flag == ResourceInformationListStateFlag::next)
                    {
                        auto resource_information = ResourceInformation{
                            .path = stream.readStringByUint8(),
                            .size = stream.readUint32(),
                            .size_original = stream.readUint32(),
                            .time = stream.readUint64()};
                        resource_information_list.emplace_back(resource_information);
                        continue;
                    }
                    assert_conditional(false, fmt::format("{}", Language::get("popcap.package.unpack_failed")), "process_package");
                }
            }
            for (auto &resource_information : resource_information_list)
            {
                if (definition.package_platform != PackagePlatformEnumeration::Type::pc_package_platform_version)
                {
                    auto skip_point_size = stream.readUint16();
                    stream.read_pos += static_cast<size_t>(skip_point_size);
                    auto is_xbox_360_package_platform = static_cast<bool>(false | (skip_point_size > 8));
                    if (is_xbox_360_package_platform)
                    {
                        definition.package_platform = PackagePlatformEnumeration::Type::xbox_360_package_platform_version;
                    }
                }
                definition.resource.emplace_back(Resource{
                    .path = resource_information.path,
                    .time = resource_information.time});
                auto resource_data = stream.readBytes(resource_information.size);
                write_bytes(fmt::format("{}/{}", resource_directory, resource_information.path), definition.zlib_compression ? Definition::Compression::Zlib::uncompress(resource_data) : resource_data);
            }
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            typename PackageInfomartion &definition,
            std::string_view source,
            std::string_view destination) -> void
        {
            auto resource_directory = fmt::format("{}/resource", destination);
            process_package(stream, definition, resource_directory);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = PackageInfomartion{};
            process_whole(stream, definition, source, destination);
            write_json(fmt::format("{}/data.json", destination), definition);
            return;
        }
    };
}