#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation
{
    using namespace Definition;
    class Decode
    {

    protected:
        DataStreamView sen;
        SexyAnimation mutable json{};
        int mutable version;

        inline auto animation_decode() const -> SexyAnimation
        {
            if (sen.readUint32() != Definition::magic)
            {
                throw Exception(fmt::format("{}", Language::get("popcap.animation.invalid_magic")), std::source_location::current(), "animation_decode");
            }
            version = sen.readUint32();
            auto index = std::find(Definition::version.begin(), Definition::version.end(), version);
            if (index == Definition::version.end())
            {
                throw Exception(fmt::format("{}: {}", Language::get("popcap.animation.invalid_version"), version), std::source_location::current(), "animation_decode");
            }
            json.version = version;
            json.frame_rate = sen.readUint8();
            json.position = AnimationPosition{(sen.readUint16() / 20), (sen.readUint16() / 20)};
            json.size = AnimationSize{(sen.readUint16() / 20), (sen.readUint16() / 20)};
            auto image_count = sen.readUint16();
            for (auto i : Range(image_count))
            {
                read_image();
            }
            auto sprite_count = sen.readUint16();
            for (auto i : Range(sprite_count))
            {
                read_sprite(i);
            }
            if (version <= 3|| sen.readBoolean()) {
                json.main_sprite = read_sprite(-1);
            }
            return json;
        }

        inline auto read_sprite(int index) const -> AnimationSprite
        {
            auto sprite_name = version >= 4 ? sen.readStringByUint16() : "*" + std::to_string(index);
            auto sprite = AnimationSprite{};
            if (version >= 4)
            {
                if (version >= 6)
                {
                    sprite.description = sen.readStringByUint16();
                }
                sen.readUint32();
            }
            // frame_rate;
            auto frame_count = sen.readUint16();
            if (version >= 5)
            {
                sprite.work_area = AnimationWorkArea{sen.readUint16(), sen.readUint16()};
                sprite.work_area.duration = frame_count;
            }
            else
            {
                sprite.work_area = AnimationWorkArea{0, frame_count};
            }
            std::vector<AnimationFrame> frame{};
            for (auto i : Range(frame_count))
            {
                frame.emplace_back(read_frame_info());
            }
            sprite.frame = frame;
            if (index == -1) {
                return sprite;
            }
            else {
                json.sprite.insert(std::pair{sprite_name, sprite});
            }   
            return sprite;
            
        }

        inline auto read_frame_info() const -> AnimationFrame
        {
            auto frame_info = AnimationFrame{};
            auto flag = sen.readUint8();
            auto count = 0;
            if ((flag & FrameFlags::remove) != 0)
            {
                count = sen.readUint8();
                if (count == 255)
                {
                    count = sen.readUint16();
                }
                for (auto i : Range(count))
                {
                    frame_info.remove.emplace_back(read_remove());
                }
            }
            if ((flag & FrameFlags::append) != 0)
            {
                count = sen.readUint8();
                if (count == 255)
                {
                    count = sen.readUint16();
                }
                for (auto i : Range(count))
                {
                    frame_info.append.emplace_back(read_append());
                }
            }
            if ((flag & FrameFlags::change) != 0)
            {
                count = sen.readUint8();
                if (count == 255)
                {
                    count = sen.readUint16();
                }
                for (auto i : Range(count))
                {
                    frame_info.change.emplace_back(read_move());
                }
            }
            if ((flag & FrameFlags::label) != 0) {
                frame_info.label = sen.readStringByUint16();
            }
            if ((flag & FrameFlags::stop) != 0) {
                frame_info.stop = true;
            }
            if ((flag & FrameFlags::command) != 0) {
                count = sen.readUint8();
                for (auto i : Range(count)) {
                    frame_info.command.emplace_back(read_command());
                }
            }
            return frame_info;
        }

        inline auto read_remove() const -> int
        {
            auto index = sen.readUint16();
            if (index >= 2047)
            {
                index = sen.readUint32();
            }
            return index;
        }

        inline auto read_command() const -> AnimationCommand
        {
            return AnimationCommand{
                sen.readStringByUint16(),
                sen.readStringByUint16()};
        }

        inline auto read_append() const -> AnimationAppend
        {
            auto append_info = AnimationAppend{};
            const auto value = sen.readUint16();
            auto index = value & 2047;
            if (index == 2047)
            {
                index = sen.readUint32();
            }
            append_info.index = index;
            append_info.sprite = (value & 32768) != 0;
            append_info.additive = (value & 16384) != 0;
            auto resource = sen.readUint8();
            if (version >= 6 and resource == 255)
            {
                resource = sen.readUint16();
            }
            append_info.resource = resource;
            if ((value & 8192) != 0)
            {
                append_info.preload_frame = sen.readUint16();
            }
            if ((value & 4096) != 0)
            {
                append_info.name = sen.readStringByUint16();
            }
            if ((value & 2048) != 0)
            {
                append_info.time_scale = sen.readInt32() / 65536.0;
            }
            return append_info;
        }

        inline auto read_move() const -> AnimationMove
        {
            auto change_info = AnimationMove{};
            const auto flag = sen.readUint16();
            auto index = flag & 1023;
            if (index == 1023)
            {
                index = sen.readUint32();
            }
            change_info.index = index;
            std::vector<double> transform{};
            if ((flag & MoveFlags::matrix) != 0)
            {
                
                auto t1 = sen.readInt32() / 65536.0;
                auto t3 = sen.readInt32() / 65536.0;
                auto t2 = sen.readInt32() / 65536.0;
                auto t4 = sen.readInt32() / 65536.0;
                transform.emplace_back(t1);
                transform.emplace_back(t2);
                transform.emplace_back(t3);
                transform.emplace_back(t4);
            }
            else if ((flag & MoveFlags::rotate) != 0)
            {
                transform.emplace_back(sen.readInt16() / 1000.0);
            }
            if ((flag & MoveFlags::long_coords) != 0)
            {
                auto y = sen.readInt32() / 20.0;
                auto x = sen.readInt32() / 20.0;
                transform.emplace_back(y);
                transform.emplace_back(x);
            }
            else
            {
                auto y = sen.readInt16() / 20.0;
                auto x = sen.readInt16() / 20.0;
                transform.emplace_back(y);
                transform.emplace_back(x);
            }
            change_info.transform = transform;
            if ((flag & MoveFlags::src_react) != 0) {
                change_info.source_rectangle.emplace_back(sen.readInt16() / 20.0);
                change_info.source_rectangle.emplace_back(sen.readInt16() / 20.0);
                change_info.source_rectangle.emplace_back(sen.readInt16() / 20.0);
                change_info.source_rectangle.emplace_back(sen.readInt16() / 20.0);
            }
            if ((flag & MoveFlags::color) != 0) {
                change_info.color.emplace_back(sen.readUint8() / 255.0);
                change_info.color.emplace_back(sen.readUint8() / 255.0);
                change_info.color.emplace_back(sen.readUint8() / 255.0);
                change_info.color.emplace_back(sen.readUint8() / 255.0);
            }
            if ((flag & MoveFlags::sprite_frame_number) != 0) {
                change_info.sprite_frame_number = sen.readUint16();
            }
            return change_info;
        }

        inline auto read_image() const -> void
        {
            auto image_name = String{sen.readStringByUint16()};
            auto name_list = image_name.split("|");
            if (json.image.contains(name_list[1]))
            {
                throw Exception(fmt::format("{}: {}", Language::get("popcap.animation.duplicate_image"), name_list[1]), std::source_location::current(), "read_image");
            }
            auto image = AnimationImage{};
            if (version >= 4)
            {
                image.size = AnimationSize{sen.readUint16(), sen.readUint16()};
            }
            image.name = std::string{name_list[0]};
            if (version == 1)
            {
                auto matrix = sen.readInt16() / 1000.0;
                image.transform = std::vector<double>{
                    Math::cos(matrix),
                    -Math::sin(matrix),
                    Math::sin(matrix),
                    Math::cos(matrix),
                    static_cast<double>(sen.readInt16() / 20.0),
                    static_cast<double>(sen.readInt16() / 20.0)};
            }
            else
            {
                image.transform = std::vector<double>{
                    static_cast<double>(sen.readInt32() / 1310720.0),
                    static_cast<double>(sen.readInt32() / 1310720.0),
                    static_cast<double>(sen.readInt32() / 1310720.0),
                    static_cast<double>(sen.readInt32() / 1310720.0),
                    static_cast<double>(sen.readInt16() / 20.0),
                    static_cast<double>(sen.readInt16() / 20.0)};
            }
            json.image.insert(std::pair{name_list[1], image});
            return;
        }

    public:
        explicit Decode(
            std::string_view source
        ) : sen(source)
        {
        }

        explicit Decode(
            DataStreamView & it
        ) : sen(it)
        {
        }

        ~Decode(

        ) = default;

        inline static auto decode_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto c = std::unique_ptr<Decode>(new Decode{source});
            auto json = c->animation_decode();
            FileSystem::write_json(destination, json);
            return;
        }
    };

}