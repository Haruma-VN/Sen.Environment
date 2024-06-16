#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{
    using XMLDocument = tinyxml2::XMLDocument;

    using Transform = std::array<double, 6>;

    using Color = std::array<double, 4>;

    struct Common
    {

        inline static auto k_version = 0;

        inline static auto k_media_scale_ratio = 0.0;

        inline static constexpr auto k_initial_transform = Transform{1.0, 0.0, 0.0, 1.0, 0.0, 0.0};

        inline static constexpr auto k_initial_color = Color{1.0, 1.0, 1.0, 1.0};

        inline static constexpr auto k_standard_resolution = 1200;

        inline static constexpr auto k_xfl_content = "PROXY-CS5"_sv;

        inline static constexpr auto k_xfl_version = "2.971"_sv;

        inline static constexpr auto k_xmlns_xsi_attribute = "http://www.w3.org/2001/XMLSchema-instance"_sv;

        inline static constexpr auto k_xmlns_attribute = "http://ns.adobe.com/xfl/2008/"_sv;

        inline static constexpr auto k_symbol_type = "graphic"_sv;

        inline static constexpr auto k_frame_resource_symbol_instance_unused = -1;

        inline static constexpr auto k_main_label_string = "main_label"_sv;

        inline static constexpr auto k_sprite_blank_name = "sprite"_sv;

        enum State : uint8_t
        {
            state_null,
            state_false,
            state_true
        };

        enum SpriteType : uint8_t 
        {
            sprite,
            label,
            main_sprite
        };

        struct Model
        {
            State state;
            int resource;
            bool sprite;
            Transform transform;
            Color color;
            int frame_start;
            int frame_duration;
        };

        struct FrameNode
        {
            int index;
            int duration;
            int resource = k_frame_resource_symbol_instance_unused;
            bool sprite;
            int first_frame;
            Transform transform;
            Color color;
        };

        using FrameNodeList = std::vector<FrameNode>;

        using FrameNodeStructure = std::map<int, FrameNodeList>;

        struct LabelInfo
        {
            int start;
            int duration;
            std::vector<int> layer_index_list;
            XMLDocument document;
        };

        struct PackageLibrary
        {
            std::map<std::string, XMLDocument> image;
            std::map<std::string, XMLDocument> sprite;
            tsl::ordered_map<std::string_view, LabelInfo> label;
            XMLDocument main_sprite;
            FrameNodeStructure frame_node;
        };

        struct FlashPackage
        {
            XMLDocument document;
            PackageLibrary library;
        };

        inline static auto exchange_simple_extra(
            typename SexyAnimation const &definition,
            typename ExtraInfo &extra) -> void
        {
            extra.version = static_cast<int>(definition.version);
            extra.position.x = static_cast<int>(definition.position.x);
            extra.position.y = static_cast<int>(definition.position.y);
            for (auto image_index : Range(definition.image.size()))
            {
                extra.image[fmt::format("image_{}", image_index + 1_size)] = ImageInfo{
                    .name = definition.image[image_index].name,
                    .id = definition.image[image_index].id,
                    .size = ImageDimension{
                        .width = static_cast<int>(definition.image[image_index].size.width),
                        .height = static_cast<int>(definition.image[image_index].size.height)}};
            }
            return;
        }

        template <auto point>
        inline static auto to_fixed(
            double number) -> std::string
        {
            static_assert(sizeof(point) == sizeof(int));
            auto stream = std::ostringstream{};
            stream << std::fixed << std::setprecision(static_cast<std::streamsize>(point)) << number;
            return stream.str();
        }

        inline static auto find_invalid_char(
            char data
        ) -> bool 
        {
            switch (data)
            {
            case ':':
                return true;
            case '*':
                return true;
            case '?':
                return true;
            case '<':
                return true;
            case '>':
                return true;
            case '|':
                return true;
            default:
                return false;
            }
        }

        inline static auto exchange_sprite_name_invalid(
            std::string &sprite_name
        ) -> bool {
            auto k_sprite_name_need_exchange = false;
            if (std::find_if(sprite_name.begin(), sprite_name.end(), &find_invalid_char) != sprite_name.end()) {
                sprite_name.erase(std::remove_if(sprite_name.begin(), sprite_name.end(), &find_invalid_char));
                k_sprite_name_need_exchange = true;
            }
            if (sprite_name.empty()) {
				sprite_name += k_sprite_blank_name;
                k_sprite_name_need_exchange = true;
            }
            return k_sprite_name_need_exchange;
        }

        inline static auto exchange_default_extra(
            typename ExtraInfo const &extra) -> void
        {
            k_version = extra.version;
            k_media_scale_ratio = static_cast<double>(k_standard_resolution) / static_cast<double>(extra.resolution);
            return;
        }

        inline static auto exchange_transform_from_variant_to_standard(
            std::vector<double> const &data,
            typename Transform &value) -> void
        {
            if (data.size() == 2_size)
            {
                value = Transform{
                    1.0, 0.0, 0.0, 1.0,
                    data[0], data[1]};
            }
            else if (data.size() == 3_size)
            {
                auto cos = std::cos(data[0]);
                auto sin = std::sin(data[0]);
                value = Transform{ cos, sin, -sin, cos, data[1], data[2]};
            }
            else if (data.size() == 6_size)
            {
                value = Transform{ data[0], data[1], data[2], data[3], data[4], data[5]};
            }
            else
            {
                assert_conditional(false, "invalid_transform_size", "exchange_transform_from_variant_to_standard");
            }
            return;
        }

        inline static auto exchange_tranform_from_rotate_to_standard(
            std::vector<double> const &data,
            typename Transform &value) -> void
        {
            try_assert(data.size() == 3_size, "invalid_animation_transform_length");
            auto cos = std::cos(data[0]);
            auto sin = std::sin(data[0]);
            value = Transform{ cos, sin, -sin, cos, data[1], data[2]};
            return;
        }

        inline static auto exchange_tranform_by_copy(
            std::vector<double> const &data,
            typename Transform &value) -> void
        {
            try_assert(data.size() == 6_size, "invalid_animation_transform_length");
            value = Transform{ data[0], data[1], data[2], data[3], data[4], data[5]};
            return;
        }
    };
}