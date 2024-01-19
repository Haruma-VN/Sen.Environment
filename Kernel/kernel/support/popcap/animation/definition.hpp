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
#pragma endregion


#pragma region AnimationSprite

    struct AnimationSprite
    {
    public:
        std::string_view description;
        AnimationWorkArea work_area;
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

#pragma endregion
}
