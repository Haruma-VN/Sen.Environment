#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup
{

    struct Encode : Common
    {

    private:
        inline static auto exchange_pack_debug(
            PacketOriginalInformation &packet_original_information,
            std::string_view source) -> void
        {
            DebuggerInformation definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            packet_original_information.version = definition.version;
            packet_original_information.composite = definition.composite;
            packet_original_information.texture_format_category = definition.texture_format_category;
            for (auto &[subgroup_id, subgroup_value] : definition.subgroup)
            {
                auto &packet = packet_original_information.subgroup[subgroup_id];
                packet.is_image = subgroup_value.is_image;
                packet.packet_data = FileSystem::read_binary<uint8_t>(fmt::format("{}/{}", source, subgroup_value.packet_path));
                packet.info = *FileSystem::read_json(fmt::format("{}/{}", source, subgroup_value.info_path));
            }
            return;
        }

        inline static auto exchange_nessessory_data(
            std::map<std::string, PacketInformation> &packet_information,
            std::map<std::string, SubgroupRawContent> &subgroup_content_map) -> void
        {
            for (auto &[subgroup_id, subgroup_value] : packet_information)
            {
                auto &subgroup_info = subgroup_content_map[subgroup_id];
                subgroup_info.is_image = subgroup_value.is_image;
                auto packet_stream = DataStreamView{};
                Sen::Kernel::Support::PopCap::ResourceStreamGroup::Pack::process_whole(packet_stream, subgroup_value.packet_structure, subgroup_value.resource_data_section_view_stored);
                subgroup_info.info = subgroup_value.subgroup_content_information;
                subgroup_info.packet_data = std::move(packet_stream.toBytes());
            }
            return;
        }

        // ----------------------------------------------------------------------------------------------------------------------

        inline static auto exchange_image_merge(
            std::map<string, ImageSpriteInfo> &texture_sprite_view_stored,
            PacketInformation &packet_info,
            TextureFormatCategory const &texture_format_category,
            int const &format,
            int const &before_resolution,
            int const &after_resolution,
            std::string const &subgroup_id) -> void
        {
            auto check = [](
                             int const &data,
                             int &value) -> void
            {
                if (data > value)
                {
                    value = data;
                }
                return;
            };
            auto log = [](
                           int &value) -> void
            {
                value = std::pow(2, std::ceil(std::log(value) * 1.4426950408889634)); // log2e
                return;
            };
            auto rectangle_list = std::vector<MaxRectsAlgorithm::Rectangle>{};
            auto image_width = MaxRectsAlgorithm::EDGE_MAX_VALUE;
            auto image_height = MaxRectsAlgorithm::EDGE_MAX_VALUE;
            packet_info.subgroup_content_information.texture.resolution = after_resolution;
            packet_info.is_image = true;
            auto resolution_resize_ratio = static_cast<float>(after_resolution) / static_cast<float>(before_resolution);
            if (resolution_resize_ratio != static_cast<float>(1))
            {
                for (auto &[data_id, image_info] : texture_sprite_view_stored)
                {
                    auto resized_image = Definition::Image<int>::resize(image_info.data, resolution_resize_ratio);
                    image_info.data.width = resized_image.width;
                    image_info.data.height = resized_image.height;
                    image_info.data.set_data(resized_image.data());
                }
            }
            for (auto &[data_id, image_info] : texture_sprite_view_stored)
            {
                check(image_info.data.width, image_width);
                check(image_info.data.height, image_height);
                rectangle_list.emplace_back(MaxRectsAlgorithm::Rectangle(image_info.data.width, image_info.data.height, data_id));
            }
            log(image_width);
            log(image_height);
            auto option = MaxRectsAlgorithm::Option{
                .border = 1,
                .padding = 3}; // safe padding.
            auto max_rects_packer = MaxRectsAlgorithm::MaxRectsPacker(image_width, image_height, option);
            max_rects_packer.addArray(rectangle_list);
            for (auto i : Range(max_rects_packer.bins.size()))
            {
                auto &max_rects_bin = max_rects_packer.bins[i];
                auto image_id_index = exchange_image_id_index(subgroup_id, i);
                auto packet_name = toupper_back(fmt::format("{}_{}", k_default_texture_packet_id, image_id_index));
                auto &image_packet = packet_info.subgroup_content_information.texture.packet[packet_name];
                image_packet.path = fmt::format("atlases/{}", image_id_index);
                image_packet.dimension.width = max_rects_bin.width;
                image_packet.dimension.height = max_rects_bin.height;
                image_packet.additional.format = format;
                image_packet.additional.index = i;
                auto image_list = std::vector<Definition::Image<int>>{};
                image_list.resize(max_rects_bin.rects.size());
                for (auto i : Range(max_rects_bin.rects.size()))
                {
                    auto &rect = max_rects_bin.rects[i];
                    auto &image_info = texture_sprite_view_stored.at(rect.source);
                    image_packet.data[rect.source] = DataCompressedInfo{
                        .type = DataType::Image,
                        .path = restore_image_path(image_info.path, after_resolution),
                        .texture_info = TextureDefault{
                            /*
                            .ax = exchange_increase_pos(rect.x),
                            .ay = exchange_increase_pos(rect.y),
                            .aw = exchange_decrease_size(rect.width),
                            .ah = exchange_decrease_size(rect.height),
                            */
                            .ax = rect.x,
                            .ay = rect.y,
                            .aw = rect.width,
                            .ah = rect.height,
                            .x = image_info.additional.x,
                            .y = image_info.additional.y,
                            .cols = image_info.additional.cols,
                            .rows = image_info.additional.rows}};
                    image_list[i].x = rect.x;
                    image_list[i].y = rect.y;
                    image_list[i].width = image_info.data.width;
                    image_list[i].height = image_info.data.height;
                    image_list[i].set_data(image_info.data.data());
                }
                auto atlas_image = Definition::Image<int>::transparent(Definition::Dimension{max_rects_bin.width, max_rects_bin.height});
                Definition::Image<int>::join_extend(atlas_image, image_list);
                auto texture_path = fmt::format("atlases/{}.ptx", image_id_index);
                auto &texture_data = packet_info.resource_data_section_view_stored[toupper_back(String::to_windows_style(texture_path))];
                auto image_format = exchange_image_format(texture_format_category, format);
                texture_data = Sen::Kernel::Support::Texture::InvokeMethod::encode_whole(atlas_image, image_format);
                exchange_image_additional(image_format, texture_data.size(), image_packet);
                auto resource_info = Sen::Kernel::Support::PopCap::ResourceStreamGroup::Resource{
                    .path = toupper_back(texture_path),
                    .use_texture_additional_instead = true,
                };
                resource_info.texture_additional.value.dimension = Sen::Kernel::Support::PopCap::ResourceStreamGroup::TextureDimension{
                    .width = max_rects_bin.width,
                    .height = max_rects_bin.height};
                resource_info.texture_additional.value.index = static_cast<int>(i);
                packet_info.packet_structure.resource.emplace_back(resource_info);
            }
            return;
        }

        inline static auto exchange_image_sprite(
            std::map<string, ImageSpriteInfo> &texture_sprite_view_stored,
            Sen::Kernel::Support::PopCap::Animation::Convert::ExtraInfo &extra,
            std::string const &resource_source,
            std::string const &path) -> void
        {
            /*
            auto k_media_scale_ratio = static_cast<double>(Sen::Kernel::Support::PopCap::Animation::Convert::Common::k_standard_resolution) / static_cast<double>(extra.resolution);
            auto exchange_size = [&](
                                     int const &data) -> int
            {
                return static_cast<int>(data * k_media_scale_ratio);
            };
            */
            for (auto &[image_name, image_value] : extra.image)
            {
                auto image_path = (!image_value.path.empty() ? image_value.path : image_name);
                /*
                while (image_path.starts_with("../"_sv)) {
                        image_destination = Path::getParents(image_destination);
                        image_path = image_path.substr("../"_sv.size(), image_path.size() - "../"_sv.size());
                }
                */
                auto image = ImageIO::read_png(fmt::format("{}/{}/library/media/{}.png", resource_source, path, image_path));
                if (texture_sprite_view_stored.contains(toupper_back(image_value.id)))
                {
                    auto &before_image = texture_sprite_view_stored[toupper_back(image_value.id)];
                    if (std::equal(before_image.data.data().begin(), before_image.data.data().end(), image.data().begin()))
                    {
                        continue;
                    }
                    else
                    {
                        assert_conditional(false, String::format(fmt::format("{}", Language::get("pvz2.scg.same_id_but_image_mismatch")), toupper_back(image_value.id)), "exchange_image_sprite");
                    }
                }
                auto &image_info = texture_sprite_view_stored[toupper_back(image_value.id)];
                image_info.path = fmt::format("{}/{}", path, image_path);
                image_info.additional = image_value.additional;
                image_info.data.width = image.width;
                image_info.data.height = image.height;
                image_info.data.set_data(image.data());
            }
            return;
        }

        inline static auto get_animation_resolution(
            TextureFormatCategory const &texture_format_category,
            int const &resolution) -> int
        {
            switch (texture_format_category)
            {
            case TextureFormatCategory::Android:
            {
                return 768; // default for android.
            }
            case TextureFormatCategory::Chinese:
            {
                assert_conditional(resolution == 1200, fmt::format("{}", Language::get("popcap.rsb.project.chinese_version_only_have_1200_resolution")), "get_animation_resolution");
                return resolution;
            }
            default:
            {
                if (resolution == static_cast<int>(k_none_size)) {
                    return 768; // most image for android.
                }
                return resolution;
            }
            }
        }

        inline static auto exchange_animation(
            std::vector<uint8_t> &resource_data,
            Sen::Kernel::Support::PopCap::Animation::Convert::ExtraInfo const &extra,
            DataCompressedInfo &data_info,
            int const &resolution,
            std::string const &animation_source,
            bool const &animation_split_label) -> void
        {
            auto extension = Kernel::Path::getExtension(data_info.path);
            assert_conditional(data_info.type == DataType::PopAnim, String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_popanim_type")), data_info.path), "exchange_animation");
            auto stream = DataStreamView{resource_data};
            auto animation = Sen::Kernel::Support::PopCap::Animation::SexyAnimation{};
            if (animation_split_label)
            {
                Sen::Kernel::Support::PopCap::Animation::Convert::FromFlash<true>::process_whole(animation, extra, animation_source);
            }
            else
            {
                Sen::Kernel::Support::PopCap::Animation::Convert::FromFlash<false>::process_whole(animation, extra, animation_source);
            }
            Sen::Kernel::Support::PopCap::Animation::Encode::process_whole(stream, animation);
            data_info.path = restore_animation_path(data_info.path, resolution);
            resource_data = std::move(stream.toBytes());
            return;
        }

        inline static auto exchange_texture_advanced(
            std::map<std::string, PacketInformation> &packet_information,
            InformationStructure const &definition,
            std::map<std::string, SubgroupDataInformation> &general_subgroup,
            std::vector<int> &resolution_list,
            std::string const &resource_source,
            bool const &animation_split_label) -> void
        {
            auto highest_resolution = resolution_list.front();
            for (auto &[subgroup_id, packet_value] : definition.subgroup)
            {
                if (!packet_value.category.common_type) {
                    general_subgroup[subgroup_id] = packet_value;
                    continue;
                }
                auto texture_sprite_view_stored = std::map<string, ImageSpriteInfo>{};
                for (auto &[resource_id, resource_info] : packet_value.resource)
                {
                    switch (resource_info.type)
                    {
                    case DataType::Image:
                    {
                        if (is_program_path(resource_info.path))
                        {
                            packet_information[fmt::format("{}_Common", subgroup_id)].subgroup_content_information.general.data[resource_id] = DataCompressedInfo{
                                .type = resource_info.type,
                                .path = resource_info.path};
                            break;
                        }
                        auto image = ImageIO::read_png(fmt::format("{}/{}", resource_source, resource_info.path));
                        auto &image_info = texture_sprite_view_stored[toupper_back(resource_id)];
                        image_info.path = resource_info.path;
                        exchange_path(image_info.path, ".png"_sv, ""_sv);
                        image_info.additional = resource_info.additional;
                        image_info.data.width = image.width;
                        image_info.data.height = image.height;
                        image_info.data.set_data(image.data());
                        break;
                    }
                    case DataType::PopAnim:
                    {
                        auto &packet_info = packet_information[fmt::format("{}_Common", subgroup_id)];
                        packet_info.packet_structure.version = definition.version;
                        packet_info.subgroup_content_information.general.locale = packet_value.category.locale;
                        Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(packet_value.category.compression, packet_info.packet_structure.compression);
                        auto data_information = DataCompressedInfo{
                            .type = resource_info.type,
                            .path = resource_info.path};
                        if (is_program_path(resource_info.path))
                        {
                            packet_info.subgroup_content_information.general.data[toupper_back(resource_id)] = data_information;
                            break;
                        }
                        else
                        {
                            auto animation_source = fmt::format("{}/{}", resource_source, resource_info.path);
                            assert_conditional(compare_string(Kernel::Path::getExtension(resource_info.path), ""_sv) && std::filesystem::is_directory(animation_source), String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_folder_and_no_file_extension")), resource_info.path), "exchange_texture_advanced");
                            Sen::Kernel::Support::PopCap::Animation::Convert::ExtraInfo extra = *FileSystem::read_json(fmt::format("{}/data.json", animation_source));
                            Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::compare_conditional(highest_resolution, extra.resolution, Path::getFileName(resource_info.path), "pvz2.scg.resolution_mismatch");
                            exchange_image_sprite(texture_sprite_view_stored, extra, resource_source, resource_info.path);
                            auto resource_data = std::vector<uint8_t>{};
                            exchange_animation(resource_data, extra, data_information, get_animation_resolution(definition.texture_format_category, highest_resolution), animation_source, animation_split_label);
                            packet_info.resource_data_section_view_stored[toupper_back(String::to_windows_style(data_information.path))] = std::move(resource_data);
                            packet_info.subgroup_content_information.general.data[toupper_back(resource_id)] = data_information;
                            packet_info.packet_structure.resource.emplace_back(Sen::Kernel::Support::PopCap::ResourceStreamGroup::Resource{
                                .path = toupper_back(data_information.path)});
                        }
                        break;
                    }
                    default:
                    {
                        auto &packet_info = packet_information[fmt::format("{}_Common", subgroup_id)];
                        packet_info.packet_structure.version = definition.version;
                        packet_info.subgroup_content_information.general.locale = packet_value.category.locale;
                        Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(packet_value.category.compression, packet_info.packet_structure.compression);
                        auto data_information = DataCompressedInfo{
                            .type = resource_info.type,
                            .path = resource_info.path};
                        if (is_program_path(resource_info.path))
                        {
                            packet_info.subgroup_content_information.general.data[toupper_back(resource_id)] = data_information;
                            continue;
                        }
                        else
                        {
                            auto resource_data = std::vector<uint8_t>{};
                            encode_popcap_file<false>(resource_data, data_information, resource_source);
                            packet_info.resource_data_section_view_stored[toupper_back(String::to_windows_style(data_information.path))] = std::move(resource_data);
                            packet_info.subgroup_content_information.general.data[toupper_back(resource_id)] = data_information;
                            packet_info.packet_structure.resource.emplace_back(Sen::Kernel::Support::PopCap::ResourceStreamGroup::Resource{
                                .path = toupper_back(data_information.path)});
                        }
                        break;
                    }
                    }
                }
                if (texture_sprite_view_stored.size() != k_none_size)
                {
                    auto before_resolution = highest_resolution;
                    for (auto resize_resolution : resolution_list)
                    {
                        if (before_resolution != highest_resolution)
                        {
                            assert_conditional(before_resolution > resize_resolution, String::format(fmt::format("{}", Language::get("popcap.rsb.project.resize_resolution_must_be_lower")), std::to_string(before_resolution), std::to_string(resize_resolution)), "exchange_texture_advanced");
                        }
                        auto subgroup_id_with_resize_resolution = fmt::format("{}_{}", subgroup_id, resize_resolution);
                        auto &packet_info_resize = packet_information[subgroup_id_with_resize_resolution];
                        packet_info_resize.packet_structure.version = definition.version;
                        Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(packet_value.category.compression, packet_info_resize.packet_structure.compression);
                        packet_info_resize.subgroup_content_information.general.locale = packet_value.category.locale;
                        exchange_image_merge(texture_sprite_view_stored, packet_info_resize, definition.texture_format_category, definition.category.format, before_resolution, resize_resolution, subgroup_id_with_resize_resolution);
                        before_resolution = resize_resolution;
                    }
                }
            }
            return;
        }

        inline static auto exchange_texture_simple(
            std::map<std::string, PacketInformation> &packet_information,
            InformationStructure const &definition,
            std::map<std::string, SubgroupDataInformation> &general_subgroup,
            std::vector<int> &resolution_list,
            std::string const &resource_source) -> void
        {
            auto highest_resolution = resolution_list.front();
            resolution_list.erase(resolution_list.begin());
            for (auto &[subgroup_id, packet_value] : definition.subgroup)
            {
                if (!packet_value.category.common_type) {
                    general_subgroup[subgroup_id] = packet_value;
                }
                for (auto &[resource_id, resource_info] : packet_value.resource)
                {
                    switch (resource_info.type)
                    {
                    case DataType::ImageData:
                    {
                        assert_conditional(Path::getExtension(resource_info.path) == ".json", String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_json_file")), resource_info.path), "exchange_texture_simple");
                        auto subgroup_id_with_resolution = fmt::format("{}_{}", subgroup_id, highest_resolution);
                        auto &packet_info = packet_information[subgroup_id_with_resolution];
                        packet_info.packet_structure.version = definition.version;
                        packet_info.subgroup_content_information.general.locale = packet_value.category.locale;
                        Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(packet_value.category.compression, packet_info.packet_structure.compression);
                        packet_info.is_image = true;
                        auto image_information = SubgroupCompressedInfo{};
                        exchange_subgroup_compression_info(*FileSystem::read_json(fmt::format("{}/{}", resource_source, resource_info.path)), image_information);
                        auto image_index = k_begin_index;
                        auto texture_sprite_view_stored = std::map<string, ImageSpriteInfo>{};
                        for (auto &[image_id, image_info] : image_information.texture.packet)
                        {
                            auto image = ImageIO::read_png(fmt::format("{}/{}.png", resource_source, image_info.path));
                            auto image_id_index = exchange_image_id_index(subgroup_id_with_resolution, image_index);
                            Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::compare_conditional(image.width, image_info.dimension.width, image_id, "pvz2.scg.mismatch_image_width");
                            Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::compare_conditional(image.height, image_info.dimension.height, image_id, "pvz2.scg.mismatch_image_height");
                            auto texture_path = fmt::format("atlases/{}.ptx", image_id_index);
                            auto &texture_data = packet_info.resource_data_section_view_stored[toupper_back(String::to_windows_style(texture_path))];
                            auto image_format = exchange_image_format(definition.texture_format_category, definition.category.format);
                            texture_data = std::move(Sen::Kernel::Support::Texture::InvokeMethod::encode_whole(image, image_format));
                            auto &image_texture = packet_info.subgroup_content_information.texture;
                            image_texture.resolution = highest_resolution;
                            auto &image_packet = image_texture.packet[toupper_back(fmt::format("{}_{}", k_default_texture_packet_id, image_id_index))];
                            image_packet.path = fmt::format("atlases/{}", image_id_index);
                            image_packet.dimension = image_info.dimension;
                            image_packet.additional.format = definition.category.format;
                            image_packet.additional.index = image_index;
                            exchange_image_additional(image_format, texture_data.size(), image_packet);
                            for (auto &[data_id, data_value] : image_info.data)
                            {
                                image_packet.data[toupper_back(data_id)] = data_value;
                                image_packet.data[toupper_back(data_id)].path = restore_image_path(data_value.path, highest_resolution);
                            }
                            auto resource_info = Sen::Kernel::Support::PopCap::ResourceStreamGroup::Resource{
                                .path = toupper_back(texture_path),
                                .use_texture_additional_instead = true,
                            };
                            resource_info.texture_additional.value.dimension = Sen::Kernel::Support::PopCap::ResourceStreamGroup::TextureDimension{
                                .width = image_packet.dimension.width,
                                .height = image_packet.dimension.height};
                            resource_info.texture_additional.value.index = static_cast<int>(image_index);
                            packet_info.packet_structure.resource.emplace_back(resource_info);
                            if (resolution_list.size() != k_none_size)
                            {
                                exchange_image_split(texture_sprite_view_stored, image, subgroup_id, image_packet);
                            }
                            ++image_index;
                        }
                        auto before_resolution = highest_resolution;
                        for (auto resize_resolution : resolution_list)
                        {
                            assert_conditional(before_resolution > resize_resolution, String::format(fmt::format("{}", Language::get("popcap.rsb.project.resize_resolution_must_be_lower")), std::to_string(before_resolution), std::to_string(resize_resolution)), "exchange_texture_simple");
                            auto subgroup_id_with_resize_resolution = fmt::format("{}_{}", subgroup_id, resize_resolution);
                            auto &packet_info_resize = packet_information[subgroup_id_with_resize_resolution];
                            packet_info_resize.packet_structure.version = definition.version;
                            packet_info_resize.packet_structure.compression = packet_info.packet_structure.compression;
                            exchange_image_merge(texture_sprite_view_stored, packet_info_resize, definition.texture_format_category, definition.category.format, before_resolution, resize_resolution, subgroup_id_with_resize_resolution);
                            before_resolution = resize_resolution;
                        }
                        break;
                    }
                    default:
                    {
                        auto &packet_info = packet_information[fmt::format("{}_Common", subgroup_id)];
                        packet_info.packet_structure.version = definition.version;
                        packet_info.subgroup_content_information.general.locale = packet_value.category.locale;
                        Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(packet_value.category.compression, packet_info.packet_structure.compression);
                        auto data_information = DataCompressedInfo{
                            .type = resource_info.type,
                            .path = resource_info.path};
                        if (is_program_path(resource_info.path))
                        {
                            packet_info.subgroup_content_information.general.data[toupper_back(resource_id)] = data_information;
                            continue;
                        }
                        else
                        {
                            auto resource_data = std::vector<uint8_t>{};
                            encode_popcap_file<false>(resource_data, data_information, resource_source);
                            if (data_information.type == DataType::PopAnim) {
                                data_information.path = restore_animation_path(data_information.path, get_animation_resolution(definition.texture_format_category, highest_resolution));
                            } 
                            packet_info.resource_data_section_view_stored[toupper_back(String::to_windows_style(data_information.path))] = std::move(resource_data);
                            packet_info.subgroup_content_information.general.data[toupper_back(resource_id)] = data_information;
                            packet_info.packet_structure.resource.emplace_back(Sen::Kernel::Support::PopCap::ResourceStreamGroup::Resource{
                                .path = toupper_back(data_information.path)});
                        }
                        break;
                    }
                    }
                }
            }
            return;
        }

        inline static auto exchange_texture_additional(
            std::map<std::string, PacketInformation> &packet_information,
            InformationStructure const &definition,
            std::map<std::string, SubgroupDataInformation> &general_subgroup,
            std::string const &resource_source,
            Setting const &setting) -> void
        {
            auto resolution_list = definition.category.resolution;
            assert_conditional(resolution_list.size() != k_none_size, fmt::format("{}", Language::get("popcap.rsb.project.category_must_contains_one_resolution")), "exchange_texture_additional");
            std::sort(resolution_list.begin(), resolution_list.end(), std::greater<int>());
            for (auto &resoulution : resolution_list)
            {
                auto is_vaild_resoulution = std::find(k_vaild_resolution_list.begin(), k_vaild_resolution_list.end(), resoulution) != k_vaild_resolution_list.end();
                assert_conditional(is_vaild_resoulution, String::format(fmt::format("{}", Language::get("popcap.rsb.project.invalid_resolution")), std::to_string(resoulution), std::string{"[1536, 768, 384, 640, 1200]"}), "exchange_texture_additional");
            }
            switch (setting.decode_method)
            {
            case DecodeMethod::Advanced:
            {
                exchange_texture_advanced(packet_information, definition, general_subgroup, resolution_list, resource_source, setting.animation_split_label);
                break;
            }
            default:
            {
                exchange_texture_simple(packet_information, definition, general_subgroup, resolution_list, resource_source);
                break;
            }
            }
            return;
        }

        // -----------------------------------------------------------------------------------------------------------------------

        template <auto encode_method_advanced>
        inline static auto encode_popcap_file(
            std::vector<uint8_t> &resource_data,
            DataCompressedInfo &data_information,
            std::string const &resource_source) -> void
        {
            static_assert(encode_method_advanced == true || encode_method_advanced == false, "encode_method_advanced must be true or false");
            auto extension = Kernel::Path::getExtension(data_information.path);
            if constexpr (encode_method_advanced)
            {
                switch (data_information.type)
                {
                case DataType::Data:
                {
                    assert_conditional(compare_string(extension, ".json"_sv), String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_json_file")), data_information.path), "encode_popcap_file");
                    auto stream = DataStreamView{};
                    Sen::Kernel::Support::PopCap::ReflectionObjectNotation::Encode::process_whole(stream, FileSystem::read_file(fmt::format("{}/{}", resource_source, data_information.path)));
                    data_information.type = DataType::File;
                    exchange_path(data_information.path, ".json"_sv, ".rton"_sv);
                    resource_data = std::move(stream.toBytes());
                    break;
                }
                case DataType::SoundBank:
                case DataType::DecodedSoundBank:
                {
                    auto soundbank_source = fmt::format("{}/{}", resource_source, data_information.path);
                    assert_conditional(compare_string(extension, ""_sv) && std::filesystem::is_directory(soundbank_source), String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_folder_and_no_file_extension")), data_information.path), "encode_popcap_file");
                    auto stream = DataStreamView{};
                    auto soundbank_definition = *FileSystem::read_json(fmt::format("{}/data.json", soundbank_source));
                    Sen::Kernel::Support::WWise::SoundBank::Encode::process_whole(stream, soundbank_definition, soundbank_source);
                    exchange_path(data_information.path, ""_sv, ".bnk"_sv);
                    resource_data = std::move(stream.toBytes());
                    break;
                }
                case DataType::PopAnim:
                {
                    assert_conditional(compare_string(extension, ".json"_sv), String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_json_file")), data_information.path), "encode_popcap_file");
                    auto animation = *FileSystem::read_json(fmt::format("{}/{}", resource_source, data_information.path));
                    auto stream = DataStreamView{};
                    Sen::Kernel::Support::PopCap::Animation::Encode::process_whole(stream, animation);
                    exchange_path(data_information.path, ".json"_sv, ""_sv);
                    if (!compare_string(Kernel::Path::getExtension(data_information.path), ".pam"_sv))
                    {
                        exchange_path(data_information.path, Kernel::Path::getExtension(data_information.path), ".pam"_sv);
                    }
                    resource_data = std::move(stream.toBytes());
                    break;
                }
                default:
                    resource_data = std::move(FileSystem::read_binary<uint8_t>(fmt::format("{}/{}", resource_source, data_information.path)));
                    break;
                }
            }
            else
            {
                resource_data = std::move(FileSystem::read_binary<uint8_t>(fmt::format("{}/{}", resource_source, data_information.path)));
            }
            return;
        }

        inline static auto exchange_general_additional(
            std::map<std::string, PacketInformation> &packet_information,
            InformationStructure const &definition,
            std::map<std::string, SubgroupDataInformation> const &subgroup,
            std::string const &resource_source,
            Setting const &setting) -> void
        {
            for (auto &[subgroup_id, packet_value] : subgroup)
            {
                auto &packet_info = packet_information[subgroup_id];
                packet_info.packet_structure.version = definition.version;
                packet_info.subgroup_content_information.general.locale = packet_value.category.locale;
                Sen::Kernel::Support::PopCap::ResourceStreamGroup::Common::packet_compression_from_data(packet_value.category.compression, packet_info.packet_structure.compression);
                for (auto &[resource_id, resource_info] : packet_value.resource)
                {
                    auto data_information = DataCompressedInfo{
                        .type = resource_info.type,
                        .path = resource_info.path};
                    if (is_program_path(resource_info.path))
                    {
                        packet_info.subgroup_content_information.general.data[toupper_back(resource_id)] = data_information;
                        continue;
                    }
                    else
                    {
                        auto resource_data = std::vector<uint8_t>{};
                        switch (setting.decode_method)
                        {
                        case DecodeMethod::Advanced:
                        {
                            encode_popcap_file<true>(resource_data, data_information, resource_source);
                            break;
                        }
                        default:
                        {
                            encode_popcap_file<false>(resource_data, data_information, resource_source);
                            break;
                        }
                        }
                        packet_info.resource_data_section_view_stored[toupper_back(String::to_windows_style(data_information.path))] = std::move(resource_data);
                        packet_info.subgroup_content_information.general.data[toupper_back(resource_id)] = data_information;
                        packet_info.packet_structure.resource.emplace_back(Sen::Kernel::Support::PopCap::ResourceStreamGroup::Resource{
                            .path = toupper_back(data_information.path)});
                    }
                }
            }
            return;
        }

        inline static auto exchange_packet(
            PacketOriginalInformation &packet_original_information,
            InformationStructure const &definition,
            std::string_view source,
            Setting const &setting) -> void
        {
            if (setting.decode_method == DecodeMethod::Debug)
            {
                exchange_pack_debug(packet_original_information, source);
                return;
            }
            packet_original_information.version = definition.version;
            packet_original_information.texture_format_category = definition.texture_format_category;
            packet_original_information.composite = definition.composite;
            auto packet_information = std::map<std::string, PacketInformation>{};
            if (definition.composite) {
                auto general_subgroup = std::map<std::string, SubgroupDataInformation>{};
                exchange_texture_additional(packet_information, definition, general_subgroup, fmt::format("{}/resource", source), setting);
                exchange_general_additional(packet_information, definition, general_subgroup, fmt::format("{}/resource", source), setting);
            }
            else {
                exchange_general_additional(packet_information, definition, definition.subgroup, fmt::format("{}/resource", source), setting);
            }
            exchange_nessessory_data(packet_information, packet_original_information.subgroup);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            InformationStructure const &definition,
            std::string_view source,
            Setting const &setting) -> void
        {
            auto packet_original_information = PacketOriginalInformation{};
            exchange_packet(packet_original_information, definition, source, setting);
            Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup::Common::exchange_stream_resource_group(stream, packet_original_information);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination,
            Setting const &setting) -> void
        {
            auto stream = DataStreamView{};
            auto definition = InformationStructure{};
            if (setting.decode_method != DecodeMethod::Debug)
            {
                definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            }
            process_whole(stream, definition, source, setting);
            stream.out_file(destination);
            return;
        }
    };
}