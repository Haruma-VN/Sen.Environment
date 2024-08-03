#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/invoke.hpp"


namespace Sen::Kernel::Support::Miscellaneous::Modding::ResourceStreamBundle
{
    using TextureInformationVersion = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::TextureInformationVersion;
    
    using ImageFormat = Sen::Kernel::Support::Texture::Format;

    enum TextureFormatCategory
    {
        Android,
        IOS,
        Chinese,
    };

    enum ExpandPath
    {
        String, 
        Array
    };

    struct ResourceInfo
    {
        ExpandPath expand_path;
        bool use_new_type_resource;
        std::string resource_additional_name;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ResourceInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["expand_path"] = nlohmann_json_t.expand_path == String ? "string" : "array";
        nlohmann_json_j["use_new_type_resource"] = nlohmann_json_t.use_new_type_resource;
        if (nlohmann_json_t.resource_additional_name.empty())
        {
            nlohmann_json_j["resource_additional_name"] = nullptr;
        }
        else
        {
            nlohmann_json_j["resource_additional_name"] = nlohmann_json_t.resource_additional_name;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ResourceInfo &nlohmann_json_t) -> void
    {
        auto expand_path_string = nlohmann_json_j["expand_path"].get<std::string>();
        assert_conditional(!(expand_path_string != "string" && expand_path_string != "array"), String::format(fmt::format("{}", Language::get("pvz2.rsb.modding.invalid_expand_path")), expand_path_string), "from_json");
        nlohmann_json_t.expand_path = expand_path_string == "string" ? ExpandPath::String : ExpandPath::Array;
        nlohmann_json_j.at("use_new_type_resource").get_to(nlohmann_json_t.use_new_type_resource);
        if (nlohmann_json_j["resource_additional_name"] != nullptr)
        {
            nlohmann_json_j.at("resource_additional_name").get_to(nlohmann_json_t.resource_additional_name);
        }
        return;
    }

    struct InfoStructure
    {
        uint32_t version;
        TextureInformationVersion texture_information_version;
        TextureFormatCategory texture_format_category;
        ResourceInfo resource_info;
        // PackageInfo package_info;
        std::vector<std::string> group;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const InfoStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["texture_information_version"] = nlohmann_json_t.texture_information_version;
        nlohmann_json_j["texture_format_category"] = nlohmann_json_t.texture_format_category;
        nlohmann_json_j["resource_info"] = nlohmann_json_t.resource_info;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        InfoStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("texture_information_version").get_to(nlohmann_json_t.texture_information_version);
        nlohmann_json_j.at("texture_format_category").get_to(nlohmann_json_t.texture_format_category);
        nlohmann_json_j.at("resource_info").get_to(nlohmann_json_t.resource_info);
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        return;
    }

    // -------------------------------------------------------

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

    struct ImageInfo
    {
        std::string path;
        int index;
        ImageFormat format;
        ImageDimension dimension;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ImageInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        nlohmann_json_j["index"] = nlohmann_json_t.index;
        nlohmann_json_j["format"] = nlohmann_json_t.format;
        nlohmann_json_j["dimension"] = nlohmann_json_t.dimension;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ImageInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        nlohmann_json_j.at("index").get_to(nlohmann_json_t.index);
        nlohmann_json_j.at("format").get_to(nlohmann_json_t.format);
        nlohmann_json_j.at("dimension").get_to(nlohmann_json_t.dimension);
        return;
    }

    struct TextureDefault
    {
        int ax;
        int ay;
        int aw;
        int ah;
        int x;
        int y;
        int cols;
        int rows;
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

    enum DataType 
    {
        File,             // 0
        Image,            // 1 - ptx
        PopAnim,          // 2 - pam
        Data,             // 3 - rton
        SoundBank,        // 4 - bank
        DecodedSoundBank, // 5 - bank
        PrimeFont,        // 6 - font
        RenderEffect      // 7 effect
    };

    struct DataInfo
    {
        DataType type;
        std::string path;
        TextureDefault texture_info;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const DataInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        nlohmann_json_j["texture_info"] = nlohmann_json_t.texture_info;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        DataInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        nlohmann_json_j.at("texture_info").get_to(nlohmann_json_t.texture_info);
        return;
    }

    struct PacketInfo
    {
        DataType type;
        ImageInfo image_info;
        std::map<std::string, DataInfo> data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PacketInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["image_info"] = nlohmann_json_t.image_info;
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PacketInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        nlohmann_json_j.at("image_info").get_to(nlohmann_json_t.image_info);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct SubgroupInfo
    {
        int resolution;
        std::map<std::string, PacketInfo> packet;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SubgroupInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["resolution"] = nlohmann_json_t.resolution;
        nlohmann_json_j["packet"] = nlohmann_json_t.packet;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SubgroupInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("resolution").get_to(nlohmann_json_t.resolution);
        nlohmann_json_j.at("packet").get_to(nlohmann_json_t.packet);
        return;
    }

    struct GroupInfo
    {
        bool composite;
        std::map<std::string, SubgroupInfo> subgroup;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GroupInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["composite"] = nlohmann_json_t.composite;
        nlohmann_json_j["subgroup"] = nlohmann_json_t.subgroup;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GroupInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("composite").get_to(nlohmann_json_t.composite);
        nlohmann_json_j.at("subgroup").get_to(nlohmann_json_t.subgroup);
        return;
    }

    struct ResInfo
    {
        ExpandPath expand_path;
        std::map<std::string, GroupInfo> group;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ResInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["expand_path"] = nlohmann_json_t.expand_path;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ResInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("expand_path").get_to(nlohmann_json_t.expand_path);
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        return;
    }
}