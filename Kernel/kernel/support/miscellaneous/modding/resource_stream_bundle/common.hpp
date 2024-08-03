#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/popcap/resource_stream_bundle/common.hpp"
#include "kernel/support/miscellaneous/modding/resource_stream_bundle/definition.hpp"
#include "kernel/support/popcap/resource_stream_group/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/definition.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::ResourceStreamBundle
{
    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using namespace Definition;

    using ImageFormat = Sen::Kernel::Support::Texture::Format;

    using BundleStructure = Sen::Kernel::Support::PopCap::ResourceStreamBundle::BundleStructure;

    using ManifestStructure = Sen::Kernel::Support::PopCap::ResourceStreamBundle::ManifestStructure;

    using DataSectionViewStored = std::map<std::string, std::vector<uint8_t>, decltype(&case_insensitive_compare)>;

    struct Common
    {
        inline static constexpr auto k_manifest_string = "__MANIFESTGROUP__";

        inline static constexpr auto k_resources_string = "resources";

        inline static constexpr auto k_package_string = "Packages"_sv;

        inline static constexpr auto k_rton_extension_string = ".rton"_sv;

        inline static constexpr auto k_newton_extension_string = ".newton"_sv;

        inline static constexpr auto DefaultCoordinateOffset = 0;

        inline static constexpr auto DefaultLayoutOffset = 1;

        inline static constexpr auto packet_list = std::array<int, 2>{1536, 1200}; // 1200 for only 2c

        inline static constexpr auto empty_virtual_image_format = 0x1000_ui;

        inline static constexpr auto k_china_reflection_object_natation_rijndael_key_string = "65bd1b2305f46eb2806b935aab7630bb"_sv;

        inline static constexpr auto k_china_reflection_object_natation_rijndael_iv_string = "1b2305f46eb2806b935aab76"_sv;

        inline static constexpr auto double_backsplash = "\\"_sv;

        inline static constexpr auto k_highest_compression_method = 3_ui;

        inline static constexpr auto k_etc1_a_palette_scale = 100_ui;

        inline static constexpr auto k_default_alpha_size = 100_size;

        inline static constexpr auto k_program_path = "!program"_sv;

        inline static constexpr auto k_data_type_file = "File"_sv;

        inline static constexpr auto k_packet_general_type_name = "general_type"_sv;

        struct PackagesFileList {
            std::vector<string>* rton_file;
            std::vector<string>* json_file;
            PackagesFileList(
                std::vector<string>* rton_file,
                std::vector<string>* json_file
            ) : rton_file(rton_file), json_file(json_file)
            {

            }
        };

        inline static auto k_data_type_table = std::map<DataType, std::string_view>{
            {DataType::Image, "Image"_sv}, {DataType::PopAnim, "PopAnim"_sv}, {DataType::Data, "Data"_sv}, {DataType::SoundBank, "SoundBank"_sv}, {DataType::DecodedSoundBank, "DecodedSoundBank"_sv}, {DataType::PrimeFont, "PrimeFont"_sv}, {DataType::RenderEffect, "RenderEffect"_sv}, {DataType::File, "File"_sv}};

        inline static auto exchange_data_type(
            DataType const &data) -> std::string_view
        {
            if (auto it = search_element_in_map(k_data_type_table, data); it != k_data_type_table.end())
            {
                return it->second;
            }
            return k_data_type_file;
        }

        inline static auto exchange_data_type(
            std::string_view const &data) -> DataType
        {
            if (auto it = search_element_in_map(k_data_type_table, data); it != k_data_type_table.end())
            {
                return it->first;
            }
            assert_conditional(false, String::format(fmt::format("{}", Language::get("pvz2.rsb.modding.invalid_data_type")), data), "exchange_data_type");
        }

        inline static auto k_image_format_ios = std::map<ImageFormat, uint32_t>{
            {ImageFormat::ARGB_8888, 0_ui}, {ImageFormat::RGBA_4444, 1_ui}, {ImageFormat::RGB_565, 2_ui}, {ImageFormat::RGBA_5551, 3_ui}, {ImageFormat::RGBA_4444_TILED, 21_ui}, {ImageFormat::RGB_565_TILED, 22_ui}, {ImageFormat::RGBA_5551_TILED, 23_ui}, {ImageFormat::RGB_PVRTC_4BPP, 30_ui}, {ImageFormat::RGB_PVRTC_4BPP_A_8, 148_ui}};

        inline static auto k_image_format_android = std::map<ImageFormat, uint32_t>{
            {ImageFormat::RGBA_8888, 0_ui}, {ImageFormat::RGBA_4444, 1_ui}, {ImageFormat::RGB_565, 2_ui}, {ImageFormat::RGBA_5551, 3_ui}, {ImageFormat::RGBA_4444_TILED, 21_ui}, {ImageFormat::RGB_565_TILED, 22_ui}, {ImageFormat::RGBA_5551_TILED, 23_ui}, {ImageFormat::RGB_ETC1_A_8, 147_ui}};

        inline static auto k_image_format_chinese = std::map<ImageFormat, uint32_t>{
            {ImageFormat::RGBA_8888, 0_ui}, {ImageFormat::RGBA_4444, 1_ui}, {ImageFormat::RGB_565, 2_ui}, {ImageFormat::RGBA_5551, 3_ui}, {ImageFormat::RGBA_4444_TILED, 21_ui}, {ImageFormat::RGB_565_TILED, 22_ui}, {ImageFormat::RGBA_5551_TILED, 23_ui}, {ImageFormat::RGB_ETC1_A_PALETTE, 147_ui}};

        template <auto texture_format_category>
        inline static auto exchange_image_format(
            uint32_t const &data) -> ImageFormat
        {
            static_assert(texture_format_category == TextureFormatCategory::IOS || texture_format_category == TextureFormatCategory::Android || texture_format_category == TextureFormatCategory::Chinese, "invaild_texture_format_category");
            if constexpr (texture_format_category == TextureFormatCategory::IOS)
            {
                if (auto it = search_element_in_map(k_image_format_ios, data); it != k_image_format_ios.end())
                {
                    return it->first;
                }
            }
            if constexpr (texture_format_category == TextureFormatCategory::Android)
            {
                if (auto it = search_element_in_map(k_image_format_android, data); it != k_image_format_android.end())
                {
                    return it->first;
                }
            }
            if constexpr (texture_format_category == TextureFormatCategory::Chinese)
            {
                if (auto it = search_element_in_map(k_image_format_chinese, data); it != k_image_format_chinese.end())
                {
                    return it->first;
                }
            }
            assert_conditional(false, fmt::format("invalid_format: {}", data), "exchange_image_format");
        }

        template <auto texture_format_category>
        inline static auto exchange_image_format(
            ImageFormat const &data) -> uint32_t
        {
            static_assert(texture_format_category == TextureFormatCategory::IOS || texture_format_category == TextureFormatCategory::Android || texture_format_category == TextureFormatCategory::Chinese, "invaild_texture_format_category");
            if constexpr (texture_format_category == TextureFormatCategory::IOS)
            {
                if (auto it = search_element_in_map(k_image_format_ios, data); it != k_image_format_ios.end())
                {
                    return it->second;
                }
            }
            if constexpr (texture_format_category == TextureFormatCategory::Android)
            {
                if (auto it = search_element_in_map(k_image_format_android, data); it != k_image_format_android.end())
                {
                    return it->second;
                }
            }
            if constexpr (texture_format_category == TextureFormatCategory::Chinese)
            {
                if (auto it = search_element_in_map(k_image_format_chinese, data); it != k_image_format_chinese.end())
                {
                    return it->second;
                }
            }
            assert_conditional(false, fmt::format("invalid_format: {}", static_cast<uint8_t>(data)), "exchange_image_format");
        }

        inline static auto exchange_image_pitch(
            ImageFormat const &format,
            uint32_t const &width) -> uint32_t
        {
            switch (format)
            {
            case ImageFormat::RGB_PVRTC_4BPP:
            {
                return width / 2_ui;
            }
            case ImageFormat::ARGB_4444:
            case ImageFormat::RGBA_4444:
            case ImageFormat::RGB_565:
            case ImageFormat::RGBA_5551:
            case ImageFormat::RGBA_4444_TILED:
            case ImageFormat::RGB_565_TILED:
            case ImageFormat::RGBA_5551_TILED:
            {
                return width * 2_ui;
            }
            default:
            {
                return width * 4_ui;
            }
            }
        }

        template <auto texture_format_category>
        inline static auto exchange_texture_value(
            Sen::Kernel::Support::PopCap::ResourceStreamGroup::TextureValue &texture_value,
            ImageInfo const &image_info,
            size_t const &texture_information_section_size) -> void
        {
            static_assert(texture_format_category == TextureFormatCategory::IOS || texture_format_category == TextureFormatCategory::Android || texture_format_category == TextureFormatCategory::Chinese, "invaild_texture_format_category");
            texture_value.dimension.width = image_info.dimension.width;
            texture_value.dimension.height = image_info.dimension.height;
            texture_value.texture_resource_information_section_block_size = texture_information_section_size;
            texture_value.texture_infomation.format = exchange_image_format<texture_format_category>(image_info.format);
            texture_value.texture_infomation.pitch = exchange_image_pitch(image_info.format, image_info.dimension.width);
            if (texture_information_section_size == Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::k_texture_resource_information_section_block_size_version_1)
            {
                switch (image_info.format)
                {
                case ImageFormat::RGB_ETC1_A_8:
                case ImageFormat::RGB_ETC1_A_PALETTE:
                    texture_value.texture_infomation.alpha_size = k_default_alpha_size; // todo.
                    break;

                default:
                {
                    texture_value.texture_infomation.alpha_size = k_default_alpha_size;
                    break;
                }
                }
                if (texture_information_section_size == Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::k_texture_resource_information_section_block_size_version_2)
                {
                    if (image_info.format == ImageFormat::RGB_ETC1_A_PALETTE)
                    {
                        texture_value.texture_infomation.scale = k_etc1_a_palette_scale;
                    }
                }
            }
            return;
        }

        template <typename T>
        inline static auto find_duplicate_elements(
            std::vector<T> const &data,
            std::vector<T> &value) -> void
        {
            auto new_data = std::move(data);
            std::sort(new_data.begin(), new_data.end());
            for (auto i : Range(1_size, new_data.size(), 1_size))
            {
                if (new_data[i - 1_size] == new_data[i])
                {
                    if (std::find(value.begin(), value.end(), new_data[i]) == value.end())
                    {
                        value.emplace_back(new_data[i]);
                    }
                }
            }
            return;
        }

        inline static auto exchange_resource_info(
            ResInfo const &data,
            nlohmann::ordered_json &value
        ) -> void
        {
            auto general_type_name = get_string(k_packet_general_type_name);
            value["expand_path"] = data.expand_path == ExpandPath::String ? "string"_sv : "array"_sv;
            auto &res_group = value["groups"];
            for (auto &[group_id, group_value] : data.group) {
                res_group[group_id]["is_composite"] = group_value.composite;
                auto &res_subgroup = res_group[group_id]["subgroup"];
                for (auto &[subgroup_id, subgroup_value] : group_value.subgroup) {
                    auto is_image = subgroup_value.resolution != static_cast<int>(k_none_size);
                    if (is_image) {
                        res_subgroup[subgroup_id]["type"] = std::to_string(subgroup_value.resolution);
                        auto &packet = res_subgroup[subgroup_id]["packet"];
                        for (auto &[packet_id, packet_value] : subgroup_value.packet) {
                            packet[packet_id] = nlohmann::ordered_json{
                                {"type", exchange_data_type(packet_value.type)}, // Always Image
                                {"path", String::to_posix_style(packet_value.image_info.path)},
                                {"dimension", {
                                    {"width", packet_value.image_info.dimension.width},
                                    {"height", packet_value.image_info.dimension.height}
                                }}
                            };
                            for (auto &[data_id, data_value]: packet_value.data) {
                                auto &data = packet[packet_id]["data"][data_id] = nlohmann::ordered_json{
                                    {"type", exchange_data_type(data_value.type)}, // Always Image
                                    {"path", String::to_posix_style(data_value.path)},
                                    {"default", {
                                        {"ax", data_value.texture_info.ax},
                                        {"ay", data_value.texture_info.ay},
                                        {"aw", data_value.texture_info.aw},
                                        {"ah", data_value.texture_info.ah},
                                        {"x", data_value.texture_info.x},
                                        {"y", data_value.texture_info.y}
                                    }}
                                };
                                if (data_value.texture_info.rows != DefaultLayoutOffset) {
                                    data["default"]["rows"] = data_value.texture_info.rows;
                                }
                                if (data_value.texture_info.cols != DefaultLayoutOffset) {
                                    data["default"]["cols"] = data_value.texture_info.cols;
                                }
                            }
                        }
                    }
                    else {
                        res_subgroup[subgroup_id]["type"] = nullptr;
                        auto &packet = res_subgroup[subgroup_id]["packet"];
                        packet["type"] = exchange_data_type(subgroup_value.packet.at(general_type_name).type);
                        for (auto &[data_id, data_value] : subgroup_value.packet.at(general_type_name).data) {
                            packet["data"][data_id] = nlohmann::ordered_json{
                                    {"type", exchange_data_type(data_value.type)}, 
                                    {"path", String::to_posix_style(data_value.path)}
                            };
                        }
                    }
                }
            }
            return;
        }

        template <auto skip_unnecessary_packet>
        inline static auto exchange_res_info(
            nlohmann::ordered_json &data,
            ResInfo &value) -> void
        {
            static_assert(skip_unnecessary_packet == true || skip_unnecessary_packet == false, "skip_unnecessary_packet must be true or false");
            auto general_type_name = get_string(k_packet_general_type_name);
            value.expand_path = data["expand_path"].get<std::string>() == "string" ? ExpandPath::String : ExpandPath::Array;
            for (auto &[id, group_value] : data["groups"].items())
            {
                auto &group_info = value.group[id] = GroupInfo{
                     .composite = group_value["is_composite"].get<bool>()};
                for (auto &[subgroup_id, subgroup_value] : group_value["subgroup"].items())
                {
                    auto subgroup_info = SubgroupInfo{};
                    if (subgroup_value["type"] != nullptr)
                    {
                        subgroup_info.resolution = std::stoi(subgroup_value["type"].get<string>());
                    }
                    auto is_image = subgroup_info.resolution != static_cast<int>(k_none_size);
                    if constexpr (skip_unnecessary_packet)
                    {
                        if (is_image && std::find(packet_list.begin(), packet_list.end(), subgroup_info.resolution) == packet_list.end())
                        {
                            continue;
                        }
                    }
                    for (auto &[packet_id, packet_value] : (is_image ? subgroup_value["packet"].items() : subgroup_value["packet"]["data"].items()))
                    {
                        if (is_image)
                        {
                            auto packet_info = PacketInfo{
                                .type = exchange_data_type(packet_value["type"].get<std::string>())
                            };
                            packet_info.image_info.path = packet_value["path"].get<std::string>();
                            packet_info.image_info.dimension = ImageDimension{
                                .width = packet_value["dimension"]["width"].get<int>(),
                                .height = packet_value["dimension"]["height"].get<int>()};
                            for (auto &[data_id, data_value] : packet_value["data"].items())
                            {
                                auto data_info = DataInfo{
                                    .type = exchange_data_type(data_value["type"].get<std::string>()),
                                    .path = data_value["path"].get<std::string>(),
                                };
                                if (data_value["default"].find("x") != data_value["default"].end() && data_value["default"]["x"].get<int>() != DefaultCoordinateOffset)
                                {
                                    data_info.texture_info.x = data_value["default"]["x"].get<int>();
                                }
                                if (data_value["default"].find("y") != data_value["default"].end() && data_value["default"]["y"].get<int>() != DefaultCoordinateOffset)
                                {
                                    data_info.texture_info.y = data_value["default"]["y"].get<int>();
                                }
                                if (data_value["default"].find("rows") != data_value["default"].end() && data_value["default"]["rows"].get<int>() != DefaultLayoutOffset)
                                {
                                    data_info.texture_info.rows = data_value["default"]["rows"].get<int>();
                                }
                                else {
                                    data_info.texture_info.rows = DefaultLayoutOffset;
                                }
                                if (data_value["default"].find("cols") != data_value["default"].end() && data_value["default"]["cols"].get<int>() != DefaultLayoutOffset)
                                {
                                    data_info.texture_info.cols = data_value["default"]["cols"].get<int>();
                                }
                                else {
                                    data_info.texture_info.cols = DefaultLayoutOffset;
                                }
                                data_info.texture_info.ax = data_value["default"]["ax"].get<int>();
                                data_info.texture_info.ay = data_value["default"]["ay"].get<int>();
                                data_info.texture_info.aw = data_value["default"]["aw"].get<int>();
                                data_info.texture_info.ah = data_value["default"]["ah"].get<int>();
                                packet_info.data[data_id] = data_info;
                            }
                            subgroup_info.packet[packet_id] = packet_info;
                        }
                        else
                        {
                            auto data_info = DataInfo{
                                .type = exchange_data_type(packet_value["type"].get<std::string>()),
                                .path = packet_value["path"].get<std::string>(),
                            };
                            subgroup_info.packet[general_type_name].data[packet_id] = data_info;
                        }
                    }
                    if (!is_image) {
                        subgroup_info.packet[general_type_name].type = exchange_data_type(subgroup_value["packet"]["type"].get<std::string>());
                    }
                    group_info.subgroup[subgroup_id] = subgroup_info;
                }
            }
            return;
        }
    };

}