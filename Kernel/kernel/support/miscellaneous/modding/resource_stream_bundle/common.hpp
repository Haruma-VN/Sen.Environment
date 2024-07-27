#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/modding/resource_stream_bundle/definition.hpp"
#include "kernel/support/texture/invoke.hpp"
#include "kernel/support/miscellaneous/shared.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::ResourceStreamBundle
{
    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using namespace Definition;

    using ImageFormat = Sen::Kernel::Support::Texture::Format;

    using DataType = Sen::Kernel::Support::Miscellaneous::Modding::PacketContainsResourceGroup::Common::DataType;

    struct Common
    {
        inline constexpr static auto k_manifest_string = "__MANIFESTGROUP__";

        inline constexpr static auto k_package_string = "packages"_sv;

        inline constexpr static auto k_rton_extension_string = ".rton"_sv;

        inline constexpr static auto k_newton_extension_string = ".newton"_sv;

        inline constexpr static auto k_general_type_name = "general"_sv;

        inline constexpr static auto DefaultCoordinateOffset = 0;

        inline constexpr static auto DefaultLayoutOffset = 1;

        inline constexpr static auto packet_list = std::array<int, 2>{1536, 1200}; // 1200 for only 2c

        inline constexpr static auto empty_virtual_image_format = 0x1000_ui;

        inline static auto exchange_data_type(
            std::string const &value) -> DataType
        {
            switch (hash_sv(value))
            {
            case (hash_sv("Image")):
            {
                return DataType::Image;
            }
            case (hash_sv("PopAnim")):
            {
                return DataType::PopAnim;
            }
            case (hash_sv("Data")):
            {
                return DataType::Data;
            }
            case (hash_sv("SoundBank")):
            {
                return DataType::SoundBank;
            }
            case (hash_sv("DecodedSoundBank")):
            {
                return DataType::DecodedSoundBank;
            }
            case (hash_sv("PrimeFont")):
            {
                return DataType::PrimeFont;
            }
            case (hash_sv("RenderEffect")):
            {
                return DataType::RenderEffect;
            }
            case (hash_sv("File")):
            {
                return DataType::File;
            }
            default:
            {
                assert_conditional(false, String::format(fmt::format("{}", Language::get("pvz2.rsb.modding.invalid_data_type")), value), "exchange_data_type"); 
            }
            }
        }

        inline static auto exchange_data_type(
            DataType const &value) -> std::string_view
        {
            switch (value)
            {
            case DataType::Image:
            {
                return "Image"_sv;
            }
            case DataType::PopAnim:
            {
                return "PopAnim"_sv;
            }
            case DataType::Data:
            {
                return "Data"_sv;
            }
            case DataType::SoundBank:
            {
                return "SoundBank"_sv;
            }
            case DataType::DecodedSoundBank:
            {
                return "DecodedSoundBank"_sv;
            }
            case DataType::PrimeFont:
            {
                return "PrimeFont"_sv;
            }
            case DataType::RenderEffect:
            {
                return "RenderEffect"_sv;
            }
            default:
                return "File"_sv;
            }
        }

        template <auto is_ios_texture_format>
        inline static auto exchange_image_format(
            uint32_t format) -> ImageFormat
        {
            static_assert(is_ios_texture_format == true || is_ios_texture_format == false, "is_ios_texture_format must be true or false");
            if constexpr (is_ios_texture_format)
            {
                switch (format)
                {
                    case 0: {
                        return ImageFormat::ARGB_8888;
                    }
                    case 1: {
                        return ImageFormat::RGBA_4444;
                    }
                    //case 2: {
                      //  return ImageFormat::RGB_565;
                   // }
                    case 3: {
                        return ImageFormat::RGBA_5551;
                    }
                    case 21: {
                        return ImageFormat::RGBA_4444_TILED;
                    }
                    //case 22: {
                      //  return ImageFormat::RGB_565_TILED
                    //}
                    case 23: {
                        return ImageFormat::RGBA_5551_TILED;
                    }
                    case 30: {
                        return ImageFormat::RGB_PVRTC_4BPP;
                    }
                    case 148: {
                        return ImageFormat::RGB_PVRTC_4BPP_A_8;
                    }
                    default: {
                        assert_conditional(false, fmt::format("invalid_format: {}", format), "exchange_image_format");
                    }
                }
            }
            else
            {
                switch (format)
                {
                    case 0: {
                        return ImageFormat::RGBA_8888;
                    }
                    case 1: {
                        return ImageFormat::RGBA_4444;
                    }
                    case 2: {
                        return ImageFormat::RGB_565;
                    }
                    case 3: {
                        return ImageFormat::RGBA_5551;
                    }
                    case 21: {
                        return ImageFormat::RGBA_4444_TILED;
                    }
                    case 22: {
                       return ImageFormat::RGB_565_TILED;
                    }
                    case 23: {
                        return ImageFormat::RGBA_5551_TILED;
                    }
                    case 147: {
                        return ImageFormat::RGB_PVRTC_4BPP;
                    }
                    default: {
                        assert_conditional(false, fmt::format("invalid_format: {}", format), "exchange_image_format");
                    }
                }
            }
        }

        inline static auto check_etc_format(
            ImageFormat const &format,
            size_t const &texture_information_section_size
        ) -> ImageFormat
        {
            if (texture_information_section_size != Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::k_texture_resource_information_section_block_size_version_0) {
                return ImageFormat::RGB_ETC1_A_PALETTE;
            }
            return format;
        }

        template <auto skip_unnecessary_packet>
        inline static auto exchange_res_info(
            nlohmann::ordered_json &data,
            ResInfo &value) -> void
        {
            static_assert(skip_unnecessary_packet == true || skip_unnecessary_packet == false, "skip_unnecessary_packet must be true or false");
            auto general_type_name = std::string{k_general_type_name};
            value.expand_path = data["expand_path"].get<std::string>() == "string" ? ExpandPath::String : ExpandPath::Array;
            for (auto &[id, group_value] : data["groups"].items())
            {
                auto group_info = GroupInfo{
                    .is_composite = group_value["is_composite"].get<bool>()};
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
                            auto packet_info = PacketInfo{};
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
                                if (data_value["default"].find("cols") != data_value["default"].end() && data_value["default"]["cols"].get<int>() != DefaultLayoutOffset)
                                {
                                    data_info.texture_info.cols = data_value["default"]["cols"].get<int>();
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
                    group_info.subgroup[subgroup_id] = subgroup_info;
                }
                value.group[id] = group_info;
            }
            return;
        }
    };

}