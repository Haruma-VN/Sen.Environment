#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation
{
    using namespace Definition;
    class Encode
    {

    protected:
        DataStreamView sen;
        SexyAnimation json;
        int mutable version;
        int mutable frame_rate;
        inline auto animation_encode() const -> DataStreamView
        {
            version = json.version;
            frame_rate = json.frame_rate;
            sen.writeUint32(Definition::magic);
            auto index = std::find(Definition::version.begin(), Definition::version.end(), version);
            if (index == Definition::version.end())
            {
                throw Exception(fmt::format("{}: {}", Language::get("popcap.animation.invalid_version"), version), std::source_location::current(), "animation_encode");
            }
            sen.writeUint32(version);
            sen.writeUint8(frame_rate);
            sen.writeUint16(json.position.x * 20);
            sen.writeUint16(json.position.y * 20);
            if (json.size.width != -1 and json.size.height != -1)
            {
                sen.writeUint16(json.size.width * 20);
                sen.writeUint16(json.size.height * 20);
            }
            else
            {
                sen.writeUint16(-1);
                sen.writeUint16(-1);
            }
            auto image = json.image;
            sen.writeUint16(image.size());
            for (auto & [key, value] : image.items())
            {
                auto image_name = fmt::format("{}|{}", value['name'], key);
                sen.writeStringByUint16(image_name);
                write_image(value);
            }
            auto sprite = json.sprite;
            sen.writeUint16(sprite.size());
            for (auto & [key, value] : sprite.items()) {
                if (version >= 4) {
                    sen.writeStringByUint16(key);
                    write_sprite(value);
                }
            }
            if (version <= 3) {
                write_sprite(json.main_sprite); 
            }
            else {
                sen.writeBoolean(true);
                sen.writeStringByUint16("");
                write_sprite(json.main_sprite);
            }
            return sen;
        }

        inline auto write_sprite(const AnimationSprite & sprite) const -> void {
            if (version >= 4) {
                if (version >= 6) {
                    sen.writeStringByUint16(sprite.description);
                }
                sen.writeUint32(frame_rate * 65536);
            }
            auto work_area = sprite.work_area;
            if (version >= 5) {
                sen.writeUint16(work_area.duration);
                sen.writeUint16(work_area.index);
                sen.writeUint16(work_area.index + work_area.duration - 1);
            }
            else {
                sen.writeUint16(work_area.duration);
            }
            for (auto frame : sprite.frame) {
                write_frame_info(frame);
            }
            return;
        }

        inline auto write_frame_info(const AnimationFrame & frame) const -> void {
            auto flag = 0;
            auto write_pos = sen.write_pos;
            sen.writeUint8(0xff);
            auto count = 0;
            auto remove = frame.remove;
            if (remove.size() > 0) {
                flag |= FrameFlags::remove;
                count = remove.size();
                if ((count < 255) and (count >= 0)) {
                    sen.writeUint8(count);
                }
                else {
                    sen.writeUint8(255);
                    sen.writeUint16(count);
                }
                for (auto i : Range(count)) {
                    write_remove(remove[i]);
                }
            }
            auto append = frame.append;
            if (append.size() > 0) {
                flag |= FrameFlags::append;
                count = append.size();
                if ((count < 255) and (count >= 0)) {
                    sen.writeUint8(count);
                }
                else {
                    sen.writeUint8(255);
                    sen.writeUint16(count);
                }
                for (auto i : Range(count)) {
                    write_append(append[i]);
                }
            }
            auto change = frame.change;
            if (change.size() > 0) {
                flag |= FrameFlags::change;
                count = change.size();
                if ((count < 255) and (count >= 0)) {
                    sen.writeUint8(count);
                }
                else {
                    sen.writeUint8(255);
                    sen.writeUint16(count);
                }
                for (auto i : Range(count)) {
                    write_move(change[i]);
                }
            }
            if (!frame.label.empty() || frame.label != "") {
                flag |= FrameFlags::label;
                sen.writeStringByUint16(frame.label);
            }
            if (frame.stop) {
                flag |= FrameFlags::stop;
            }
            auto command = frame.command;
            if (command.size() > 0) {
                flag |= FrameFlags::command;
                count = command.size();
                if ((count < 255) and (count >= 0)) {
                    sen.writeUint8(count);
                }
                else {
                    sen.writeUint8(255);
                    sen.writeUint16(count);
                }
                for (auto i : Range(count)) {
                    write_command(command[i]);
                }
            }
            auto thiz_pos = sen.write_pos;
            sen.writeUint8(flag, write_pos);
            sen.write_pos = thiz_pos;
            return;
        }

        inline auto write_remove(int index) const -> void {
            if (index >= 2047) {
                sen.writeUint16(2047);
                sen.writeUint32(index);
            }
            else {
                sen.writeUint16(index);
            }
            return;
        }

        inline auto write_append(const AnimationAppend & append) const -> void {
            auto write_pos = sen.write_pos;
            sen.writeUint16(0);
            auto flag = 0;
            auto index = append.index;
            if (index >= 2047 || index < 0) {
                flag |= 2047;
                sen.writeUint32(index);
            }
            else {
                flag |= index;
            }
            flag |= append.sprite ? 32768 : 0;
            flag |= append.additive ? 16384 : 0;
            auto resource = append.resource;
            if (version >= 6) {
                if (resource >= 255 || resource < 0) {
                    sen.writeUint8(255);
                    sen.writeUint16(resource);
                }
                else {
                    sen.writeUint8(resource);
                }
            }
            if (append.preload_frame != 0) {
                flag |= 8192;
                sen.writeUint16(append.preload_frame);
            }
            if (append.name != "" and append.name.empty()) {
                flag |= 4096;
                sen.writeStringByUint16(append.name);
            }
            if (append.time_scale != 1) {
                flag |= 2048;
                sen.writeUint32(append.time_scale * 65536);
            }
            auto thiz_pos = sen.write_pos;
            sen.writeUint16(flag, write_pos);
            sen.write_pos = thiz_pos;
            return;
        }

        inline auto write_move(
            const AnimationMove & move
        ) const -> void {
            auto write_pos = sen.write_pos;
            sen.writeUint16(0);
            auto flag = 0;
            auto index = move.index;
            if (index >= 1023 || index < 0) {
                flag |= 1023;
                sen.writeUint32(index);
            }
            else {
                flag |= index;
            }
            auto num_flag = 0;
            auto transform = move.transform;
            auto transform_size = transform.size();
            if (transform_size == 6) {
                num_flag = MoveFlags::matrix;
                sen.writeInt32(std::round(transform[0] * 65536.0));
                sen.writeInt32(std::round(transform[2] * 65536.0));
                sen.writeInt32(std::round(transform[1] * 65536.0));
                sen.writeInt32(std::round(transform[3] * 65536.0));
            } 
            else if (transform_size == 3) {
                num_flag |= MoveFlags::rotate;
                sen.writeInt16(std::round(transform[0] * 1000.0));
            }
            auto y = std::round(transform[transform_size - static_cast<size_t>(2)] * 20.0);
            auto x = std::round(transform[transform_size - static_cast<size_t>(1)] * 20.0);
            if (version >= 5) {
                num_flag |= MoveFlags::long_coords;
                sen.writeInt32(y);
                sen.writeInt32(x);
            }
            else {
                sen.writeInt16(y);
                sen.writeInt16(x);
            }
            auto source_rectangle = move.source_rectangle;
            if (!source_rectangle.empty() && source_rectangle.size() == 4) {
                num_flag |= MoveFlags::src_react;
                sen.writeInt16(source_rectangle[0] * 20);
                sen.writeInt16(source_rectangle[1] * 20);
                sen.writeInt16(source_rectangle[2] * 20);
                sen.writeInt16(source_rectangle[3] * 20);
            }
            auto color = move.color;
            if (!color.empty() && color.size() == 4) {
                num_flag |= MoveFlags::color;
                sen.writeUint8(color[0] * 255);
                sen.writeUint8(color[1] * 255);
                sen.writeUint8(color[2] * 255);
                sen.writeUint8(color[3] * 255);
            }
            if (move.sprite_frame_number != 0) {
                num_flag |= MoveFlags::sprite_frame_number;
                sen.writeUint8(move.sprite_frame_number);
            };
            flag |= num_flag;
            auto thiz_pos = sen.write_pos;
            sen.writeUint16(flag, write_pos);
            sen.write_pos = thiz_pos;
            return;
        }

        inline auto write_command(const AnimationCommand & command) const -> void {
            sen.writeStringByUint16(command.parameter);
            sen.writeStringByUint16(command.command);
            return;
        }


        inline auto write_image(const AnimationImage & image) const -> void
        {
            if (version >= 4)
            {
                sen.writeUint16(image.size.width);
                sen.writeUint16(image.size.height);
            }
            else
            {
                sen.writeUint16(-1);
                sen.writeUint16(-1);
            }
            if (version == 1)
            {
                if (image.transform.size() < 2)
                {
                    sen.writeInt16(0);
                    sen.writeInt16(0);
                    sen.writeInt16(0);
                }
                else if (image.transform.size() >= 6)
                {
                    auto acos = std::round(std::acos(image.transform[0]));
                    if ((image.transform[1] * (version == 2 ? -1 : 1)) < 0)
                    {
                        acos = -acos;
                    }
                    sen.writeInt16(acos);
                    sen.writeInt16(std::round(image.transform[4] * 20.0));
                    sen.writeInt16(std::round(image.transform[5] * 20.0));
                }
                else if (image.transform.size() >= 4)
                {
                    auto acos = std::round(std::acos(image.transform[0]));
                    if ((image.transform[1] * (version == 2 ? -1 : 1)) < 0)
                    {
                        acos = -acos;
                    }
                    sen.writeInt16(acos);
                    sen.writeInt16(0);
                    sen.writeInt16(0);
                }
                else if (image.transform.size() >= 2)
                {
                    sen.writeInt16(0);
                    sen.writeInt16(std::round(image.transform[0] * 20.0));
                    sen.writeInt16(std::round(image.transform[1] * 20.0));
                }
            }
            else
            {
                if (image.transform.size() < 2)
                {
                    sen.writeInt32(1310720);
                    sen.writeInt32(0);
                    sen.writeInt32(0);
                    sen.writeInt32(1310720);
                    sen.writeInt16(0);
                    sen.writeInt16(0);
                }
                else if (image.transform.size() >= 6)
                {
                    sen.writeInt32(std::round(image.transform[0] * 1310720.0));
                    sen.writeInt32(std::round(image.transform[2] * 1310720.0));
                    sen.writeInt32(std::round(image.transform[1] * 1310720.0));
                    sen.writeInt32(std::round(image.transform[3] * 1310720.0));
                    sen.writeInt16(std::round(image.transform[4] * 20.0));
                    sen.writeInt16(std::round(image.transform[5] * 20.0));
                }
                else if (image.transform.size() >= 4)
                {
                    sen.writeInt32(std::round(image.transform[0] * 1310720.0));
                    sen.writeInt32(std::round(image.transform[2] * 1310720.0));
                    sen.writeInt32(std::round(image.transform[1] * 1310720.0));
                    sen.writeInt32(std::round(image.transform[3] * 1310720.0));
                    sen.writeInt16(0);
                    sen.writeInt16(0);
                }
                else if (image.transform.size() >= 2)
                {
                    sen.writeInt32(1310720);
                    sen.writeInt32(0);
                    sen.writeInt32(0);
                    sen.writeInt32(1310720);
                    sen.writeInt16(std::round(image.transform[0] * 20.0));
                    sen.writeInt16(std::round(image.transform[1] * 20.0));
                }
            }
            return;
        }

    public:
        explicit Encode(
            const SexyAnimation & json
        ) : json(json)
        {
        }

        ~Encode(

        ) = default;

        inline static auto encode_fs(
            std::string_view source,
            std::string_view destination
            ) -> void
        {
            auto json = FileSystem::read_json(source);
            auto c = std::unique_ptr<Encode>(new Encode{json});
            auto sen = c->animation_encode();
            sen.out_file(destination);
            return;
        }
    };

}