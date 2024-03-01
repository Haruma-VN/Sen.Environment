#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation
{
    using namespace Definition;

    template <typename T> requires std::is_integral<T>::value
    class Decode
    {

    public:
        
        SexyAnimation mutable json{};

    protected:

        T mutable version;
        
        std::unique_ptr<DataStreamView> sen;

        template <typename ...Args> requires (std::is_integral<Args>::value && ...)
        inline auto read_sprite(
            AnimationSprite &sprite,
            Args... index
        ) const -> void
        {
            static_assert(sizeof...(Args) == 1 || sizeof...(Args) == 0, "index can only be 0 or 1");
            [[maybe_unused]] auto sprite_name = std::string{};
            if constexpr (sizeof...(Args) == 1) {
                sprite_name = version >= 4 ? sen->readStringByUint16() : "*" + std::to_string(std::get<0>(std::make_tuple(index...)));
            }
            else {
                sen->readStringByUint16();
            }
            if (version >= 4)
            {
                if (version >= 6)
                {
                    sprite.description = sen->readStringByUint16();
                }
                sen->readUint32();
            }
            auto frame_count = sen->readUint16();
            if (version >= 5)
            {
                sprite.work_area = AnimationWorkArea{sen->readUint16(), sen->readUint16()};
                sprite.work_area.duration = frame_count;
            }
            else
            {
                sprite.work_area = AnimationWorkArea{0, frame_count};
            }
            for (auto i : Range(frame_count))
            {
                auto frame_info = AnimationFrame{};
                read_frame_info(&frame_info);
                sprite.frame.push_back(std::move(frame_info));
            }
            if constexpr (sizeof...(Args) == 1) {
                json.sprite[sprite_name] = sprite;
            }
            return;
            
        }

        inline auto read_frame_info (
            AnimationFrame* frame_info
        ) const -> void
        {
            auto flag = sen->readUint8();
            auto count = 0;
            if ((flag & FrameFlags::remove) != 0)
            {
                count = sen->readUint8();
                if (count == 255)
                {
                    count = sen->readUint16();
                }
                for (auto i : Range(count))
                {
                    frame_info->remove.emplace_back(read_remove<int>());
                }
            }
            if ((flag & FrameFlags::append) != 0)
            {
                count = sen->readUint8();
                if (count == 255)
                {
                    count = sen->readUint16();
                }
                for (auto i : Range(count))
                {
                    auto animation_append = AnimationAppend{};
                    read_append(&animation_append);
                    frame_info->append.push_back(std::move(animation_append));
                }
            }
            if ((flag & FrameFlags::change) != 0)
            {
                count = sen->readUint8();
                if (count == 255)
                {
                    count = sen->readUint16();
                }
                for (auto i : Range(count))
                {
                    auto anim_move = AnimationMove{};
                    read_move(&anim_move);
                    frame_info->change.push_back(std::move(anim_move));
                }
            }
            if ((flag & FrameFlags::label) != 0) {
                frame_info->label = sen->readStringByUint16();
            }
            if ((flag & FrameFlags::stop) != 0) {
                frame_info->stop = true;
            }
            if ((flag & FrameFlags::command) != 0) {
                count = sen->readUint8();
                for (auto i : Range(count)) {
                    auto animation_command = AnimationCommand{};
                    read_command(&animation_command);
                    frame_info->command.push_back(std::move(animation_command));
                }
            }
            return;
        }

        template <typename U> requires std::is_integral<U>::value
        inline auto read_remove(

        ) const -> U
        {
            auto index = static_cast<std::uint32_t>(sen->readUint16());
            if (index >= 2047)
            {
                index = sen->readUint32();
            }
            return index;
        }

        inline auto read_command(
            AnimationCommand* animation_command
        ) const -> void
        {
            animation_command->command = sen->readStringByUint16();
            animation_command->parameter = sen->readStringByUint16();
            return;
        }

        inline auto read_append (
            AnimationAppend* append_info
        ) const -> void
        {
            const auto value = sen->readUint16();
            auto index = value & 2047;
            if (index == 2047)
            {
                index = sen->readUint32();
            }
            append_info->index = index;
            append_info->sprite = (value & 32768) != 0;
            append_info->additive = (value & 16384) != 0;
            auto resource = static_cast<std::uint16_t>(sen->readUint8());
            if (version >= 6 and resource == 255)
            {
                resource = sen->readUint16();
            }
            append_info->resource = resource;
            if ((value & 8192) != 0)
            {
                append_info->preload_frame = sen->readUint16();
            }
            if ((value & 4096) != 0)
            {
                append_info->name = sen->readStringByUint16();
            }
            if ((value & 2048) != 0)
            {
                append_info->time_scale = sen->readInt32() / 65536.0;
            }
            return;
        }

        inline auto read_move(
            AnimationMove* change_info
        ) const -> void
        {
            const auto flag = sen->readUint16();
            auto index = flag & 1023;
            if (index == 1023)
            {
                index = sen->readUint32();
            }
            change_info->index = index;
            if ((flag & MoveFlags::matrix) != 0)
            {
                
                auto t1 = sen->readInt32() / 65536.0;
                auto t3 = sen->readInt32() / 65536.0;
                auto t2 = sen->readInt32() / 65536.0;
                auto t4 = sen->readInt32() / 65536.0;
                change_info->transform.emplace_back(t1);
                change_info->transform.emplace_back(t2);
                change_info->transform.emplace_back(t3);
                change_info->transform.emplace_back(t4);
            }
            else if ((flag & MoveFlags::rotate) != 0)
            {
                change_info->transform.emplace_back(sen->readInt16() / 1000.0);
            }
            if ((flag & MoveFlags::long_coords) != 0)
            {
                auto y = sen->readInt32() / 20.0;
                auto x = sen->readInt32() / 20.0;
                change_info->transform.emplace_back(y);
                change_info->transform.emplace_back(x);
            }
            else
            {
                auto y = sen->readInt16() / 20.0;
                auto x = sen->readInt16() / 20.0;
                change_info->transform.emplace_back(y);
                change_info->transform.emplace_back(x);
            }
            if ((flag & MoveFlags::src_react) != 0) {
                change_info->source_rectangle.emplace_back(sen->readInt16() / 20.0);
                change_info->source_rectangle.emplace_back(sen->readInt16() / 20.0);
                change_info->source_rectangle.emplace_back(sen->readInt16() / 20.0);
                change_info->source_rectangle.emplace_back(sen->readInt16() / 20.0);
            }
            if ((flag & MoveFlags::color) != 0) {
                change_info->color.emplace_back(sen->readUint8() / 255.0);
                change_info->color.emplace_back(sen->readUint8() / 255.0);
                change_info->color.emplace_back(sen->readUint8() / 255.0);
                change_info->color.emplace_back(sen->readUint8() / 255.0);
            }
            if ((flag & MoveFlags::sprite_frame_number) != 0) {
                change_info->sprite_frame_number = sen->readUint16();
            }
            return;
        }

        inline auto read_image(

        ) const -> void
        {
            auto image_name = String{sen->readStringByUint16()};
            auto name_list = image_name.split("|");
            if (json.image.contains(name_list[1]))
            {
                throw Exception(fmt::format("{}: {}", Language::get("popcap.animation.duplicate_image"), name_list[1]), std::source_location::current(), "read_image");
            }
            auto image = AnimationImage{};
            if (version >= 4)
            {
                image.size = AnimationSize{sen->readUint16(), sen->readUint16()};
            }
            image.name = std::string{name_list[0]};
            if (version == 1)
            {
                auto matrix = sen->readInt16() / 1000.0;
                image.transform = std::vector<double>{
                    Math::cos(matrix),
                    -Math::sin(matrix),
                    Math::sin(matrix),
                    Math::cos(matrix),
                    static_cast<double>(sen->readInt16() / 20.0),
                    static_cast<double>(sen->readInt16() / 20.0)};
            }
            else
            {
                image.transform = std::vector<double>{
                    static_cast<double>(sen->readInt32() / 1310720.0),
                    static_cast<double>(sen->readInt32() / 1310720.0),
                    static_cast<double>(sen->readInt32() / 1310720.0),
                    static_cast<double>(sen->readInt32() / 1310720.0),
                    static_cast<double>(sen->readInt16() / 20.0),
                    static_cast<double>(sen->readInt16() / 20.0)};
            }
            json.image[name_list[1]] = image;
            return;
        }

    public:

        explicit Decode(
            std::string_view source
        ) : sen(std::make_unique<DataStreamView>(source))
        {
        }

        inline auto process(

        ) const -> void
        {
            if (sen->readUint32() != Definition::magic)
            {
                throw Exception(fmt::format("{}", Language::get("popcap.animation.invalid_magic")), std::source_location::current(), "process");
            }
            version = sen->readUint32();
            auto index = std::find(Definition::version.begin(), Definition::version.end(), version);
            if (index == Definition::version.end())
            {
                throw Exception(fmt::format("{}: {}", Language::get("popcap.animation.invalid_version"), version), std::source_location::current(), "process");
            }
            json.version = version;
            json.frame_rate = sen->readUint8();
            json.position = AnimationPosition{(sen->readUint16() / 20), (sen->readUint16() / 20)};
            json.size = AnimationSize{(sen->readUint16() / 20), (sen->readUint16() / 20)};
            auto image_count = sen->readUint16();
            for (auto i : Range(image_count))
            {
                read_image();
            }
            auto sprite_count = sen->readUint16();
            for (auto i : Range(sprite_count))
            {
                auto sprite = AnimationSprite{};
                read_sprite(sprite, i);
            }
            if (version <= 3 || sen->readBoolean()) {
                read_sprite(json.main_sprite);
            }
            return;
        }

        explicit Decode(
            DataStreamView & it
        ) : sen(&it)
        {
        }

        ~Decode(

        ) = default;

        inline static auto decode_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto c = Decode<T>{source};
            c.process();
            FileSystem::write_json(destination, c.json);
            return;
        }
    };

}