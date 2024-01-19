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

        std::map<std::string_view, AnimationSprite> mutable sprite_map;

        int mutable version;

        inline auto animation_decode() const -> SexyAnimation
        {
            if (sen.readUint32() != Definition::magic)
            {
                throw Exception("invaild_magic");
            }
            version = sen.readUint32();
            auto index = std::find(Definition::version.begin(), Definition::version.end(), version);
            if (index == Definition::version.end())
            {
                throw Exception("invaild_version");
            }
            auto json = SexyAnimation{version, sen.readUint8()};
            json.position = AnimationPosition{(sen.readUint16() / 20), (sen.readUint16() / 20)};
            json.size = AnimationSize{(sen.readUint16() / 20), (sen.readUint16() / 20)};
            auto image_count = sen.readUint16();
            for (auto i : Range(image_count))
            {
                read_image(json);
            }
            auto sprite_count = sen.readUint16();
            for (auto i : Range(sprite_count))
            {
                read_sprite(i);
            }
            json.sprite = sprite_map;
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
                frame.push_back(read_frame_info());
            }
            if (index == -1) {
                return sprite;
            }
            else {
                sprite_map.insert({sprite_name, sprite});
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
                    frame_info.remove.push_back(read_remove());
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
                    frame_info.append.push_back(read_append());
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
                    frame_info.change.push_back(read_move());
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
                    frame_info.command.push_back(read_command());
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
                append_info.time_scale = sen.readInt32() / 65536;
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
            if ((flag & MoveFlags::matrix) != 0)
            {
                change_info.transform = std::vector<double>{0, 0, 0, 0, 0, 0};
                change_info.transform[0] = sen.readInt32() / 65536;
                change_info.transform[2] = sen.readInt32() / 65536;
                change_info.transform[1] = sen.readInt32() / 65536;
                change_info.transform[3] = sen.readInt32() / 65536;
            }
            else if ((flag & MoveFlags::rotate) != 0)
            {
                change_info.transform = std::vector<double>{0, 0, 0};
                change_info.transform[0] = sen.readInt16() / 1000;
            }
            if ((flag & MoveFlags::long_coords) != 0)
            {
                change_info.transform[change_info.transform.size() - 2] = sen.readInt32() / 20;
                change_info.transform[change_info.transform.size() - 1] = sen.readInt32() / 20;
            }
            else
            {
                change_info.transform[change_info.transform.size() - 2] = sen.readInt16() / 20;
                change_info.transform[change_info.transform.size() - 1] = sen.readInt16() / 20;
            }
            if ((flag & MoveFlags::src_react) != 0) {
                change_info.source_rectangle[0] = sen.readInt16() / 20;
                change_info.source_rectangle[1] = sen.readInt16() / 20;
                change_info.source_rectangle[2] = sen.readInt16() / 20;
                change_info.source_rectangle[3] = sen.readInt16() / 20;
            }
            if ((flag & MoveFlags::color) != 0) {
                change_info.color[0] = sen.readUint8() / 255;
                change_info.color[1] = sen.readUint8() / 255;
                change_info.color[2] = sen.readUint8() / 255;
                change_info.color[3] = sen.readUint8() / 255;
            }
            if ((flag & MoveFlags::sprite_frame_number) != 0) {
                change_info.sprite_frame_number = sen.readUint16();
            }
            return change_info;
        }

        inline auto read_image(SexyAnimation &json) const -> void
        {
            auto image_name = String{sen.readStringByUint16()};
            auto name_list = image_name.split("|");
            if (json.image.contains(name_list[1]))
            {
                throw Exception("duplicate_image");
            }
            auto image = AnimationImage{};
            if (version >= 4)
            {
                image.size = AnimationSize{sen.readUint16(), sen.readUint16()};
            }
            image.name = std::string_view{name_list[0]};
            if (version == 1)
            {
                auto matrix = sen.readInt16() / 1000;
                image.transform = std::vector<double>{
                    std::cos(matrix),
                    -std::sin(matrix),
                    std::sin(matrix),
                    std::cos(matrix),
                    (double)(sen.readInt16() / 20),
                    (double)(sen.readInt16() / 20)};
            }
            else
            {
                image.transform = std::vector<double>{
                    (double)(sen.readInt32() / 1310720),
                    (double)(sen.readInt32() / 1310720),
                    (double)(sen.readInt32() / 1310720),
                    (double)(sen.readInt32() / 1310720),
                    (double)(sen.readInt16() / 20),
                    (double)(sen.readInt16() / 20)};
            }
            json.image.insert({std::string_view{name_list[1]}, image});
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
            // Write_json;
            return;
        }
    };

}