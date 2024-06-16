#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Animation
{

#pragma region AnimationSize
    struct AnimationSize
    {
    public:
        double width;
        double height;
        explicit AnimationSize(

            ) = default;

        explicit constexpr AnimationSize(
            double width,
            double height) : width(width), height(height)
        {
        }

        ~AnimationSize(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationSize &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"width", anim.width},
            {"height", anim.height}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationSize &anim) -> void
    {
        json.at("width").get_to(anim.width);
        json.at("height").get_to(anim.height);
        return;
    }

#pragma endregion

#pragma region AnimationPosition
    struct AnimationPosition
    {
    public:
        double x;
        double y;
        explicit AnimationPosition(

            ) = default;

        explicit constexpr AnimationPosition(
            double x,
            double y) : x(x), y(y)
        {
        }

        ~AnimationPosition(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationPosition &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"x", anim.x},
            {"y", anim.y}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationPosition &anim) -> void
    {
        json.at("x").get_to(anim.x);
        json.at("y").get_to(anim.y);
        return;
    }

#pragma endregion

#pragma region using

    using Sen::Kernel::Definition::DataStreamView;

#pragma endregion

#pragma region AnimationImage

    struct AnimationImage
    {
    public:
        std::string name;
        std::string id;
        AnimationSize size;
        std::vector<double> transform;

        explicit AnimationImage(

            ) = default;

        ~AnimationImage(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationImage &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"name", anim.name},
            {"id", anim.id},
            {"size", anim.size},
            {"transform", anim.transform}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationImage &anim) -> void
    {
        json.at("name").get_to(anim.name);
        json.at("id").get_to(anim.id);
        json.at("size").get_to(anim.size);
        json.at("transform").get_to(anim.transform);
        return;
    }

#pragma endregion

#pragma region AnimationWorkArea

    struct AnimationWorkArea
    {
    public:
        int16_t start = 0;
        int16_t duration = 0;
        explicit AnimationWorkArea(

            ) = default;

        explicit constexpr AnimationWorkArea(
            int16_t start,
            int16_t duration) : start(start), duration(duration)
        {
        }

        ~AnimationWorkArea(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationWorkArea &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"start", anim.start},
            {"duration", anim.duration}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationWorkArea &anim) -> void
    {
        json.at("start").get_to(anim.start);
        json.at("duration").get_to(anim.duration);
        return;
    }

#pragma endregion

#pragma region AnimationCommand

    struct AnimationCommand
    {
    public:
        std::string command;
        std::string argument;
        explicit AnimationCommand(

            ) = default;

        explicit constexpr AnimationCommand(
            std::string command,
            std::string argument) : command(command), argument(argument)
        {
        }

        ~AnimationCommand(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationCommand &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"command", anim.command},
            {"argument", anim.argument}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationCommand &anim) -> void
    {
        json.at("command").get_to(anim.command);
        json.at("argument").get_to(anim.argument);
        return;
    }

#pragma endregion

#pragma region AnimationAppend

    struct AnimationAppend
    {
    public:
        int index;
        std::string name;
        uint16_t resource;
        bool sprite;
        bool additive;
        int16_t preload_frame;
        double time_scale = 1.0;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationAppend &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"index", anim.index},
            {"name", anim.name},
            {"resource", anim.resource},
            {"sprite", anim.sprite},
            {"additive", anim.additive},
            {"preload_frame", anim.preload_frame},
            {"time_scale", anim.time_scale}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationAppend &anim) -> void
    {
        json.at("name").get_to(anim.name);
        json.at("index").get_to(anim.index);
        json.at("resource").get_to(anim.resource);
        json.at("sprite").get_to(anim.sprite);
        json.at("additive").get_to(anim.additive);
        json.at("preload_frame").get_to(anim.preload_frame);
        json.at("time_scale").get_to(anim.time_scale);
        return;
    }

#pragma endregion

#pragma region AnimationChange

    struct AnimationChange
    {
    public:
        int index;
        std::vector<double> transform;
        std::array<double, 4> color;
        std::array<double, 4> source_rectangle;
        int16_t sprite_frame_number = 0;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationChange &anim) -> void
    {
        nlohmann::ordered_json color = nullptr;
        if (!(anim.color.at(0) == 0.0 && anim.color.at(1) == 0.0 && anim.color.at(2) == 0.0 && anim.color.at(3) == 0.0))
        {
            color = anim.color;
        }
        nlohmann::ordered_json source_rectangle = nullptr;
        if (!(anim.source_rectangle.at(0) == 0.0 && anim.source_rectangle.at(1) == 0.0 && anim.source_rectangle.at(2) == 0.0 && anim.source_rectangle.at(3) == 0.0))
        {
            source_rectangle = anim.source_rectangle;
        }
        json = nlohmann::ordered_json{
            {"index", anim.index},
            {"transform", anim.transform},
            {"color", color},
            {"sprite_frame_number", anim.sprite_frame_number},
            {"source_rectangle", source_rectangle}
            };
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationChange &anim) -> void
    {
        json.at("index").get_to(anim.index);
        json.at("transform").get_to(anim.transform);
        if (json.at("color") != nullptr)
        {
            json.at("color").get_to(anim.color);
        }
        json.at("sprite_frame_number").get_to(anim.sprite_frame_number);
        if (json.at("source_rectangle") != nullptr)
        {
            json.at("source_rectangle").get_to(anim.source_rectangle);
        }
        return;
    }

#pragma endregion

#pragma region AnimationFrame

    struct AnimationFrame
    {
    public:
        std::string label;
        bool stop;
        std::vector<AnimationCommand> command;
        std::vector<int> remove;
        std::vector<AnimationAppend> append;
        std::vector<AnimationChange> change;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationFrame &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"label", anim.label},
            {"stop", anim.stop},
            {"command", anim.command},
            {"remove", anim.remove},
            {"append", anim.append},
            {"change", anim.change}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationFrame &anim) -> void
    {
        json.at("label").get_to(anim.label);
        json.at("stop").get_to(anim.stop);
        json.at("command").get_to(anim.command);
        json.at("remove").get_to(anim.remove);
        json.at("append").get_to(anim.append);
        json.at("change").get_to(anim.change);
        return;
    }

#pragma endregion

#pragma region AnimationSprite

    struct AnimationSprite
    {
    public:
        std::string name;
        AnimationWorkArea work_area;
        std::vector<AnimationFrame> frame;

        explicit AnimationSprite(
            const std::string &name,
            const AnimationWorkArea &work_area,
            const std::vector<AnimationFrame> &frame) : name(name), work_area(work_area), frame(frame)
        {
        }

        explicit AnimationSprite(

            ) = default;

        ~AnimationSprite(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const AnimationSprite &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"name", anim.name},
            {"work_area", anim.work_area},
            {"frame", anim.frame}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        AnimationSprite &anim) -> void
    {
        json.at("name").get_to(anim.name);
        json.at("work_area").get_to(anim.work_area);
        json.at("frame").get_to(anim.frame);
        return;
    }

#pragma endregion

#pragma region SexyAnimation

    struct SexyAnimation
    {
    public:
        int version;
        uint8_t frame_rate;
        AnimationPosition position;
        AnimationSize size;
        std::vector<AnimationImage> image;
        std::vector<AnimationSprite> sprite;
        AnimationSprite main_sprite;

        SexyAnimation() = default;

        ~SexyAnimation(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json &json,
        const SexyAnimation &anim) -> void
    {
        json = nlohmann::ordered_json{
            {"version", anim.version},
            {"frame_rate", anim.frame_rate},
            {"position", anim.position},
            {"size", anim.size},
            {"image", anim.image},
            {"sprite", anim.sprite},
            {"main_sprite", anim.main_sprite}};
        return;
    };

    inline static auto from_json(
        const nlohmann::ordered_json &json,
        SexyAnimation &anim) -> void
    {
        json.at("version").get_to(anim.version);
        json.at("frame_rate").get_to(anim.frame_rate);
        json.at("position").get_to(anim.position);
        json.at("size").get_to(anim.size);
        json.at("image").get_to(anim.image);
        json.at("sprite").get_to(anim.sprite);
        json.at("main_sprite").get_to(anim.main_sprite);
        return;
    }

#pragma endregion
}
