#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/common.hpp"
#include "kernel/support/popcap/resource_stream_bundle/common.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamGroup
{

    //----------------------------------------------

    struct PacketCompression
    {
        bool general;
        bool texture;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PacketCompression &nlohmann_json_t) -> void
    {
        nlohmann_json_j["general"] = nlohmann_json_t.general;
        nlohmann_json_j["texture"] = nlohmann_json_t.texture;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PacketCompression &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("general").get_to(nlohmann_json_t.general);
        nlohmann_json_j.at("texture").get_to(nlohmann_json_t.texture);
        return;
    }

    struct GeneralResourceAdditional
    {
        // std::string_view type = k_general_type_string;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GeneralResourceAdditional &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = Common::k_general_type_string;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GeneralResourceAdditional &nlohmann_json_t) -> void
    {
        assert(nlohmann_json_j.at("type").get<std::string_view>() == Common::k_general_type_string);
        return;
    }

    struct TextureResourceInformation
    {
        uint32_t pitch;
        uint32_t format;
        uint32_t additional_byte_count;
        uint32_t scale;
    };

    struct TextureDimension
    {
        int width;
        int height;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TextureDimension &nlohmann_json_t) -> void
    {
        nlohmann_json_j["width"] = nlohmann_json_t.width;
        nlohmann_json_j["height"] = nlohmann_json_t.height;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TextureDimension &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("width").get_to(nlohmann_json_t.width);
        nlohmann_json_j.at("height").get_to(nlohmann_json_t.height);
        return;
    }

    struct TextureValue
    {
        int index;
        TextureDimension dimension;
        size_t texture_resource_information_section_block_size = 0;
        TextureResourceInformation texture_infomation;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TextureValue &nlohmann_json_t) -> void
    {
        nlohmann_json_j["index"] = nlohmann_json_t.index;
        nlohmann_json_j["dimension"] = nlohmann_json_t.dimension;
        switch (nlohmann_json_t.texture_resource_information_section_block_size)
        {
        case ResourceStreamBundle::Common::k_texture_resource_information_section_block_size_version_0:
        {
            nlohmann_json_j["format"] = nlohmann_json_t.texture_infomation.format;
            nlohmann_json_j["pitch"] = nlohmann_json_t.texture_infomation.pitch;
            break;
        }
        case ResourceStreamBundle::Common::k_texture_resource_information_section_block_size_version_1:
        {
            nlohmann_json_j["format"] = nlohmann_json_t.texture_infomation.format;
            nlohmann_json_j["pitch"] = nlohmann_json_t.texture_infomation.pitch;
            nlohmann_json_j["additional_byte_count"] = nlohmann_json_t.texture_infomation.additional_byte_count;
            break;
        }
        case ResourceStreamBundle::Common::k_texture_resource_information_section_block_size_version_2:
        {
            nlohmann_json_j["format"] = nlohmann_json_t.texture_infomation.format;
            nlohmann_json_j["pitch"] = nlohmann_json_t.texture_infomation.pitch;
            nlohmann_json_j["additional_byte_count"] = nlohmann_json_t.texture_infomation.additional_byte_count;
            nlohmann_json_j["scale"] = nlohmann_json_t.texture_infomation.scale;
            break;
        }
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TextureValue &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("index").get_to(nlohmann_json_t.index);
        nlohmann_json_j.at("dimension").get_to(nlohmann_json_t.dimension);
        auto texture_resource_information_section_block_size = size_t{0};
        try
        {
            nlohmann_json_j.at("format").get_to(nlohmann_json_t.texture_infomation.format);
            texture_resource_information_section_block_size += size_t{4};
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("pitch").get_to(nlohmann_json_t.texture_infomation.pitch);
            texture_resource_information_section_block_size += size_t{4};
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("additional_byte_count").get_to(nlohmann_json_t.texture_infomation.additional_byte_count);
            texture_resource_information_section_block_size += size_t{4};
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("scale").get_to(nlohmann_json_t.texture_infomation.scale);
            texture_resource_information_section_block_size += size_t{4};
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        if (texture_resource_information_section_block_size != size_t{0}) {
            nlohmann_json_t.texture_resource_information_section_block_size = texture_resource_information_section_block_size + size_t{8}; 
        } 
        return;
    }

    struct TextureResourceAdditional
    {
        // std::string_view type = k_texture_type_string;
        TextureValue value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TextureResourceAdditional &nlohmann_json_t) -> void
    {
       // nlohmann_json_j["type"] = Common::k_texture_type_string;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TextureResourceAdditional &nlohmann_json_t) -> void
    {
      //  assert(nlohmann_json_j.at("type").get<std::string_view>() == Common::k_texture_type_string);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct Resource
    {
        std::string path;
        bool use_texture_additional_instead;
        GeneralResourceAdditional general_additional;
        TextureResourceAdditional texture_additional;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Resource &nlohmann_json_t) -> void
    {
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        if (!nlohmann_json_t.use_texture_additional_instead)
        {
          // nlohmann_json_j["additional"] = nlohmann_json_t.general_additional;
        }
        else
        {
            nlohmann_json_j["additional"] = nlohmann_json_t.texture_additional;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Resource &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        try
        {
            nlohmann_json_j.at("additional").get_to(nlohmann_json_t.texture_additional);
            nlohmann_json_t.use_texture_additional_instead = true;
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        /*
        switch (hash_sv(nlohmann_json_j["additional"]["type"].get<std::string_view>()))
        {
        case hash_sv(Common::k_general_type_string):
        {
            nlohmann_json_t.use_texture_additional_instead = false;
            nlohmann_json_j.at("additional").get_to(nlohmann_json_t.general_additional);
            break;
        }
        case hash_sv(Common::k_texture_type_string):
        {
            nlohmann_json_t.use_texture_additional_instead = true;
            nlohmann_json_j.at("additional").get_to(nlohmann_json_t.texture_additional);
            break;
        }
        default:
        {
            assert_conditional(false, "invalid_rsg_additional_type", "convert_json"); // TODO: add to localization.
        }
        }
        */
        return;
    }

    struct PacketStructure
    {
        uint32_t version;
        PacketCompression compression;
        std::vector<Resource> resource;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PacketStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["compression"] = nlohmann_json_t.compression;
        nlohmann_json_j["resource"] = nlohmann_json_t.resource;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PacketStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("compression").get_to(nlohmann_json_t.compression);
        nlohmann_json_j.at("resource").get_to(nlohmann_json_t.resource);
        return;
    }

}
