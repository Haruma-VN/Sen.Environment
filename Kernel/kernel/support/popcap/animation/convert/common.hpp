#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{
    using XMLDocument = tinyxml2::XMLDocument;

    using XMLElement = tinyxml2::XMLElement;

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

        inline static constexpr auto k_native_index = -1;

        inline static constexpr auto k_main_label_string = "main_label"_sv;

        inline static constexpr auto k_sprite_blank_name = "sprite"_sv;

        inline static constexpr auto k_pi = 3.14159265358979323846;

        inline static constexpr auto k_false = false;

        inline static constexpr auto k_begin_index_int = 0;

        inline static constexpr auto k_max_label_name_size = 128_size;

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
            int index;
        };

        struct FrameNode
        {
            int index;
            int duration;
            int resource = k_native_index;
            bool sprite;
            int first_frame;
            Transform transform;
            Color color;
        };

        using FrameNodeList = std::vector<FrameNode>;

        inline auto static to_json_2(
            nlohmann::ordered_json &nlohmann_json_j,
            const FrameNodeList &nlohmann_json_t) -> void
        {
            for (auto &v : nlohmann_json_t)
            {
                auto new_json = nlohmann::ordered_json{};
                new_json["index"] = v.index;
                new_json["duration"] = v.duration;
                new_json["resource"] = v.resource;
                new_json["sprite"] = v.sprite;
                new_json["first_frame"] = v.first_frame;
                nlohmann_json_j.emplace_back(new_json);
            }
            return;
        }

        using FrameNodeStructure = std::map<int, FrameNodeList>;

        inline auto static to_json(
            nlohmann::ordered_json &nlohmann_json_j,
            const FrameNodeStructure &nlohmann_json_t) -> void
        {
            for (auto &[i, v] : nlohmann_json_t)
            {
                to_json_2(nlohmann_json_j[std::to_string(i)], v);
            }
            return;
        }

        struct AnimationNameList
        {
            std::vector<string> image;
            std::vector<string> sprite;
        };

        struct LabelInfo
        {
            int start;
            int duration;
            std::vector<int> layer_index_list;
        };

        struct PackageLibrary
        {
            std::map<std::string, XMLDocument> image;
            std::map<std::string, XMLDocument> sprite;
            std::map<std::string, XMLDocument> label_document;
            tsl::ordered_map<std::string, LabelInfo> label;
            XMLDocument main_sprite;
            FrameNodeStructure frame_node;
        };

        struct FlashPackage
        {
            XMLDocument document;
            PackageLibrary library;
        };

        inline static auto exchange_simple_extra(
            SexyAnimation const &definition,
            ExtraInfo &extra) -> void
        {
            auto &version_list = Sen::Kernel::Support::PopCap::Animation::Common::version_list;
            auto index = std::find(version_list.begin(), version_list.end(), definition.version);
            assert_conditional((index != version_list.end()), fmt::format("{}", Language::get("popcap.animation.invalid_version")), "exchange_simple_extra");
            extra.version = static_cast<int>(definition.version);
            extra.position.x = static_cast<int>(definition.position.x);
            extra.position.y = static_cast<int>(definition.position.y);
            return;
        }

        inline static auto exchange_simple_definition(
            SexyAnimation &definition,
            ExtraInfo const &extra) -> void
        {
            auto &version_list = Sen::Kernel::Support::PopCap::Animation::Common::version_list;
            auto index = std::find(version_list.begin(), version_list.end(), extra.version);
            assert_conditional((index != version_list.end()), fmt::format("{}", Language::get("popcap.animation.invalid_version")), "exchange_simple_definition");
            definition.version = extra.version;
            definition.position.x = static_cast<double>(extra.position.x);
            definition.position.y = static_cast<double>(extra.position.y);
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
            char data) -> bool
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

        template <auto is_label>
        inline static auto exchange_sprite_name_invalid(
            std::string &sprite_name) -> bool
        {
            static_assert(is_label == true || is_label == false, "is_label must be true or false");
            auto check = [&]() -> bool
            {
                auto k_sprite_state_test = false;
                if (std::find(sprite_name.begin(), sprite_name.end(), '/') != sprite_name.end())
                {
                    auto string_list = String{sprite_name}.split("/");
                    sprite_name = string_list.back();
                    k_sprite_state_test = true;
                }
                if (std::find(sprite_name.begin(), sprite_name.end(), '\\') != sprite_name.end())
                {
                    auto string_list = String{sprite_name}.split("\\");
                    sprite_name = string_list.back();
                    k_sprite_state_test = true;
                }
                if (std::find_if(sprite_name.begin(), sprite_name.end(), &find_invalid_char) != sprite_name.end())
                {
                    sprite_name.erase(std::remove_if(sprite_name.begin(), sprite_name.end(), &find_invalid_char));
                    k_sprite_state_test = true;
                }
                if (sprite_name.empty())
                {
                    if constexpr (is_label) {
                        sprite_name += k_sprite_blank_name;
                    } 
                    else {
                        sprite_name += k_sprite_blank_name;
                    }
                    k_sprite_state_test = true;
                }
                return k_sprite_state_test;
            };
            auto index = k_begin_index;
            while (true) {
                if (!check()) {
                    break;
                }
                else {
                    ++index;
                }
            }
            trim(sprite_name);
            return index > k_begin_index;
        }

        inline static auto exchange_default_extra(
            ExtraInfo const &extra) -> void
        {
            k_version = extra.version;
            k_media_scale_ratio = static_cast<double>(k_standard_resolution) / static_cast<double>(extra.resolution);
            return;
        }

        inline static auto exchange_transform_from_variant_to_standard(
            std::vector<double> const &data,
            Transform &value) -> void
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
                value = Transform{cos, sin, -sin, cos, data[1], data[2]};
            }
            else if (data.size() == 6_size)
            {
                value = Transform{data[0], data[1], data[2], data[3], data[4], data[5]};
            }
            else
            {
                assert_conditional(false, "invalid_transform_size", "exchange_transform_from_variant_to_standard");
            }
            return;
        }

        inline static auto exchange_transform_from_standard_to_variant(
            Transform const &data,
            std::vector<double> &value) -> void
        {
            if (data[0] == data[3] && data[1] == -data[2])
            {
                if (data[0] == 1.0 && data[1] == 0.0)
                {
                    value = std::vector<double>{data[4], data[5]};
                    return;
                }
                auto cos = data[0];
                auto sin = data[1];
                auto radian = std::atan(sin / cos) + k_pi * (cos >= 0.0 ? 0.0 : sin >= 0 ? +1.0
                                                                                         : -1.0);
                if (number_is_equal(std::sin(radian), sin, 1e-2) && number_is_equal(std::cos(radian), cos, 1e-2))
                {
                    value = std::vector<double>{radian, data[4], data[5]};
                    return;
                }
            }
            value = std::vector<double>{data[0], data[1], data[2], data[3], data[4], data[5]};
            return;
        }

        inline static auto exchange_tranform_from_rotate_to_standard(
            std::vector<double> const &data,
            Transform &value) -> void
        {
            try_assert(data.size() == 3_size, "invalid_animation_transform_length");
            auto cos = std::cos(data[0]);
            auto sin = std::sin(data[0]);
            value = Transform{cos, sin, -sin, cos, data[1], data[2]};
            return;
        }

        inline static auto number_is_equal(
            double const &x,
            double const &y,
            double const &tolerance) -> boolean
        {
            return std::abs(x - y) < tolerance;
        }

        inline static auto exchange_transform_from_standard_to_rotate(
            Transform const &data,
            std::vector<double> &value) -> void
        {
            try_assert(data[0] == data[3] && data[1] == -data[2], "invalid_animation_transform");
            auto cos = data[0];
            auto sin = data[1];
            auto radian = std::atan(sin / cos) + k_pi * (cos >= 0.0 ? 0.0 : sin >= 0 ? +1.0
                                                                                     : -1.0);
            try_assert(number_is_equal(std::sin(radian), sin, 1e-2) && number_is_equal(std::cos(radian), cos, 1e-2), "radian_is_wrong");
            value = std::vector<double>{radian, data[4], data[5]};
            return;
        }

        inline static auto exchange_tranform_by_copy(
            std::vector<double> const &data,
            Transform &value) -> void
        {
            try_assert(data.size() == 6_size, "invalid_animation_transform_length");
            value = Transform{data[0], data[1], data[2], data[3], data[4], data[5]};
            return;
        }

        inline static auto exchange_tranform_by_copy(
            Transform const &data,
            std::vector<double> &value) -> void
        {
            try_assert(data.size() == 6_size, "invalid_animation_transform_length");
            value = std::vector<double>{data[0], data[1], data[2], data[3], data[4], data[5]};
            return;
        }

        inline static auto exchange_transform(
            XMLElement *data,
            Transform &value) -> void
        {
            auto a = data->FindAttribute("a");
            auto b = data->FindAttribute("b");
            auto c = data->FindAttribute("c");
            auto d = data->FindAttribute("d");
            auto tx = data->FindAttribute("tx");
            auto ty = data->FindAttribute("ty");
            value = Transform{std::stod((a ? a->Value() : "1")), std::stod((b ? b->Value() : "0")), std::stod((c ? c->Value() : "0")), std::stod((d ? d->Value() : "1")), std::stod((tx ? tx->Value() : "0")), std::stod((ty ? ty->Value() : "0"))};
            return;
        }

        inline static auto exchange_color(
            XMLElement *data,
            Color &value) -> void
        {
            auto color_compute = [](
                                     double const &mutil,
                                     double const &offset) -> double
            {
                return static_cast<double>((std::max)(0.0, (std::min)(255.0, mutil * 255.0 + offset)) / 255.0);
            };
            auto r = data->FindAttribute("redMultiplier");
            auto g = data->FindAttribute("greenMultiplier");
            auto b = data->FindAttribute("blueMultiplier");
            auto a = data->FindAttribute("alphaMultiplier");
            auto r_pos = data->FindAttribute("redOffset");
            auto g_pos = data->FindAttribute("greenOffset");
            auto b_pos = data->FindAttribute("blueOffset");
            auto a_pos = data->FindAttribute("alphaOffset");
            value = Color{
                color_compute(std::stod(r ? r->Value() : "1"), std::stod(r_pos ? r_pos->Value() : "0")),
                color_compute(std::stod(g ? g->Value() : "1"), std::stod(g_pos ? g_pos->Value() : "0")),
                color_compute(std::stod(b ? b->Value() : "1"), std::stod(b_pos ? b_pos->Value() : "0")),
                color_compute(std::stod(a ? a->Value() : "1"), std::stod(a_pos ? a_pos->Value() : "0"))};
            return;
        }

        inline static auto fix_media_image(
            std::string const&source,
            std::string const&media_name
        ) -> void
        {
            auto document = XMLDocument{};
			FileSystem::read_xml(source, &document);
            auto elements = document.FirstChildElement("DOMSymbolItem")->FirstChildElement("timeline")->FirstChildElement("DOMTimeline")->FirstChildElement("layers")->FirstChildElement("DOMLayer")->FirstChildElement("frames")->FirstChildElement("DOMFrame")->FirstChildElement("elements");
            auto dom_bitmap_instance = elements->FirstChildElement("DOMBitmapInstance");
            dom_bitmap_instance->SetAttribute("libraryItemName", fmt::format("media/{}", media_name).data());
			FileSystem::write_xml(source, &document);
            return;
        }

        template <typename T>
        inline static auto get_index(
            const std::vector<T> &v,
            T k) -> int
        {
            static_assert(sizeof(T) != 0);
            auto it = std::find(v.begin(), v.end(), k);
            if (it != v.end())
            {
                return static_cast<int>(it - v.begin());
            }
            return -1;
        }

        // trim from start (in place)
        inline static auto ltrim(std::string &s) -> void
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                            { return !std::isspace(ch); }));
            return;
        }

        // trim from end (in place)
        inline static auto rtrim(std::string &s) -> void
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                                 { return !std::isspace(ch); })
                        .base(),
                    s.end());
            return;
        }

        inline static auto trim(
            std::string &s) -> void
        {
            ltrim(s);
            rtrim(s);
            return;
        }

        inline static auto trim_string(
            std::string const &value) -> std::string
        {
            auto str_trim = value;
            trim(str_trim);
            return str_trim;
        }

        template <typename T, typename E>
        inline static auto check_element_in_vector(
            const std::vector<T> &v,
            const E &e) -> bool
        {
            static_assert(sizeof(T) != 0);
            static_assert(sizeof(E) != 0);
            if (std::find(v.begin(), v.end(), e) != v.end())
            {
                return true;
            }
            return false;
        }
    };
}