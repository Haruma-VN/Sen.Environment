#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup
{
    using namespace Definition;

    using PacketCompression = Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::PacketCompression;

    struct Decode : Common
    {

    private:
        inline static auto exchange_nessessory_data(
            std::map<std::string, SubgroupRawContent> const &subgroup_content_map,
            std::map<std::string, PacketInformation> &packet_information) -> void
        {
            for (auto &[subgroup_id, subgroup_value] : subgroup_content_map)
            {
                auto &packet_info = packet_information[subgroup_id];
                packet_info.is_image = subgroup_value.is_image;
                auto packet_stream = DataStreamView{subgroup_value.packet_data};
                Sen::Kernel::Support::PopCap::ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_info.packet_structure, packet_info.resource_data_section_view_stored);
                packet_info.subgroup_content_information = subgroup_value.info;
            }
            return;
        }

        inline static auto exchange_unpack_debug(
            PacketOriginalInformation const &packet_original_information,
            std::string_view destination) -> void
        {
            auto definition = DebuggerInformation{
                .version = packet_original_information.version,
                .texture_format_category = packet_original_information.texture_format_category,
                .composite = packet_original_information.composite};
            for (auto &[subgroup_id, subgroup_value] : packet_original_information.subgroup)
            {
                auto packet_path = fmt::format("packet/{}.rsg", subgroup_id);
                auto info_path = fmt::format("resource/{}.json", subgroup_id);
                write_bytes(fmt::format("{}/{}", destination, packet_path), subgroup_value.packet_data);
                write_json(fmt::format("{}/{}", destination, info_path), subgroup_value.info);
                definition.subgroup[subgroup_id] = DebuggerSubgroupInformation{
                    .is_image = subgroup_value.is_image,
                    .packet_path = packet_path,
                    .info_path = info_path};
            }
            write_json(fmt::format("{}/data.json", destination), definition);
            return;
        }

        // ---------------------------------------------------------------------------------------

        inline static auto exchange_subgroup_id(
            std::string const &subgroup_id,
            int const &resolution) -> std::string
        {
            auto res = resolution != static_cast<int>(k_none_size) ? fmt::format("_{}", resolution) : "_common";
            if (auto index = tolower_back(subgroup_id).find(res); index != std::string::npos)
            {
                auto result = subgroup_id;
                result.erase(index, res.size());
                return result;
            }
            return subgroup_id;
        }

        inline static auto exchange_compression(
            PacketCompression const &data,
            PacketCompression &value) -> void
        {
            if (data.general)
            {
                value.general = data.general;
            }
            if (data.texture)
            {
                value.texture = data.texture;
            }
            return;
        }

        inline static auto exchange_texture_simple(
            std::map<std::string, PacketInformation> &general_information,
            std::map<std::string, PacketInformation> &packet_information,
            InformationStructure &definition,
            std::string const &resource_destination) -> void
        {
            auto highest_resolution = definition.category.resolution.front();
            auto packet_compression = PacketCompression{};
            auto common_subgroup_id_list = std::vector<std::string>{};
            for (auto &[id, packet_value] : packet_information)
            {
                auto subgroup_id = exchange_subgroup_id(id, packet_value.subgroup_content_information.texture.resolution);
                exchange_compression(packet_value.packet_structure.compression, packet_compression);
                if (packet_value.is_image)
                {
                    if (highest_resolution != packet_value.subgroup_content_information.texture.resolution)
                    {
                        continue;
                    }
                    auto image_index = k_begin_index;
                    auto image_information = SubgroupCompressedInfo{};
                    image_information.texture.resolution = packet_value.subgroup_content_information.texture.resolution;
                    for (auto &[packet_id, packet_info] : packet_value.subgroup_content_information.texture.packet)
                    {
                        auto image_id_index = exchange_image_id_index(subgroup_id, image_index);
                        auto &image_info = image_information.texture.packet[toupper_back(fmt::format("{}_{}", k_default_texture_packet_id, image_id_index))];
                        image_info = packet_info;
                        definition.category.format = packet_info.additional.format;
                        for (auto &[resource_path, resource_data] : packet_value.resource_data_section_view_stored)
                        {
                            if (compare_string(String::to_posix_style(resource_path), fmt::format("{}.ptx", packet_info.path)))
                            {
                                image_info.path = fmt::format("atlases/{}", image_id_index);
                                for (auto &[data_id, data_value] : image_info.data)
                                {
                                    image_info.data[data_id].path = exchange_image_path(data_value.path);
                                    auto &texture_info = data_value.texture_info;
                                    if (texture_info.ax + texture_info.aw > packet_info.dimension.width)
                                    {
                                        image_info.data[data_id].texture_info.aw = packet_info.dimension.width - texture_info.ax;
                                    }
                                    if (texture_info.ay + texture_info.ah > packet_info.dimension.height)
                                    {
                                        image_info.data[data_id].texture_info.ah = packet_info.dimension.height - texture_info.ay;
                                    }
                                }
                                auto image = Sen::Kernel::Support::Texture::InvokeMethod::decode_whole(resource_data, packet_info.dimension.width, packet_info.dimension.height, exchange_image_format(definition.texture_format_category, packet_info.additional.format));
                                write_image(fmt::format("{}/{}.png", resource_destination, image_info.path), image);
                                packet_value.resource_data_section_view_stored.erase(resource_path);
                                ++image_index;
                                break;
                            }
                        }
                    }
                    auto result = nlohmann::ordered_json{};
                    exchange_subgroup_compression_info(image_information, result);
                    write_json(fmt::format("{}/{}/{}.json", resource_destination, k_atlases_folder_string, subgroup_id), result); // atlases
                    auto &info = definition.subgroup[subgroup_id].resource[subgroup_id];
                    info.type = DataType::ImageData;
                    info.path = fmt::format("{}/{}.json", k_atlases_folder_string, subgroup_id);
                    if (std::find(common_subgroup_id_list.begin(), common_subgroup_id_list.end(), subgroup_id) == common_subgroup_id_list.end())
                    {
                        common_subgroup_id_list.emplace_back(subgroup_id);
                    }
                }
                else
                {
                    if (subgroup_id == id)
                    {
                        general_information[subgroup_id] = packet_value;
                        continue;
                    }
                    auto &packet_info = definition.subgroup[subgroup_id];
                    for (auto &[data_id, data_value] : packet_value.subgroup_content_information.general.data)
                    {
                        auto resource_information = ResourceInformation{
                            .type = data_value.type,
                            .path = data_value.path};
                        if (is_program_path(data_value.path))
                        {
                            packet_info.resource[data_id] = resource_information;
                            continue;
                        }
                        else
                        {
                            for (auto &[resource_path, resource_data] : packet_value.resource_data_section_view_stored)
                            {
                                if (compare_string(String::to_posix_style(resource_path), data_value.path))
                                {
                                    resource_information.path = resource_information.type == DataType::PopAnim ? exchange_animation_path(data_value.path) : data_value.path;
                                    trim_string(resource_information.path);
                                    decode_popcap_file<false>(resource_data, resource_information, resource_destination);
                                    packet_info.resource[data_id] = resource_information;
                                    packet_value.resource_data_section_view_stored.erase(resource_path);
                                    if (std::find(common_subgroup_id_list.begin(), common_subgroup_id_list.end(), subgroup_id) == common_subgroup_id_list.end())
                                    {
                                        common_subgroup_id_list.emplace_back(subgroup_id);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            for (auto &subgroup_id : common_subgroup_id_list)
            {
                auto &subgroup_value = definition.subgroup[subgroup_id];
                subgroup_value.category.common_type = true;
                Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_to_data(subgroup_value.category.compression, packet_compression);
            }
            return;
        }

        inline static auto exchange_animation(
            std::vector<uint8_t> const &resource_data,
            Sen::Kernel::Support::PopCap::Animation::Convert::ExtraInfo &extra,
            ResourceInformation &data_info,
            std::string const &resource_destination,
            bool const &animation_split_label) -> void
        {
            trim_string(data_info.path);
            auto extension = Kernel::Path::getExtension(data_info.path);
            assert_conditional(data_info.type == DataType::PopAnim, String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_popanim_type")), data_info.path), "exchange_animation");
            assert_conditional(compare_string(extension, ".pam"_sv), String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_pam_file")), data_info.path), "exchange_animation");
            auto stream = DataStreamView{resource_data};
            assert_conditional(try_fix_popanim(stream), String::format(fmt::format("{}", Language::get("pvz2.scg.pam_is_corrupted")), data_info.path), "exchange_animation");
            auto animation = Sen::Kernel::Support::PopCap::Animation::SexyAnimation{};
            Sen::Kernel::Support::PopCap::Animation::Decode::process_whole(stream, animation);
            data_info.path = exchange_animation_path(data_info.path);
            exchange_path(data_info.path, ".pam"_sv, ""_sv);
            trim_right(data_info.path);
            auto animation_destination = fmt::format("{}/{}", resource_destination, data_info.path);
            if (animation_split_label)
            {
                Sen::Kernel::Support::PopCap::Animation::Convert::ToFlash<true>::process_whole(animation, extra, animation_destination);
            }
            else
            {
                Sen::Kernel::Support::PopCap::Animation::Convert::ToFlash<false>::process_whole(animation, extra, animation_destination);
            }
            return;
        }

        inline static auto exchange_image_sprite(
            std::map<string, ImageSpriteInfo> &texture_sprite_view_stored,
            Sen::Kernel::Support::PopCap::Animation::Convert::ExtraInfo &extra,
            std::string const &destination) -> void
        {
            for (auto &[image_name, image_value] : extra.image)
            {
                if (texture_sprite_view_stored.contains(image_value.id))
                {
                    auto image_destination = destination;
                    auto &image_info = texture_sprite_view_stored[image_value.id];
                    auto image_path = (!image_value.path.empty() ? image_value.path : image_name);
                    if (image_path.starts_with("../"_sv))
                    { // fix chinese pam
                        /*
                        image_destination = Path::getParents(image_destination);
                        image_path = image_path.substr("../"_sv.size(), image_path.size() - "../"_sv.size());
                        */
                        image_path = Path::getFileNameWithoutExtension(image_path);
                        if (image_path == image_name)
                        {
                            image_path = image_name;
                            extra.image[image_name].path.clear();
                        }
                        else
                        {
                            extra.image[image_name].path = image_path;
                        }
                        Sen::Kernel::Support::PopCap::Animation::Convert::Common::fix_media_image(fmt::format("{}/library/image/{}.xml", destination, image_name), image_path);
                    }
                    write_image(fmt::format("{}/library/media/{}.png", destination, image_path), image_info.data);
                    extra.image[image_name].additional = image_info.additional;
                    extra.image[image_name].use_image_additional = true;
                    image_info.is_use = true;
                    // texture_sprite_view_stored.erase(image_value.id);
                }
                /*
                else {
                    auto missing_image = Definition::Image<int>::transparent(Definition::Dimension{image_value.size.width, image_value.size.height});
                    write_image(fmt::format("{}/library/media/{}.png", destination, (!image_value.name.empty() ? image_value.name : image_name)), missing_image);
                    extra.image[image_name].comment = k_missing_image_comment_string;
                    extra.image[image_name].use_image_additional = true;
                }
                */
            }
            return;
        }

        inline static auto exchange_texture_advanced(
            std::map<std::string, PacketInformation> &general_information,
            std::map<std::string, PacketInformation> &packet_information,
            InformationStructure &definition,
            std::string const &resource_destination,
            bool const &animation_split_label) -> void
        {
            auto highest_resolution = definition.category.resolution.front();
            auto packet_compression = PacketCompression{};
            auto texture_sprite_view_stored = std::map<string, ImageSpriteInfo>{};
            auto common_subgroup_id_list = std::vector<std::string>{};
            for (auto &[id, packet_value] : packet_information)
            {
                if (packet_value.is_image)
                {
                    auto subgroup_id = exchange_subgroup_id(id, packet_value.subgroup_content_information.texture.resolution);
                    exchange_compression(packet_value.packet_structure.compression, packet_compression);
                    if (highest_resolution != packet_value.subgroup_content_information.texture.resolution)
                    {
                        continue;
                    }
                    for (auto &[packet_id, packet_info] : packet_value.subgroup_content_information.texture.packet)
                    {
                        definition.category.format = packet_info.additional.format;
                        for (auto &[resource_path, resource_data] : packet_value.resource_data_section_view_stored)
                        {
                            if (compare_string(String::to_posix_style(resource_path), fmt::format("{}.ptx", packet_info.path)))
                            {
                                auto image = Sen::Kernel::Support::Texture::InvokeMethod::decode_whole(resource_data, packet_info.dimension.width, packet_info.dimension.height, exchange_image_format(definition.texture_format_category, packet_info.additional.format));
                                exchange_image_split(texture_sprite_view_stored, image, subgroup_id, packet_info);
                                packet_value.resource_data_section_view_stored.erase(resource_path);
                                break;
                            }
                        }
                    }
                }
            }
            for (auto &[id, packet_value] : packet_information)
            {
                if (!packet_value.is_image)
                {
                    auto subgroup_id = exchange_subgroup_id(id, packet_value.subgroup_content_information.texture.resolution);
                    if (subgroup_id == id)
                    {
                        general_information[subgroup_id] = packet_value;
                        continue;
                    }
                    exchange_compression(packet_value.packet_structure.compression, packet_compression);
                    auto &packet_info = definition.subgroup[subgroup_id];
                    for (auto &[data_id, data_value] : packet_value.subgroup_content_information.general.data)
                    {
                        auto resource_information = ResourceInformation{
                            .type = data_value.type,
                            .path = data_value.path};
                        if (is_program_path(data_value.path))
                        {
                            packet_info.resource[data_id] = resource_information;
                            continue;
                        }
                        else
                        {
                            for (auto &[resource_path, resource_data] : packet_value.resource_data_section_view_stored)
                            {
                                if (compare_string(String::to_posix_style(resource_path), data_value.path))
                                {
                                    switch (data_value.type)
                                    {
                                    case DataType::PopAnim:
                                    {
                                        auto extra = Sen::Kernel::Support::PopCap::Animation::Convert::ExtraInfo{
                                            .resolution = highest_resolution};
                                        exchange_animation(resource_data, extra, resource_information, resource_destination, animation_split_label);
                                        auto animation_destination = fmt::format("{}/{}", resource_destination, resource_information.path);
                                        exchange_image_sprite(texture_sprite_view_stored, extra, animation_destination);
                                        write_json(fmt::format("{}/data.json", animation_destination), extra);
                                        break;
                                    }
                                    default:
                                    {
                                        decode_popcap_file<false>(resource_data, resource_information, resource_destination);
                                        break;
                                    }
                                    }
                                    if (std::find(common_subgroup_id_list.begin(), common_subgroup_id_list.end(), subgroup_id) == common_subgroup_id_list.end())
                                    {
                                        common_subgroup_id_list.emplace_back(subgroup_id);
                                    }
                                    packet_info.resource[data_id] = resource_information;
                                    packet_value.resource_data_section_view_stored.erase(resource_path);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            for (auto &[image_id, image_value] : texture_sprite_view_stored)
            {
                if (!image_value.is_use)
                {
                    write_image(fmt::format("{}/{}.png", resource_destination, image_value.path), image_value.data);
                    definition.subgroup[image_value.subgroup_id].resource[image_id] = ResourceInformation{
                        .type = DataType::Image, // always image
                        .path = fmt::format("{}.png", image_value.path),
                        .use_image_additional = true,
                        .additional = image_value.additional};
                    if (std::find(common_subgroup_id_list.begin(), common_subgroup_id_list.end(), image_value.subgroup_id) == common_subgroup_id_list.end())
                    {
                        common_subgroup_id_list.emplace_back(image_value.subgroup_id);
                    }
                }
            }
            for (auto &subgroup_id : common_subgroup_id_list)
            {
                auto &subgroup_value = definition.subgroup[subgroup_id];
                subgroup_value.category.common_type = true;
                Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_to_data(subgroup_value.category.compression, packet_compression);
            }
            return;
        }

        inline static auto exchange_texture_additional(
            std::map<std::string, PacketInformation> &general_information,
            std::map<std::string, PacketInformation> &packet_information,
            InformationStructure &definition,
            std::string const &resource_destination,
            Setting const &setting) -> void
        {
            auto &resolution_list = definition.category.resolution;
            for (auto &[subgroup_id, packet_value] : packet_information)
            {
                if (packet_value.is_image)
                {
                    if (std::find(resolution_list.begin(), resolution_list.end(), packet_value.subgroup_content_information.texture.resolution) == resolution_list.end())
                    {
                        resolution_list.emplace_back(packet_value.subgroup_content_information.texture.resolution);
                    };
                }
            }
            if (resolution_list.size() == k_none_size)
            {
                definition.composite = false;
                exchange_general_additional(packet_information, definition, resource_destination, setting);
                return;
            }
            else
            {
                std::sort(resolution_list.begin(), resolution_list.end(), std::greater<int>());
            }
            switch (setting.decode_method)
            {
            case DecodeMethod::Advanced:
            {
                exchange_texture_advanced(general_information, packet_information, definition, resource_destination, setting.animation_split_label);
                break;
            }
            default:
            {
                exchange_texture_simple(general_information, packet_information, definition, resource_destination);
                break;
            }
            }
            return;
        }

        // ---------------------------------------------------------------------------------------

        template <auto decode_method_advanced>
        inline static auto decode_popcap_file(
            std::vector<uint8_t> const &resource_data,
            ResourceInformation &resource_information,
            std::string const &resource_destination) -> void
        {
            static_assert(decode_method_advanced == true || decode_method_advanced == false, "decode_method_advanced must be true or false");
            trim_string(resource_information.path);
            auto extension = Kernel::Path::getExtension(resource_information.path);
            if constexpr (decode_method_advanced)
            {
                if (compare_string(extension, ".rton"_sv) && resource_information.type == DataType::File)
                {
                    resource_information.type = DataType::Data;
                }
                switch (resource_information.type)
                {
                case DataType::Data:
                {
                    assert_conditional(compare_string(extension, ".rton"_sv), String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_rton_file")), resource_information.path), "decode_popcap_file");
                    auto writer = JsonWriter{};
                    writer.WriteIndent = true;
                    auto stream = DataStreamView{resource_data};
                    Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Decode::process_whole(stream, writer);
                    exchange_path(resource_information.path, ".rton"_sv, ".json"_sv);
                    write_text(fmt::format("{}/{}", resource_destination, resource_information.path), writer.ToString());
                    break;
                }
                case DataType::SoundBank:
                case DataType::DecodedSoundBank:
                {
                    assert_conditional(compare_string(extension, ".bnk"_sv), String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_bnk_file")), resource_information.path), "decode_popcap_file");
                    auto stream = DataStreamView{resource_data};
                    exchange_path(resource_information.path, ".bnk"_sv, ""_sv);
                    auto soundbank_definition = Sen::Kernel::Support::WWise::SoundBank::SoundBankInformation{};
                    auto soundbank_dest = fmt::format("{}/{}", resource_destination, resource_information.path);
                    Sen::Kernel::Support::WWise::SoundBank::Decode::process_whole(stream, soundbank_definition, soundbank_dest);
                    write_json(fmt::format("{}/data.json", soundbank_dest), soundbank_definition);
                    break;
                }
                case DataType::PopAnim:
                {
                    assert_conditional(compare_string(extension, ".pam"_sv), String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_pam_file")), resource_information.path), "decode_popcap_file");
                    auto stream = DataStreamView{resource_data};
                    assert_conditional(try_fix_popanim(stream), String::format(fmt::format("{}", Language::get("pvz2.scg.pam_is_corrupted")), resource_information.path), "decode_popcap_file");
                    auto animation = Sen::Kernel::Support::PopCap::Animation::SexyAnimation{};
                    Sen::Kernel::Support::PopCap::Animation::Decode::process_whole(stream, animation);
                    exchange_path(resource_information.path, ""_sv, ".json"_sv);
                    write_json(fmt::format("{}/{}", resource_destination, resource_information.path), animation);
                    break;
                }
                default:
                    write_bytes(fmt::format("{}/{}", resource_destination, resource_information.path), resource_data);
                    break;
                }
            }
            else
            {
                write_bytes(fmt::format("{}/{}", resource_destination, resource_information.path), resource_data);
            }
            return;
        }

        inline static auto exchange_general_additional(
            std::map<std::string, PacketInformation> &packet_information,
            InformationStructure &definition,
            std::string const &resource_destination,
            Setting const &setting) -> void
        {
            for (auto &[subgroup_id, packet_value] : packet_information)
            {
                assert_conditional(!packet_value.is_image, fmt::format("{}", Language::get("pvz2.scg.general_cannot_cotain_image")), "exchange_general_additional");
                auto &packet_info = definition.subgroup[subgroup_id];
                packet_info.category.locale = packet_value.subgroup_content_information.general.locale;
                auto before_compression = PacketCompression{};
                Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(packet_info.category.compression, before_compression);
                exchange_compression(packet_value.packet_structure.compression, before_compression);
                Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_to_data(packet_info.category.compression, before_compression);
                for (auto &[data_id, data_value] : packet_value.subgroup_content_information.general.data)
                {
                    auto resource_information = ResourceInformation{
                        .type = data_value.type,
                        .path = data_value.path};
                    if (is_program_path(data_value.path))
                    {
                        packet_info.resource[data_id] = resource_information;
                        continue;
                    }
                    else
                    {
                        for (auto &[resource_path, resource_data] : packet_value.resource_data_section_view_stored)
                        {
                            if (compare_string(String::to_posix_style(resource_path), data_value.path))
                            {
                                switch (setting.decode_method)
                                {
                                case DecodeMethod::Advanced:
                                {
                                    decode_popcap_file<true>(resource_data, resource_information, resource_destination);
                                    break;
                                }
                                default:
                                {
                                    decode_popcap_file<false>(resource_data, resource_information, resource_destination);
                                    break;
                                }
                                }
                                packet_info.resource[data_id] = resource_information;
                                packet_value.resource_data_section_view_stored.erase(resource_path);
                                break;
                            }
                        }
                    }
                }
            }
            return;
        }

        inline static auto exchange_packet(
            PacketOriginalInformation const &packet_original_information,
            InformationStructure &definition,
            std::string_view destination,
            Setting const &setting) -> void
        {
            if (setting.decode_method == DecodeMethod::Debug)
            {
                exchange_unpack_debug(packet_original_information, destination);
                return;
            }
            else
            {
                if (setting.decode_method == DecodeMethod::Simple)
                {
                    definition.expand_method = "simple";
                }
                else if (setting.decode_method == DecodeMethod::Advanced)
                {
                    definition.expand_method = "advanced";
                }
            }
            definition.version = packet_original_information.version;
            definition.texture_format_category = packet_original_information.texture_format_category;
            definition.composite = packet_original_information.composite;
            auto packet_information = std::map<std::string, PacketInformation>{};
            exchange_nessessory_data(packet_original_information.subgroup, packet_information);
            if (definition.composite)
            {
                auto general_group_information = std::map<std::string, PacketInformation>{};
                exchange_texture_additional(general_group_information, packet_information, definition, fmt::format("{}/resource", destination), setting);
                exchange_general_additional(general_group_information, definition, fmt::format("{}/resource", destination), setting);
            }
            else
            {
                exchange_general_additional(packet_information, definition, fmt::format("{}/resource", destination), setting);
            }
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            InformationStructure &definition,
            std::string_view destination,
            Setting const &setting) -> void
        {
            auto packet_original_information = PacketOriginalInformation{};
            Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_stream_resource_group(packet_original_information, stream);
            exchange_packet(packet_original_information, definition, destination, setting);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination,
            Setting const &setting) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = InformationStructure{};
            process_whole(stream, definition, destination, setting);
            if (setting.decode_method != DecodeMethod::Debug)
            {
                write_json(fmt::format("{}/data.json", destination), definition);
            }
            return;
        }
    };
}