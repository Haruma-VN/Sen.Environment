#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Animation
{

#pragma region AnimationSize
    struct AnimationSize
    {
    public:
        int width;
        int height;
        explicit AnimationSize(

            ) = default;

        explicit constexpr AnimationSize(
            int width,
            int height) : width(width), height(height)
        {
        }

        ~AnimationSize(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationSize & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"width", anim.width},
            {"height", anim.height}
        };
        return;
    };

#pragma endregion

#pragma region AnimationPosition
    struct AnimationPosition
    {
    public:
        int x;
        int y;
        explicit AnimationPosition(

            ) = default;

        explicit constexpr AnimationPosition(
            int x,
            int y) : x(x), y(y)
        {
        }

        ~AnimationPosition(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationPosition & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"x", anim.x},
            {"x", anim.y}
        };
        return;
    };

#pragma endregion

#pragma region using

    using Sen::Kernel::Definition::DataStreamView;

#pragma endregion

#pragma region definition

    struct Definition
    {

    public:
        // magic

        inline static constexpr auto magic = 3136297300;

        inline static constexpr auto version = std::array<int, 6>{1, 2, 3, 4, 5, 6};
    };

#pragma region FrameFlags

    struct FrameFlags
    {
    public:
        inline static constexpr auto remove = 1;
        inline static constexpr auto append = 2;
        inline static constexpr auto change = 4;
        inline static constexpr auto label = 8;
        inline static constexpr auto stop = 16;
        inline static constexpr auto command = 32;
    };

#pragma endregion

#pragma region MoveFlags

    struct MoveFlags
    {
    public:
        inline static constexpr auto src_react = 32768;
        inline static constexpr auto rotate = 16384;
        inline static constexpr auto color = 8192;
        inline static constexpr auto matrix = 4096;
        inline static constexpr auto long_coords = 2048;
        inline static constexpr auto sprite_frame_number = 1024;
    };

#pragma endregion

#pragma region AnimationImage

    struct AnimationImage
    {
    public:
        std::string_view name;
        AnimationSize size;
        std::vector<double> transform;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationImage & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"name", anim.name},
            {"size", anim.size},
            {"transform", anim.transform}
        };
        return;
    };

#pragma endregion


#pragma region AnimationWorkArea

    struct AnimationWorkArea
    {
    public:
        int index;
        int duration;
        explicit AnimationWorkArea(

            ) = default;

        explicit constexpr AnimationWorkArea(
            int index,
            int duration) : index(index), duration(duration)
        {
        }

        ~AnimationWorkArea(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationWorkArea & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"index", anim.index},
            {"duration", anim.duration}
        };
        return;
    };
#pragma endregion


#pragma region AnimationCommand

    struct AnimationCommand
    {
    public:
        std::string_view command;
        std::string_view parameter;
        explicit AnimationCommand(

            ) = default;

        explicit constexpr AnimationCommand(
            std::string_view command,
            std::string_view parameter) : command(command), parameter(parameter)
        {
        }

        ~AnimationCommand(

            ) = default;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationCommand & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"command", anim.command},
            {"parameter", anim.parameter}
        };
        return;
    };

#pragma endregion



#pragma region AnimationAppend

    struct AnimationAppend
    {
    public:
        std::string_view name;
        int index;
        int resource;
        bool sprite;
        bool additive;
        int preload_frame;
        int time_scale = 1;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationAppend & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"name", anim.name},
            {"index", anim.index},
            {"resource", anim.resource},
            {"sprite", anim.sprite},
            {"additive", anim.additive},
            {"preload_frame", anim.preload_frame},
            {"time_scale", anim.time_scale}
        };
        return;
    };

#pragma endregion



#pragma region AnimationMove

    struct AnimationMove
    {
    public:
        int index;
        std::vector<double> transform;
        std::array<double, 4> color{1, 1, 1, 1};
        std::array<double, 4> source_rectangle{0, 0, 0, 0};
        int sprite_frame_number;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationMove & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"index", anim.index},
            {"transform", anim.transform},
            {"color", anim.color},
            {"source_rectangle", anim.source_rectangle},
            {"sprite_frame_number", anim.sprite_frame_number}
        };
        return;
    };

#pragma endregion


#pragma region AnimationFrame

    struct AnimationFrame
    {
    public:
        std::string_view label;
        bool stop;
        std::vector<AnimationCommand> command;
        std::vector<int> remove;
        std::vector<AnimationAppend> append;
        std::vector<AnimationMove> change;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationFrame & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"label", anim.label},
            {"stop", anim.stop},
            {"command", anim.command},
            {"remove", anim.remove},
            {"append", anim.append},
            {"change", anim.change}
        };
        return;
    };

#pragma endregion

#pragma region AnimationSprite

    struct AnimationSprite
    {
    public:
        std::string_view description;
        AnimationWorkArea work_area;
        std::vector<AnimationFrame> frame;
    };

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const AnimationSprite & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"description", anim.description},
            {"work_area", anim.work_area},
            {"frame", anim.frame}
        };
        return;
    };

#pragma endregion

#pragma region SexyAnimation

    struct SexyAnimation
    {
    public:
        int version;
        int frame_rate;
        AnimationPosition position;
        AnimationSize size;
        std::map<std::string_view, AnimationImage> image;
        std::map<std::string_view, AnimationSprite> sprite;
        AnimationSprite main_sprite;

        explicit SexyAnimation(

            ) = default;

        explicit SexyAnimation(
            int version,
            int frame_rate
        ) : version(version), frame_rate(frame_rate)
        {
        }

        ~SexyAnimation(

            ) = default;
    };
    

    inline static auto to_json(
        nlohmann::ordered_json & json,
        const SexyAnimation & anim
    ) -> void {
        json = nlohmann::ordered_json{
            {"version", anim.version},
            {"frame_rate", anim.frame_rate},
            {"position", anim.position},
            {"size", anim.size},
            {"image", anim.image},
            {"sprite", anim.sprite},
            {"main_sprite", anim.main_sprite}
        };
        return;
    };

#pragma endregion
}
