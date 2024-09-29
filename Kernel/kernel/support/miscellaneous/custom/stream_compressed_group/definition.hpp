#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup
{
    using ImageAdditional = Sen::Kernel::Support::PopCap::Animation::Convert::ImageAdditional;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    enum DecodeMethod
    {
        Simple = 0,
        Advanced,
        Debug
    };

    enum TextureFormatCategory
    {
        Android,
        IOS,
        Chinese
    };

    struct Setting
    {
        DecodeMethod decode_method;
        bool animation_split_label;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Setting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["decode_method"] = nlohmann_json_t.decode_method;
        nlohmann_json_j["animation_split_label"] = nlohmann_json_t.animation_split_label;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Setting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("decode_method").get_to(nlohmann_json_t.decode_method);
        nlohmann_json_j.at("animation_split_label").get_to(nlohmann_json_t.animation_split_label);
        return;
    }

    // -------------------------------------------------------------------------

    enum class DataType : uint8_t
    {
        File,             // 0
        Image,            // 1 - ptx
        PopAnim,          // 2 - pam
        Data,             // 3 - rton
        SoundBank,        // 4 - bank
        DecodedSoundBank, // 5 - bank
        PrimeFont,        // 6 - font
        RenderEffect,     // 7 - effect
        ImageData         // 8 - imageinfo
    };

    struct TextureDefault
    {
        int ax;
        int ay;
        int aw;
        int ah;
        int x;
        int y;
        int cols = 1;
        int rows = 1;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TextureDefault &nlohmann_json_t) -> void
    {
        nlohmann_json_j["ax"] = nlohmann_json_t.ax;
        nlohmann_json_j["ay"] = nlohmann_json_t.ay;
        nlohmann_json_j["aw"] = nlohmann_json_t.aw;
        nlohmann_json_j["ah"] = nlohmann_json_t.ah;
        nlohmann_json_j["x"] = nlohmann_json_t.x;
        nlohmann_json_j["y"] = nlohmann_json_t.y;
        nlohmann_json_j["cols"] = nlohmann_json_t.cols;
        nlohmann_json_j["rows"] = nlohmann_json_t.rows;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TextureDefault &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("ax").get_to(nlohmann_json_t.ax);
        nlohmann_json_j.at("ay").get_to(nlohmann_json_t.ay);
        nlohmann_json_j.at("aw").get_to(nlohmann_json_t.aw);
        nlohmann_json_j.at("ah").get_to(nlohmann_json_t.ah);
        nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
        nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
        nlohmann_json_j.at("cols").get_to(nlohmann_json_t.cols);
        nlohmann_json_j.at("rows").get_to(nlohmann_json_t.rows);
        return;
    }

    struct DataCompressedInfo
    {
        DataType type;
        std::string path;
        TextureDefault texture_info;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const DataCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        nlohmann_json_j["texture_info"] = nlohmann_json_t.texture_info;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        DataCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        nlohmann_json_j.at("texture_info").get_to(nlohmann_json_t.texture_info);
        return;
    }

    struct ImageDimension
    {
        int width;
        int height;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ImageDimension &nlohmann_json_t) -> void
    {
        nlohmann_json_j["width"] = nlohmann_json_t.width;
        nlohmann_json_j["height"] = nlohmann_json_t.height;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ImageDimension &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("width").get_to(nlohmann_json_t.width);
        nlohmann_json_j.at("height").get_to(nlohmann_json_t.height);
        return;
    }

    struct TextureResourceAdditional
    {
        int index;
        int format;
        int pitch;
        int alpha_size;
        int scale;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TextureResourceAdditional &nlohmann_json_t) -> void
    {
        nlohmann_json_j["index"] = nlohmann_json_t.index;
        nlohmann_json_j["format"] = nlohmann_json_t.format;
        nlohmann_json_j["pitch"] = nlohmann_json_t.pitch;
        nlohmann_json_j["alpha_size"] = nlohmann_json_t.alpha_size;
        nlohmann_json_j["scale"] = nlohmann_json_t.scale;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TextureResourceAdditional &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("index").get_to(nlohmann_json_t.index);
        nlohmann_json_j.at("format").get_to(nlohmann_json_t.format);
        nlohmann_json_j.at("pitch").get_to(nlohmann_json_t.pitch);
        nlohmann_json_j.at("alpha_size").get_to(nlohmann_json_t.alpha_size);
        nlohmann_json_j.at("scale").get_to(nlohmann_json_t.scale);
        return;
    }

    struct TexturePacketCompressedInfo
    {
        std::string path;
        ImageDimension dimension;
        TextureResourceAdditional additional;
        tsl::ordered_map<string, DataCompressedInfo> data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TexturePacketCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        nlohmann_json_j["dimension"] = nlohmann_json_t.dimension;
        nlohmann_json_j["additional"] = nlohmann_json_t.additional;
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TexturePacketCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        nlohmann_json_j.at("dimension").get_to(nlohmann_json_t.dimension);
        nlohmann_json_j.at("additional").get_to(nlohmann_json_t.additional);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct TextureSubgroupCompressedInfo
    {
        int resolution;
        std::string locale;
        std::map<std::string, TexturePacketCompressedInfo> packet;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TextureSubgroupCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["resolution"] = nlohmann_json_t.resolution;
        nlohmann_json_j["locale"] = nlohmann_json_t.locale;
        nlohmann_json_j["packet"] = nlohmann_json_t.packet;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TextureSubgroupCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("resolution").get_to(nlohmann_json_t.resolution);
        nlohmann_json_j.at("locale").get_to(nlohmann_json_t.locale);
        nlohmann_json_j.at("packet").get_to(nlohmann_json_t.packet);
        return;
    }

    struct GenenalSubgroupCompressedInfo
    {
        std::string locale;
        tsl::ordered_map<std::string, DataCompressedInfo> data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GenenalSubgroupCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["locale"] = nlohmann_json_t.locale;
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GenenalSubgroupCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("locale").get_to(nlohmann_json_t.locale);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct SubgroupCompressedInfo
    {
        GenenalSubgroupCompressedInfo general;
        TextureSubgroupCompressedInfo texture;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SubgroupCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["general"] = nlohmann_json_t.general;
        nlohmann_json_j["texture"] = nlohmann_json_t.texture;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SubgroupCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("general").get_to(nlohmann_json_t.general);
        nlohmann_json_j.at("texture").get_to(nlohmann_json_t.texture);
        return;
    }

    struct GroupCompressedInfo
    {
        bool composite;
        std::map<std::string, SubgroupCompressedInfo> subgroup;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GroupCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["composite"] = nlohmann_json_t.composite;
        nlohmann_json_j["subgroup"] = nlohmann_json_t.subgroup;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GroupCompressedInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("composite").get_to(nlohmann_json_t.composite);
        nlohmann_json_j.at("subgroup").get_to(nlohmann_json_t.subgroup);
        return;
    }

    enum ExpandPath
    {
        String,
        Array
    };

    struct CustomResourceInformation
    {
        ExpandPath expand_path;
        std::map<std::string, GroupCompressedInfo> group;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const CustomResourceInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["expand_path"] = nlohmann_json_t.expand_path;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        CustomResourceInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("expand_path").get_to(nlohmann_json_t.expand_path);
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        return;
    }

    // ------------------------------------------------------------------------------

    struct ResourceInformation
    {
        DataType type;
        std::string path;
        bool use_image_additional;
        ImageAdditional additional;
    };

    /*
    inline static auto k_data_type_table = std::map<DataType, std::string_view>{
            {DataType::Image, "Image"_sv}, {DataType::PopAnim, "PopAnim"_sv}, {DataType::Data, "Data"_sv}, {DataType::SoundBank, "SoundBank"_sv}, {DataType::DecodedSoundBank, "DecodedSoundBank"_sv}, {DataType::PrimeFont, "PrimeFont"_sv}, {DataType::RenderEffect, "RenderEffect"_sv}, {DataType::ImageData, "ImageData"_sv}, {DataType::File, "File"_sv}};

        inline static auto exchange_data_type(
            DataType const &data) -> std::string_view
        {
            if (auto it = search_element_in_map(k_data_type_table, data); it != k_data_type_table.end())
            {
                return it->second;
            }
            assert_conditional(false, String::format(fmt::format("{}", Language::get("pvz2.scg.invalid_data_type")), std::to_string(static_cast<uint8_t>(data))), "exchange_data_type");
        }

        inline static auto exchange_data_type(
            std::string_view const &data) -> DataType
        {
            if (auto it = search_element_in_map(k_data_type_table, data); it != k_data_type_table.end())
            {
                return it->first;
            }
            assert_conditional(false, String::format(fmt::format("{}", Language::get("pvz2.scg.invalid_data_type")), data), "exchange_data_type");
        }
    */

    inline static auto exchange_data_type(
        DataType const &data) -> std::string_view
    {
        return magic_enum::enum_name(data);
    }

    inline static auto exchange_data_type(
        std::string_view const &data) -> DataType
    {
        auto data_type = magic_enum::enum_cast<DataType>(data);
        assert_conditional(data_type.has_value(), String::format(fmt::format("{}", Language::get("pvz2.scg.invalid_data_type")), data), "exchange_data_type");
        return data_type.value();
    }

    inline static auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ResourceInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = exchange_data_type(nlohmann_json_t.type);
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        if (nlohmann_json_t.use_image_additional)
        {
            nlohmann_json_j["additional"] = nlohmann_json_t.additional;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ResourceInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_t.type = exchange_data_type(nlohmann_json_j["type"].get<std::string>());
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        try
        {
            nlohmann_json_j.at("additional").get_to(nlohmann_json_t.additional);
            nlohmann_json_t.use_image_additional = true;
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        return;
    }

    struct SubgroupCategory
    {
        bool common_type;
        std::string locale;
        uint32_t compression;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SubgroupCategory &nlohmann_json_t) -> void
    {
        nlohmann_json_j["common_type"] = nlohmann_json_t.common_type;
        if (!nlohmann_json_t.locale.empty())
        {
            nlohmann_json_j["locale"] = nlohmann_json_t.locale;
        }
        else
        {
            nlohmann_json_j["locale"] = nullptr;
        }
        nlohmann_json_j["compression"] = nlohmann_json_t.compression;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SubgroupCategory &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("common_type").get_to(nlohmann_json_t.common_type);
        if (nlohmann_json_j.at("locale") != nullptr)
        {
            nlohmann_json_j.at("locale").get_to(nlohmann_json_t.locale);
        }
        nlohmann_json_j.at("compression").get_to(nlohmann_json_t.compression);
        return;
    }

    struct SubgroupDataInformation
    {
        SubgroupCategory category;
        tsl::ordered_map<std::string, ResourceInformation> resource;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SubgroupDataInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["category"] = nlohmann_json_t.category;
        nlohmann_json_j["resource"] = nlohmann_json_t.resource;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SubgroupDataInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("category").get_to(nlohmann_json_t.category);
        nlohmann_json_j.at("resource").get_to(nlohmann_json_t.resource);
        return;
    }

    struct GroupCategoryInfo
    {
        std::vector<int> resolution;
        int format;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GroupCategoryInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["resolution"] = nlohmann_json_t.resolution;
        nlohmann_json_j["format"] = nlohmann_json_t.format;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GroupCategoryInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("resolution").get_to(nlohmann_json_t.resolution);
        nlohmann_json_j.at("format").get_to(nlohmann_json_t.format);
        return;
    }

    struct InformationStructure
    {
        std::string expand_method;
        uint32_t version;
        TextureFormatCategory texture_format_category;
        bool composite;
        GroupCategoryInfo category;
        std::map<std::string, SubgroupDataInformation> subgroup;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const InformationStructure &nlohmann_json_t) -> void
    {
        if (!nlohmann_json_t.expand_method.empty())
        {
            nlohmann_json_j["#expand_method"] = nlohmann_json_t.expand_method;
        }
        else
        {
            nlohmann_json_j["#expand_method"] = nullptr;
        }
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["texture_format_category"] = nlohmann_json_t.texture_format_category;
        nlohmann_json_j["composite"] = nlohmann_json_t.composite;
        if (nlohmann_json_t.composite)
        {
            nlohmann_json_j["category"] = nlohmann_json_t.category;
        }
        else
        {
            nlohmann_json_j["category"] = nullptr;
        }
        nlohmann_json_j["subgroup"] = nlohmann_json_t.subgroup;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        InformationStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("texture_format_category").get_to(nlohmann_json_t.texture_format_category);
        nlohmann_json_j.at("composite").get_to(nlohmann_json_t.composite);
        if (nlohmann_json_t.composite)
        {
            nlohmann_json_j.at("category").get_to(nlohmann_json_t.category);
        }
        nlohmann_json_j.at("subgroup").get_to(nlohmann_json_t.subgroup);
        return;
    }

    //------------------------------------------------------------------------------

    struct DebuggerSubgroupInformation
    {
        bool is_image;
        std::string packet_path;
        std::string info_path;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const DebuggerSubgroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["is_image"] = nlohmann_json_t.is_image;
        nlohmann_json_j["packet_path"] = nlohmann_json_t.packet_path;
        nlohmann_json_j["info_path"] = nlohmann_json_t.info_path;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        DebuggerSubgroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("is_image").get_to(nlohmann_json_t.is_image);
        nlohmann_json_j.at("packet_path").get_to(nlohmann_json_t.packet_path);
        nlohmann_json_j.at("info_path").get_to(nlohmann_json_t.info_path);
        return;
    }

    struct DebuggerInformation
    {
        std::string expand_method = "debug";
        uint32_t version;
        TextureFormatCategory texture_format_category;
        bool composite;
        std::map<std::string, DebuggerSubgroupInformation> subgroup;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const DebuggerInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["#expand_method"] = nlohmann_json_t.expand_method;
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["texture_format_category"] = nlohmann_json_t.texture_format_category;
        nlohmann_json_j["composite"] = nlohmann_json_t.composite;
        nlohmann_json_j["subgroup"] = nlohmann_json_t.subgroup;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        DebuggerInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("texture_format_category").get_to(nlohmann_json_t.texture_format_category);
        nlohmann_json_j.at("composite").get_to(nlohmann_json_t.composite);
        nlohmann_json_j.at("subgroup").get_to(nlohmann_json_t.subgroup);
        return;
    }
}