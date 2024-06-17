#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/definition.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamGroup
{

    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Pack : Common
    {
    protected:
        template <typename Args>
            requires std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value || std::is_same<Args, std::string_view>::value
        inline static auto process_package(
            DataStreamView &stream,
            PacketStructure const &definition,
            Args args) -> void
        {
            auto index = std::find(k_version_list.begin(), k_version_list.end(), static_cast<int>(definition.version));
            assert_conditional((index != k_version_list.end()), "invalid_rsg_version", "process"); // TODO: add to localization.
            auto resource_information_structure = std::map<std::string, ResourceInformation>{};
            auto resource_data_section_view_stored = std::map<std::string_view, DataStreamView>{};
            for (auto &resource_definition : definition.resource)
            {
                auto resource_definition_path = String::to_windows_style(resource_definition.path);
                auto resource_data = std::vector<uint8_t>{};
                if constexpr (std::is_same_v<Args, std::map<std::string, std::vector<uint8_t>>>)
                {
                    resource_data = std::move(args.at(resource_definition_path));
                }
                if constexpr (std::is_same_v<Args, std::string_view>)
                {
                    resource_data = std::move(FileSystem::read_binary<uint8_t>(fmt::format("{}/{}", args, resource_definition_path)));
                }
                auto resource_information = ResourceInformation{};
                auto current_resource_type = k_general_type_string;
                if (resource_definition.use_texture_additional_instead)
                {
                    current_resource_type = k_texture_type_string;
                    resource_information.read_texture_additional = true;
                    resource_information.texture_value.index = resource_definition.texture_additional.value.index;
                    resource_information.texture_value.width = resource_definition.texture_additional.value.dimension.width;
                    resource_information.texture_value.height = resource_definition.texture_additional.value.dimension.height;
                }
                resource_information.resource_data_section_offset = resource_data_section_view_stored[current_resource_type].write_pos;
                resource_information.resource_data_section_size = resource_data.size();
                resource_data_section_view_stored[current_resource_type].writeBytes(resource_data);
                resource_data_section_view_stored[current_resource_type].writeNull(compute_padding_size(resource_data_section_view_stored[current_resource_type].size(), k_padding_unit_size));
                resource_information_structure[resource_definition_path] = resource_information;
            }
            stream.writeNull(information_header_section_size);
            auto information_structure_header = HeaderInformaiton{
                .resource_information_section_offset = static_cast<uint32_t>(stream.write_pos)
            };
            CompiledMapData::encode(stream, resource_information_structure, &exchange_from_resource_infomation);
            information_structure_header.resource_information_section_size = stream.write_pos - information_structure_header.resource_information_section_offset;
            stream.writeNull(compute_padding_size(stream.write_pos, k_padding_unit_size));
            information_structure_header.information_section_size = static_cast<uint32_t>(stream.write_pos);
            packet_compression_to_data(information_structure_header.resource_data_section_compression, definition.compression);
            for (auto &current_resource_type : std::vector<std::string_view>{k_general_type_string, k_texture_type_string})
            {
                auto resource_data = resource_data_section_view_stored.contains(current_resource_type) ? resource_data_section_view_stored[current_resource_type].toBytes() : std::vector<uint8_t>{};
                auto resource_padding_size = k_none_size;
                auto resource_data_section_offset = static_cast<uint32_t>(stream.write_pos);
                auto resource_data_section_size_original = static_cast<uint32_t>(resource_data.size());
                switch (hash_sv(current_resource_type))
                {
                case hash_sv(k_general_type_string):
                {
                    if (definition.compression.general)
                    {
                        resource_data = std::move(Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_9>(resource_data));
                    }
                    resource_padding_size = compute_padding_size(stream.write_pos + resource_data.size(), k_padding_unit_size);
                    information_structure_header.general_resource_data_section_offset = resource_data_section_offset;
                    information_structure_header.general_resource_data_section_size = static_cast<uint32_t>(resource_data.size() + resource_padding_size);
                    information_structure_header.general_resource_data_section_size_original = resource_data_section_size_original;
                    break;
                }
                case hash_sv(k_texture_type_string):
                {
                    if (definition.compression.texture && resource_data.size() != k_none_size)
                    {
                        resource_data = std::move(Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_9>(resource_data));
                    }
                    resource_padding_size = compute_padding_size(stream.write_pos + resource_data.size(), k_padding_unit_size);
                    information_structure_header.texture_resource_data_section_offset = resource_data_section_offset;
                    information_structure_header.texture_resource_data_section_size = static_cast<uint32_t>(resource_data.size() + resource_padding_size);
                    information_structure_header.texture_resource_data_section_size_original = resource_data_section_size_original;
                    break;
                }
                }
                stream.writeBytes(resource_data);
                stream.writeNull(resource_padding_size);
            }
            information_structure_header.magic = Common::k_magic_identifier;
            information_structure_header.version = definition.version;
            exchange_header(information_structure_header, stream);
            return;
        }

    public:
        template <typename Args>
            requires std::is_same<Args, std::map<std::string, std::vector<uint8_t>>>::value || std::is_same<Args, std::string_view>::value
        inline static auto process_whole(
            DataStreamView &stream,
            PacketStructure const &value,
            Args args) -> void
        {
            if constexpr (std::is_same_v<Args, std::map<std::string, std::vector<uint8_t>>>)
            {
                process_package(stream, value, args);
            }
            if constexpr (std::is_same_v<Args, std::string_view>)
            {
                process_package(stream, value, std::string_view{fmt::format("{}/Resource", args)});
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            auto definition = *FileSystem::read_json(fmt::format("{}/definition.json", source));
            process_whole(stream, definition, source);
            stream.out_file(destination);
            return;
        }
    };

}