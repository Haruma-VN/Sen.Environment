#pragma once

#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{
    using SpriteInfo = std::vector<std::string>;

    struct Position
    {
        int x;
        int y;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Position &nlohmann_json_t) -> void
    {
        nlohmann_json_j["x"] = nlohmann_json_t.x;
        nlohmann_json_j["y"] = nlohmann_json_t.y;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Position &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
        nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
        return;
    }

    struct ImageAdditional
    {
        int x;
        int y;
        int rows;
        int cols;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ImageAdditional &nlohmann_json_t) -> void
    {

        if (nlohmann_json_t.x != 0)
        {
            nlohmann_json_j["additional"]["x"] = nlohmann_json_t.x;
        }
        if (nlohmann_json_t.y != 0)
        {
            nlohmann_json_j["additional"]["y"] = nlohmann_json_t.y;
        }
        if (nlohmann_json_t.rows != 1)
        {
            nlohmann_json_j["additional"]["rows"] = nlohmann_json_t.rows;
        }
        if (nlohmann_json_t.cols != 1)
        {
            nlohmann_json_j["additional"]["cols"] = nlohmann_json_t.cols;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ImageAdditional &nlohmann_json_t) -> void
    {
        try
        {
            nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("rows").get_to(nlohmann_json_t.rows);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("cols").get_to(nlohmann_json_t.cols);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
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

    struct ImageInfo
    {
        std::string name;
        std::string id;
        ImageDimension size;
        bool use_image_additional;
        ImageAdditional additional;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ImageInfo &nlohmann_json_t) -> void
    {
        if (!nlohmann_json_t.name.empty())
        {
            nlohmann_json_j["name"] = nlohmann_json_t.name;
        }
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["size"] = nlohmann_json_t.size;
        if (nlohmann_json_t.use_image_additional) {
            nlohmann_json_j["additional"] = nlohmann_json_t.additional;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ImageInfo &nlohmann_json_t) -> void
    {
        try
        {
            nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("size").get_to(nlohmann_json_t.size);
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

    struct ExtraInfo
    {
        int version;
        int resolution;
        Position position;
        tsl::ordered_map<std::string, ImageInfo> image;
        std::map<std::string, SpriteInfo> sprite;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ExtraInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["resolution"] = nlohmann_json_t.resolution;
        nlohmann_json_j["position"] = nlohmann_json_t.position;
        nlohmann_json_j["image"] = nlohmann_json_t.image;
        nlohmann_json_j["sprite"] = nlohmann_json_t.sprite;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ExtraInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("resolution").get_to(nlohmann_json_t.resolution);
        nlohmann_json_j.at("position").get_to(nlohmann_json_t.position);
        nlohmann_json_j.at("image").get_to(nlohmann_json_t.image);
        nlohmann_json_j.at("sprite").get_to(nlohmann_json_t.sprite);
        return;
    }
}