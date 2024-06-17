#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/common.hpp"

namespace Sen::Kernel::Support::PopCap::Animation
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Decode : Common
    {
    protected:

        inline static auto exchange_image(
            DataStreamView &stream,
            AnimationImage &value) -> void
        {
            auto name = String{stream.readStringByUint16()}.split(vertical_bar);
            value.name = name[0];
            value.id = name[1];
            if (k_version >= 4_ui)
            {
                value.size.width = stream.readInt16();
                value.size.height = stream.readInt16();
            }
            if (k_version == 1_ui)
            {
                value.transform.resize(3_size);
                exchange_floater_with_rate<int16_t, ValueRate::angle>(stream, value.transform[0]);
            }
            else
            {
                value.transform.resize(6_size);
                exchange_floater_with_rate<int32_t, ValueRate::matrix_exact>(stream, value.transform[0]);
                exchange_floater_with_rate<int32_t, ValueRate::matrix_exact>(stream, value.transform[1]);
                exchange_floater_with_rate<int32_t, ValueRate::matrix_exact>(stream, value.transform[2]);
                exchange_floater_with_rate<int32_t, ValueRate::matrix_exact>(stream, value.transform[3]);
            }
            exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.transform[value.transform.size() - 2_size]);
            exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.transform[value.transform.size() - 1_size]);
            return;
        }

        template <typename RawShortValue, typename RawLongValue, auto flag_count>
            requires std::is_arithmetic_v<RawShortValue> && std::is_arithmetic_v<RawLongValue> 
        inline static auto exchange_integer_variant_with_flag(
            DataStreamView &stream,
            int &value,
            std::bitset<flag_count> & flag) -> void
        {
            auto value_short_bit_count = static_cast<size_t>(sizeof(RawShortValue) * size_t{8} - static_cast<size_t>(flag_count));
            auto value_short_maximum = static_cast<RawShortValue>((std::numeric_limits<RawShortValue>::max)() >> flag_count); 
            auto value_short_with_flag = stream.read_of<RawShortValue>();
            auto value_short = clip_bit(value_short_with_flag, k_begin_index, value_short_bit_count);
            auto value_flag = clip_bit(value_short_with_flag, value_short_bit_count, static_cast<size_t>(flag_count));
            auto value_long = RawLongValue{};
            if (value_short != value_short_maximum)
            {
                value_long = static_cast<RawLongValue>(value_short);
            }
            else
            {
                value_long = static_cast<RawLongValue>(stream.read_of<RawLongValue>());
            }
            value = static_cast<int>(value_long);
            flag = std::bitset<flag_count>(static_cast<uint64_t>(value_flag));
            return;
        }

        template <typename RawShortValue, typename RawLongValue>
            requires std::is_arithmetic_v<RawShortValue> && std::is_arithmetic_v<RawLongValue>
        inline static auto exchange_integer_variant(
            DataStreamView &stream,
            int &value) -> void
        {
            auto value_short_maximum = static_cast<RawShortValue>((std::numeric_limits<RawShortValue>::max)());
            auto value_short = stream.read_of<RawShortValue>();
            auto value_long = RawLongValue{};
            if (value_short != value_short_maximum)
            {
                value_long = static_cast<RawLongValue>(value_short);
            }
            else
            {
                value_long = static_cast<RawLongValue>(stream.read_of<RawLongValue>());
            }
            value = static_cast<int>(value_long);
            return;
        }

        inline static auto exchange_layer_remove(
            DataStreamView &stream,
            int &value) -> void
        {
            auto flag = std::bitset<LayerRemoveFlag::k_count>{};
            exchange_integer_variant_with_flag<uint16_t, uint32_t>(stream, value, flag);
            return;
        }

        inline static auto exchange_layer_append(
            DataStreamView &stream,
            AnimationAppend &value) -> void
        {
            auto flag = std::bitset<LayerAppendFlag::k_count>{};
            exchange_integer_variant_with_flag<uint16_t, uint32_t>(stream, value.index, flag);
            value.resource = static_cast<uint16_t>(stream.readUint8());
            if (k_version >= 6_ui && value.resource == static_cast<uint16_t>((std::numeric_limits<uint8_t>::max)()))
            {
                value.resource = stream.readUint16();
            }
            if (flag.test(LayerAppendFlag::sprite))
            {
                value.sprite = true;
            }
            else
            {
                value.sprite = false;
            }
            if (flag.test(LayerAppendFlag::additive))
            {
                value.additive = true;
            }
            else
            {
                value.additive = false;
            }
            if (flag.test(LayerAppendFlag::preload_frame))
            {
                value.preload_frame = stream.readInt16();
            }
            else
            {
                value.preload_frame = static_cast<int16>(0);
            }
            if (flag.test(LayerAppendFlag::name))
            {
                value.name = stream.readStringByUint16();
            }
            if (flag.test(LayerAppendFlag::time_scale))
            {
                exchange_floater_with_rate<int, ValueRate::time>(stream, value.time_scale);
            }
            else
            {
                value.time_scale = static_cast<double>(1.0);
            }
            return;
        }

        inline static auto exchange_layer_change(
            DataStreamView &stream,
            AnimationChange &value) -> void
        {
            auto flag = std::bitset<LayerChangeFlag::k_count>{};
            exchange_integer_variant_with_flag<uint16_t, uint32_t>(stream, value.index, flag);
            if (!flag.test(LayerChangeFlag::rotate) && !flag.test(LayerChangeFlag::matrix))
            {
                value.transform.resize(2_size);
            }
            if (flag.test(LayerChangeFlag::rotate))
            {
                assert(!flag.test(LayerChangeFlag::matrix));
                value.transform.resize(3);
                exchange_floater_with_rate<int16_t, ValueRate::angle>(stream, value.transform[0]);
            }
            if (flag.test(LayerChangeFlag::matrix))
            {
                assert(!flag.test(LayerChangeFlag::rotate));
                value.transform.resize(6_size);
                exchange_floater_with_rate<int32_t, ValueRate::matrix>(stream, value.transform[0]);
                exchange_floater_with_rate<int32_t, ValueRate::matrix>(stream, value.transform[2]);
                exchange_floater_with_rate<int32_t, ValueRate::matrix>(stream, value.transform[1]);
                exchange_floater_with_rate<int32_t, ValueRate::matrix>(stream, value.transform[3]);
            }
            if (flag.test(LayerChangeFlag::long_coord))
            {
                exchange_floater_with_rate<int32_t, ValueRate::size>(stream, value.transform[value.transform.size() - 2_size]);
                exchange_floater_with_rate<int32_t, ValueRate::size>(stream, value.transform[value.transform.size() - 1_size]);
            }
            else
            {
                exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.transform[value.transform.size() - 2_size]);
                exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.transform[value.transform.size() - 1_size]);
            }
            if (flag.test(LayerChangeFlag::source_rectangle))
            {
                exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.source_rectangle[0]);
                exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.source_rectangle[1]);
                exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.source_rectangle[2]);
                exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.source_rectangle[3]);
            }
            else
            {
                value.source_rectangle = std::array<double, 4>{0.0, 0.0, 0.0, 0.0};
            }
            if (flag.test(LayerChangeFlag::color))
            {
                exchange_floater_with_rate<uint8_t, ValueRate::color>(stream, value.color[0]);
                exchange_floater_with_rate<uint8_t, ValueRate::color>(stream, value.color[1]);
                exchange_floater_with_rate<uint8_t, ValueRate::color>(stream, value.color[2]);
                exchange_floater_with_rate<uint8_t, ValueRate::color>(stream, value.color[3]);
            }
            else
            {
                value.color = std::array<double, 4>{0.0, 0.0, 0.0, 0.0};
            }
            if (flag.test(LayerChangeFlag::sprite_frame_number))
            {
                value.sprite_frame_number = stream.readInt16();
            }
            else
            {
                value.sprite_frame_number = static_cast<int16_t>(0);
            }
            return;
        }

        inline static auto exchange_command(
            DataStreamView &stream,
            AnimationCommand &value) -> void
        {
            value.command = stream.readStringByUint16();
            value.argument = stream.readStringByUint16();
            return;
        }

        inline static auto exchange_frame(
            DataStreamView &stream,
            AnimationFrame &value) -> void
        {
            auto flag = std::bitset<FrameFlag::k_count>(static_cast<uint64_t>(stream.readUint8()));
            if (flag.test(FrameFlag::remove))
            {
                auto size = static_cast<int32_t>(0);
                exchange_integer_variant<uint8_t, uint16_t>(stream, size);
                exchange_list(stream, value.remove, &exchange_layer_remove, static_cast<std::size_t>(size));
            }
            if (flag.test(FrameFlag::append))
            {
                auto size = static_cast<int32_t>(0);
                exchange_integer_variant<uint8_t, uint16_t>(stream, size);
                exchange_list(stream, value.append, &exchange_layer_append, static_cast<std::size_t>(size));
            }
            if (flag.test(FrameFlag::change))
            {
                auto size = static_cast<int32_t>(0);
                exchange_integer_variant<uint8_t, uint16_t>(stream, size);
                exchange_list(stream, value.change, &exchange_layer_change, static_cast<std::size_t>(size));
            }
            if (flag.test(FrameFlag::label))
            {
                value.label = stream.readStringByUint16();
            }
            if (flag.test(FrameFlag::stop))
            {
                value.stop = true;
            }
            else
            {
                value.stop = false;
            }
            if (flag.test(FrameFlag::command))
            {
                exchange_list(stream, value.command, &exchange_command, static_cast<std::size_t>(stream.readUint8()));
            }
            return;
        }

        inline static auto exchange_sprite(
            DataStreamView &stream,
            AnimationSprite &value) -> void
        {
            if (k_version >= 4_ui)
            {
                value.name = stream.readStringByUint16();
                if (k_version >= 6_ui)
                {
                    stream.readStringByUint16();
                }
                stream.readInt32();
            }
            value.frame.resize(static_cast<size_t>(stream.readUint16()));
            if (k_version >= 5_ui)
            {
                value.work_area.start = stream.readInt16();
                value.work_area.duration = stream.readInt16();
            }
            exchange_list(stream, value.frame, &exchange_frame);
            return;
        }

        inline static auto exchange_animation(
            DataStreamView &stream,
            SexyAnimation &value) -> void
        {
            value.frame_rate = stream.readUint8();
            exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.position.x);
            exchange_floater_with_rate<int16_t, ValueRate::size>(stream, value.position.y);
            exchange_floater_with_rate<uint16_t, ValueRate::size>(stream, value.size.width);
            exchange_floater_with_rate<uint16_t, ValueRate::size>(stream, value.size.height);
            exchange_list(stream, value.image, &exchange_image, static_cast<std::size_t>(stream.readUint16()));
            exchange_list(stream, value.sprite, &exchange_sprite, static_cast<std::size_t>(stream.readUint16()));
            if (k_version < 4_ui || stream.readBoolean())
            {
                exchange_sprite(stream, value.main_sprite);
            }
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            SexyAnimation &value) -> void
        {
            assert_conditional((stream.readUint32() == k_magic_identifier), fmt::format("{}", Language::get("popcap.animation.invalid_magic")), "process_whole");
            auto version = stream.readUint32();
            auto index = std::find(version_list.begin(), version_list.end(), version);
            assert_conditional((index != version_list.end()), fmt::format("{}", Language::get("popcap.animation.invalid_version")), "process_whole");
            k_version = version;
            value.version = version;
            exchange_animation(stream, value);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto animation = SexyAnimation{};
            auto stream = DataStreamView{source};
            process_whole(stream, animation);
            FileSystem::write_json(destination, animation);
            return;
        }
    };

}