#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;
    
    struct Encode : Common
    {
    protected:

        template <typename RawShortValue, typename RawLongValue, auto flag_count> requires std::is_arithmetic_v<RawShortValue> && std::is_arithmetic_v<RawLongValue>
        inline static auto exchange_integer_variant_with_flag(
            DataStreamView & stream,
            int const & value,
            std::bitset<flag_count> & flag
        ) -> void
        {
            auto value_short_bit_count = static_cast<size_t>(sizeof(RawShortValue) * size_t{8} - static_cast<size_t>(flag_count));
            auto value_short_maximum = static_cast<RawShortValue>((std::numeric_limits<RawShortValue>::max)() >> flag_count); 
            auto value_long = static_cast<RawLongValue>(value);
            auto value_flag = static_cast<RawShortValue>(flag.to_ullong());
            auto value_short_with_flag = value_flag << value_short_bit_count;
            if (value_long < static_cast<RawLongValue>(value_short_maximum)) {
                value_short_with_flag |= static_cast<RawShortValue>(value_long);
                stream.write_of<RawShortValue>(value_short_with_flag);
            }
            else {
                value_short_with_flag |= value_short_maximum;
                stream.write_of<RawShortValue>(value_short_with_flag);
                stream.write_of<RawLongValue>(value_long);
            }
            return;
        }

        template <typename RawShortValue, typename RawLongValue> requires std::is_arithmetic_v<RawShortValue> && std::is_arithmetic_v<RawLongValue>
        inline static auto exchange_integer_variant(
            DataStreamView & stream,
            int const & value
        ) -> void
        {
            auto value_short_maximum = static_cast<RawShortValue>((std::numeric_limits<RawShortValue>::max)());
            auto value_long = static_cast<RawLongValue>(value);
            if (value_long < static_cast<RawLongValue>(value_short_maximum)) {
                stream.write_of<RawShortValue>(static_cast<RawShortValue>(value_long));
            }
            else {
                stream.write_of<RawShortValue>(value_short_maximum);
                stream.write_of<RawLongValue>(value_long);
            }
            return;
        }

        inline static auto exchange_image(
            DataStreamView &stream,
            AnimationImage const &value
        ) -> void
        {
            stream.writeStringByUint16(String::join(std::vector<std::string>{ value.name, value.id}, vertical_bar));
            if (k_version >= 4_ui) {
                stream.writeInt16(value.size.width);
                stream.writeInt16(value.size.height);
            }
            if (k_version == 1_ui) {
                exchange_floater_with_rate<int16_t, ValueRate::angle>(value.transform[0], stream);
            }
            if (k_version >= 2_ui) {
                exchange_floater_with_rate<int32_t, ValueRate::matrix_exact>(value.transform[0], stream);
                exchange_floater_with_rate<int32_t, ValueRate::matrix_exact>(value.transform[1], stream);
                exchange_floater_with_rate<int32_t, ValueRate::matrix_exact>(value.transform[2], stream);
                exchange_floater_with_rate<int32_t, ValueRate::matrix_exact>(value.transform[3], stream);
            }
            exchange_floater_with_rate<int16_t, ValueRate::size>(value.transform[value.transform.size() - 2_size], stream);
            exchange_floater_with_rate<int16_t, ValueRate::size>(value.transform[value.transform.size() - 1_size], stream);
            return;
        }

        inline static auto exchange_layer_remove(
            DataStreamView &stream,
            int const &value
        ) -> void
        {
            auto flag = std::bitset<LayerRemoveFlag::k_count>{};
            exchange_integer_variant_with_flag<uint16_t, uint32_t>(stream, value, flag);
            return;
        }

        inline static auto exchange_layer_append(
            DataStreamView &stream,
            AnimationAppend const & value
        ) -> void
        {
            auto flag = std::bitset<LayerAppendFlag::k_count>{};
            if (value.time_scale != static_cast<double>(1.0)) {
                flag.set(LayerAppendFlag::time_scale);
            }
            if (!value.name.empty()) {
                flag.set(LayerAppendFlag::name);
            }
            if (value.preload_frame != static_cast<int16_t>(0)) {
                flag.set(LayerAppendFlag::preload_frame);
            }
            if (value.additive) {
                flag.set(LayerAppendFlag::additive);
            }
            if (value.sprite) {
                flag.set(LayerAppendFlag::sprite);
            }
            exchange_integer_variant_with_flag<uint16_t, uint32_t>(stream, value.index, flag);
            if (k_version >= 6_ui) {
                exchange_integer_variant<uint8_t, uint16_t>(stream, value.resource);
            }
            else {
                stream.writeUint8(value.resource);
            }
            if (value.preload_frame != static_cast<int16_t>(0)) {
                stream.writeInt16(value.preload_frame);
            }
            if (!value.name.empty()) {
                stream.writeStringByUint16(value.name);
            }
            if (value.time_scale != 1.0) {
                exchange_floater_with_rate<int, ValueRate::time>(value.time_scale, stream);
            }
            return;
        }

        inline static auto exchange_layer_change(
            DataStreamView &stream,
            AnimationChange const & value
        ) -> void
        {
            auto flag = std::bitset<LayerChangeFlag::k_count>{};
            if (value.sprite_frame_number != static_cast<int16_t>(0)) {
                flag.set(LayerChangeFlag::sprite_frame_number);
            }
            if (!(value.color.at(0) == 0.0 && value.color.at(1) == 0.0 && value.color.at(2) == 0.0 && value.color.at(3) == 0.0)) {
                flag.set(LayerChangeFlag::color);
            }
            if (!(value.source_rectangle.at(0) == 0.0 && value.source_rectangle.at(1) == 0.0 && value.source_rectangle.at(2) == 0.0 && value.source_rectangle.at(3) == 0.0)) {
                flag.set(LayerChangeFlag::source_rectangle);
            }
            if (value.transform.size() == 3_size) {
                flag.set(LayerChangeFlag::rotate);
            }
            if (value.transform.size() == 6_size) {
                flag.set(LayerChangeFlag::matrix);
            }
            flag.set(LayerChangeFlag::long_coord);
            exchange_integer_variant_with_flag<uint16_t, uint32_t>(stream, value.index, flag);
            if (flag.test(LayerChangeFlag::rotate)) {
                exchange_floater_with_rate<int16_t, ValueRate::angle>(value.transform[0], stream);
            }
            if (flag.test(LayerChangeFlag::matrix)) {
                exchange_floater_with_rate<int32_t, ValueRate::matrix>(value.transform[0], stream);
                exchange_floater_with_rate<int32_t, ValueRate::matrix>(value.transform[2], stream);
                exchange_floater_with_rate<int32_t, ValueRate::matrix>(value.transform[1], stream);
                exchange_floater_with_rate<int32_t, ValueRate::matrix>(value.transform[3], stream);
            }
            exchange_floater_with_rate<int32_t, ValueRate::size>(value.transform[value.transform.size() - 2_size], stream);
            exchange_floater_with_rate<int32_t, ValueRate::size>(value.transform[value.transform.size() - 1_size], stream);
            if (flag.test(LayerChangeFlag::source_rectangle)) {
                exchange_floater_with_rate<int16_t, ValueRate::size>(value.source_rectangle[0], stream);
                exchange_floater_with_rate<int16_t, ValueRate::size>(value.source_rectangle[1], stream);
                exchange_floater_with_rate<int16_t, ValueRate::size>(value.source_rectangle[2], stream);
                exchange_floater_with_rate<int16_t, ValueRate::size>(value.source_rectangle[3], stream);
            }
            if (flag.test(LayerChangeFlag::color)) {
                exchange_floater_with_rate<uint8_t, ValueRate::color>(value.color[0], stream);
                exchange_floater_with_rate<uint8_t, ValueRate::color>(value.color[1], stream);
                exchange_floater_with_rate<uint8_t, ValueRate::color>(value.color[2], stream);
                exchange_floater_with_rate<uint8_t, ValueRate::color>(value.color[3], stream);
            }
            if (flag.test(LayerChangeFlag::sprite_frame_number)) {
                stream.writeInt16(value.sprite_frame_number);
            }
            return;
        }

        inline static auto exchange_layer_command(
            DataStreamView &stream,
            AnimationCommand const &value 
        ) -> void
        {
            stream.writeStringByUint16(value.command);
            stream.writeStringByUint16(value.argument);
            return;
        }

        inline static auto exchange_frame(
            DataStreamView &stream,
            AnimationFrame const &value 
        ) -> void
        {
            auto flag = std::bitset<FrameFlag::k_count>{};
            if (!value.remove.empty()) {
                flag.set(FrameFlag::remove);
            }
            if (!value.append.empty()) {
                flag.set(FrameFlag::append);
            }
            if (!value.change.empty()) {
                flag.set(FrameFlag::change);
            }
            if (!value.label.empty()) {
                flag.set(FrameFlag::label);
            }
            if (value.stop) {
                flag.set(FrameFlag::stop);
            }
            if (!value.command.empty()) {
                flag.set(FrameFlag::command);
            }
            stream.writeUint8(static_cast<uint8_t>(flag.to_ullong()));
            if (flag.test(FrameFlag::remove)) {
                exchange_integer_variant<uint8_t, uint16_t>(stream, value.remove.size());
                exchange_list<false, uint16_t>(stream, value.remove, &exchange_layer_remove);
            }
            if (flag.test(FrameFlag::append)) {
                exchange_integer_variant<uint8_t, uint16_t>(stream, value.append.size());
                exchange_list<false, uint16_t>(stream, value.append, &exchange_layer_append);
            }
            if (flag.test(FrameFlag::change)) {
                exchange_integer_variant<uint8_t, uint16_t>(stream, value.change.size());
                exchange_list<false, uint16_t>(stream, value.change, &exchange_layer_change);
            }
            if (flag.test(FrameFlag::label)) {
                stream.writeStringByUint16(value.label);
            }
            if (flag.test(FrameFlag::command)) {
                stream.writeUint8(value.command.size());
                exchange_list<false, uint16_t>(stream, value.command, &exchange_layer_command);
            }
            return;
        }   

        inline static auto exchange_sprite(
            DataStreamView &stream,
            AnimationSprite const &value
        ) -> void
        {
            if (k_version >= 4_size) {
                stream.writeStringByUint16(value.name);
                if (k_version >= 6_size) {
                    stream.writeNull(2_size);
                }
                exchange_floater_with_rate<int32_t, ValueRate::time>(k_frame_rate, stream);
            }
            stream.writeUint16(static_cast<uint16_t>(value.frame.size()));
            if (k_version >= 5_size) {
                stream.writeInt16(value.work_area.start);
                stream.writeInt16(value.work_area.duration);
            }
            exchange_list<false, uint16_t>(stream, value.frame, &exchange_frame);
            return;
        }

        inline static auto exchange_animation(
            DataStreamView &stream,
            SexyAnimation const &value
        ) -> void
        {
            stream.writeUint8(value.frame_rate);
            k_frame_rate = static_cast<double>(value.frame_rate);
            exchange_floater_with_rate<int16_t, ValueRate::size>(value.position.x, stream);
            exchange_floater_with_rate<int16_t, ValueRate::size>(value.position.y, stream);
            exchange_floater_with_rate<int16_t, ValueRate::size>(value.size.width, stream);
            exchange_floater_with_rate<int16_t, ValueRate::size>(value.size.height, stream);
            exchange_list<true, uint16_t>(stream, value.image, &exchange_image);
            exchange_list<true, uint16_t>(stream, value.sprite, &exchange_sprite);
            if (k_version >= 4_size) {
                stream.writeBoolean(true);
            }
            exchange_sprite(stream, value.main_sprite);
            return;
        }

    public:

        inline static auto process_whole(
            DataStreamView &stream,
            SexyAnimation const &definition
        ) -> void
        {
            stream.writeUint32(k_magic_identifier);
            stream.writeUint32(static_cast<uint32_t>(definition.version));
            k_version = static_cast<uint32_t>(definition.version);
            exchange_animation(stream, definition);
            return;
        }

        inline static auto proces_fs(
            std::string_view source,
            std::string_view destination
            ) -> void
        {
            auto animation = *FileSystem::read_json(source);
            auto stream = DataStreamView{};
            process_whole(stream, animation);
            stream.out_file(destination);
            return;
        }
    };

}