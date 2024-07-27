#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/definition.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamGroup
{

    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Unpack : Common
    {
    protected:
        template <typename Args>
            requires std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value || std::is_same<Args, std::string_view>::value || std::is_same<Args, bool>::value
        inline static auto process_package(
            DataStreamView &stream,
            PacketStructure &definition,
            Args &args) -> void
        {
            auto information_structure_header = HeaderInformaiton{};
            exchange_header(stream, information_structure_header);
            assert_conditional(information_structure_header.magic == k_magic_identifier, fmt::format("{}", Language::get("popcap.rsg.unpack.invalid_rsg_magic")), "process_package"); 
            auto index = std::find(k_version_list.begin(), k_version_list.end(), static_cast<int>(information_structure_header.version));
            assert_conditional((index != k_version_list.end()),  String::format(fmt::format("{}", Language::get("popcap.rsg.invalid_version")), std::to_string(static_cast<int>(information_structure_header.version))), "process"); 
            definition.version = information_structure_header.version;
            auto resource_information_structure = std::map<std::string, ResourceInformation>{};
            CompiledMapData::decode(stream, information_structure_header.resource_information_section_offset, information_structure_header.resource_information_section_size, resource_information_structure, &exchange_to_resource_infomation);
            packet_compression_from_data(information_structure_header.resource_data_section_compression, definition.compression);
            definition.resource.reserve(resource_information_structure.size());
            auto resource_data_section_view_stored_map = std::unordered_map<std::string_view, std::vector<std::uint8_t>>{};
            if constexpr (!(std::is_same<Args, bool>::value))
            {
                for (auto &current_resource_type : std::vector<std::string_view>{k_general_type_string, k_texture_type_string})
                {
                    auto resource_data_section_offset = k_begin_index;
                    auto resource_data_section_size = k_none_size;
                    auto resource_data_section_size_original = k_none_size;
                    auto compress_resource_data_section = false;
                    switch (hash_sv(current_resource_type))
                    {
                    case hash_sv(k_general_type_string):
                    {
                        resource_data_section_offset = information_structure_header.general_resource_data_section_offset;
                        resource_data_section_size = information_structure_header.general_resource_data_section_size;
                        resource_data_section_size_original = information_structure_header.general_resource_data_section_size_original;
                        compress_resource_data_section = definition.compression.general;
                        break;
                    }
                    case hash_sv(k_texture_type_string):
                    {
                        resource_data_section_offset = information_structure_header.texture_resource_data_section_offset;
                        resource_data_section_size = information_structure_header.texture_resource_data_section_size;
                        resource_data_section_size_original = information_structure_header.texture_resource_data_section_size_original;
                        compress_resource_data_section = definition.compression.texture;
                        break;
                    }
                    }
                    auto resource_data_section_view_stored = stream.getBytes(static_cast<size_t>(resource_data_section_offset), static_cast<size_t>(resource_data_section_offset + resource_data_section_size));
                    if (!compress_resource_data_section)
                    {
                        resource_data_section_view_stored_map[current_resource_type] = std::move(resource_data_section_view_stored);
                    }
                    else
                    {
                        if (resource_data_section_size_original != k_none_size)
                        {
                            resource_data_section_view_stored_map[current_resource_type] = std::move(Definition::Compression::Zlib::uncompress(resource_data_section_view_stored));
                        }
                    }
                }
            }
            for (auto &[resource_path, resource_information] : resource_information_structure)
            {
                auto resource_info = Resource{
                    .path = String::to_posix_style(resource_path)};
                auto resource_type_string = k_general_type_string;
                if (resource_information.read_texture_additional)
                {
                    resource_info.use_texture_additional_instead = true;
                    resource_type_string = k_texture_type_string;
                    resource_info.texture_additional = TextureResourceAdditional{
                        .value = TextureValue{
                            .index = resource_information.texture_value.index,
                            .dimension = TextureDimension{
                                .width = resource_information.texture_value.width,
                                .height = resource_information.texture_value.height}}};
                }
                definition.resource.emplace_back(resource_info);
                if constexpr (!(std::is_same<Args, bool>::value))
                {
                    auto resource_data = sub_bytes(resource_data_section_view_stored_map[resource_type_string], resource_information.resource_data_section_offset, resource_information.resource_data_section_size);
                    if constexpr (std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value)
                    {
                        args[resource_path] = std::move(resource_data);
                    }
                    if constexpr (std::is_same<Args, std::string_view>::value)
                    {
                        write_bytes(fmt::format("{}/{}", args, resource_path), resource_data);
                    }
                }
            }
            return;
        }

    public:
        template <typename Args>
            requires std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value || std::is_same<Args, std::string_view>::value || std::is_same<Args, bool>::value
        inline static auto process_whole(
            DataStreamView &stream,
            PacketStructure &definition,
            Args &args) -> void
        {
            if constexpr (std::is_same<Args, std::string_view>::value)
            {
                auto resource_directory = std::string_view{fmt::format("{}/resource", args)};
                process_package(stream, definition, resource_directory);
            }
            else 
            {
                process_package(stream, definition, args);
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = PacketStructure{};
            process_whole(stream, definition, destination);
            FileSystem::write_json(fmt::format("{}/data.json", destination), definition);
            return;
        }
    };

}