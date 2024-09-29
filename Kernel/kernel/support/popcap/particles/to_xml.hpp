#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/particles/definition.hpp"
#include "kernel/support/popcap/particles/common.hpp"

namespace Sen::Kernel::Support::PopCap::Particles
{
    using namespace Definition;

    class ToXML : Common
    {
    private:
        using XMLDocument = tinyxml2::XMLDocument;

        using XMLElement = tinyxml2::XMLElement;

    protected:
        inline static auto to_fixed(
            float const &number) -> std::string
        {
           // static_assert(sizeof(point) == sizeof(int));
            auto stream = std::ostringstream{};
            stream << std::fixed << std::setprecision(1) << number;
            return stream.str();
        }

        inline auto float_to_string(
            float const &data) -> std::string
        {
            auto ans = to_fixed(data);
            if (auto pos = ans.find(".0"); pos != std::string::npos) {
                ans = ans.substr(0_size, ans.size() - 2_size);
            }
            if (ans.find("0.") == 0)
            {
                return ans.substr(1_size);
            }
            else
            {
                return ans;
            }
        }

        inline auto write_track_nodes(
            const std::vector<ParticlesTrackNode> &data,
            XMLElement *element) -> void
        {
            auto string = std::string{};
            for (auto i : Range(data.size()))
            {
                if (i > 0_size)
                {
                    string += " ";
                }
                auto &node = data[i];
                if (node.low_value == node.high_value)
                {
                    if (node.distribution == 0)
                    {
                        string += fmt::format("[{}]", float_to_string(node.low_value));
                    }
                    else if (node.distribution == 1)
                    {
                        string += float_to_string(node.low_value);
                    }
                    else
                    {
                        string += fmt::format("[{}", float_to_string(node.low_value));
                        if (node.distribution < 0 || node.distribution > 13)
                        {
                            string += fmt::format(" TodCurves({})", node.distribution);
                        }
                        else
                        {
                            string += fmt::format(" {}", trail_enum[node.distribution]);
                        }
                        string += fmt::format(" {}]", float_to_string(node.high_value));
                    }
                }
                else
                {
                    string += fmt::format("[{}", float_to_string(node.low_value));
                    if (node.distribution != 1)
                    {
                        if (node.distribution < 0 || node.distribution > 13)
                        {
                            string += fmt::format(" TodCurves({})", node.distribution);
                        }
                        else
                        {
                            string += fmt::format(" {}", trail_enum[node.distribution]);
                        }
                    }
                    string += fmt::format(" {}]", float_to_string(node.high_value));
                }
                if (node.time != 0.0 && node.time != 1.0)
                {
                    string += fmt::format(",{}", float_to_string(node.time * 100));
                }
                if (node.curve_type != 1.0)
                {
                    if (node.curve_type < 0 || node.curve_type > 13)
                    {
                        string += fmt::format(" TodCurves({})", node.curve_type);
                    }
                    else
                    {
                        string += fmt::format(" {}", trail_enum[node.curve_type]);
                    }
                }
            }
            element->InsertNewText(string.data());
            return;
        }

        inline auto convert(
            Particles const &definition,
            XMLDocument &document) -> void
        {
            for (auto &emitter : definition.emitters)
            {
                auto emitter_element = document.NewElement("Emitter");
                if (!emitter.name_is_null)
                {
                    auto name_element = document.NewElement("Name");
                    name_element->InsertNewText(emitter.name.data());
                    emitter_element->InsertEndChild(name_element);
                }
                if (!emitter.spawn_min_active_is_null)
                {
                    auto value = document.NewElement("SpawnMinActive");
                    write_track_nodes(emitter.spawn_min_active, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.spawn_max_launched_is_null)
                {
                    auto value = document.NewElement("SpawnMaxLaunched");
                    write_track_nodes(emitter.spawn_max_launched, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_duration_is_null)
                {
                    auto value = document.NewElement("ParticleDuration");
                    write_track_nodes(emitter.particle_duration, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_red_is_null)
                {
                    auto value = document.NewElement("ParticleRed");
                    write_track_nodes(emitter.particle_red, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_green_is_null)
                {
                    auto value = document.NewElement("ParticleGreen");
                    write_track_nodes(emitter.particle_green, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_blue_is_null)
                {
                    auto value = document.NewElement("ParticleBlue");
                    write_track_nodes(emitter.particle_blue, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_alpha_is_null)
                {
                    auto value = document.NewElement("ParticleAlpha");
                    write_track_nodes(emitter.particle_alpha, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_brightness_is_null)
                {
                    auto value = document.NewElement("ParticleBrightness");
                    write_track_nodes(emitter.particle_brightness, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_spin_angle_is_null)
                {
                    auto value = document.NewElement("ParticleSpinAngle");
                    write_track_nodes(emitter.particle_spin_angle, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_spin_speed_is_null)
                {
                    auto value = document.NewElement("ParticleSpinSpeed");
                    write_track_nodes(emitter.particle_spin_speed, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_scale_is_null)
                {
                    auto value = document.NewElement("ParticleScale");
                    write_track_nodes(emitter.particle_scale, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.particle_stretch_is_null)
                {
                    auto value = document.NewElement("ParticleStretch");
                    write_track_nodes(emitter.particle_stretch, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_radius_is_null)
                {
                    auto value = document.NewElement("EmitterRadius");
                    write_track_nodes(emitter.emitter_radius, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_type_is_null && emitter.emitter_type != 1)
                {
                    auto value = document.NewElement("EmitterType");
                    if (emitter.emitter_type < 0 || emitter.emitter_type > 4)
                    {
                        value->InsertNewText(emitter_type[emitter.emitter_type].data());
                    }
                    else
                    {
                        value->InsertNewText(fmt::format("Emitter({})", emitter.emitter_type).data());
                    }
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.system_duration_is_null)
                {
                    auto value = document.NewElement("SystemDuration");
                    write_track_nodes(emitter.system_duration, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.animation_rate_is_null)
                {
                    auto value = document.NewElement("AnimationRate");
                    write_track_nodes(emitter.animation_rate, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.image_frames_is_null)
                {
                    auto value = document.NewElement("ImageFrames");
                    value->InsertNewText(std::to_string(emitter.image_frames).data());
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_offset_x_is_null)
                {
                    auto value = document.NewElement("EmitterOffsetX");
                    write_track_nodes(emitter.emitter_offset_x, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_offset_y_is_null)
                {
                    auto value = document.NewElement("EmitterOffsetY");
                    write_track_nodes(emitter.emitter_offset_y, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_box_x_is_null)
                {
                    auto value = document.NewElement("EmitterBoxX");
                    write_track_nodes(emitter.emitter_box_x, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_box_y_is_null)
                {
                    auto value = document.NewElement("EmitterBoxY");
                    write_track_nodes(emitter.emitter_box_y, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_path_is_null)
                {
                    auto value = document.NewElement("EmitterPath");
                    write_track_nodes(emitter.emitter_path, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_skew_x_is_null)
                {
                    auto value = document.NewElement("EmitterSkewX");
                    write_track_nodes(emitter.emitter_skew_x, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.emitter_skew_y_is_null)
                {
                    auto value = document.NewElement("EmitterSkewY");
                    write_track_nodes(emitter.emitter_skew_y, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.image_is_null)
                {
                    auto value = document.NewElement("Image");
                    value->InsertNewText(emitter.image.data());
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.image_path_is_null)
                {
                    auto value = document.NewElement("ImageResource");
                    value->InsertNewText(emitter.image_path.data());
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.field_is_null) {
                    for (auto &field : emitter.field) {
                        auto field_element = document.NewElement("Field");
                        if (field.field_type != 0) {
                            auto field_type_element = document.NewElement("FieldType");
                            if (field.field_type < 0 || field.field_type > 11) {
                                field_type_element->InsertNewText(fmt::format("Field({})", field.field_type).data());
                            }
                            else {
                                field_type_element->InsertNewText(field_type[field.field_type].data());
                            }
                            field_element->InsertEndChild(field_type_element);
                        } 
                        if (!field.x_is_null) {
                            auto x_element = document.NewElement("X");
                            write_track_nodes(field.x, x_element);
                            field_element->InsertEndChild(x_element);
                        }
                        if (!field.y_is_null) {
                            auto y_element = document.NewElement("Y");
                            write_track_nodes(field.y, y_element);
                            field_element->InsertEndChild(y_element);
                        }
                        emitter_element->InsertEndChild(field_element);
                    }
                }
                if (!emitter.system_field_is_null) {
                    for (auto &field : emitter.system_field) {
                        auto field_element = document.NewElement("SystemField");
                        if (field.field_type != 0) {
                            auto field_type_element = document.NewElement("FieldType");
                            if (field.field_type < 0 || field.field_type > 11) {
                                field_type_element->InsertNewText(fmt::format("Field({})", field.field_type).data());
                            }
                            else {
                                field_type_element->InsertNewText(field_type[field.field_type].data());
                            }
                            field_element->InsertEndChild(field_type_element);
                        } 
                        if (!field.x_is_null) {
                            auto x_element = document.NewElement("X");
                            write_track_nodes(field.x, x_element);
                            field_element->InsertEndChild(x_element);
                        }
                        if (!field.y_is_null) {
                            auto y_element = document.NewElement("Y");
                            write_track_nodes(field.y, y_element);
                            field_element->InsertEndChild(y_element);
                        }
                        emitter_element->InsertEndChild(field_element);
                    }
                }
                if (!emitter.image_cols_is_null)
                {
                    auto value = document.NewElement("ImageCol");
                    value->InsertNewText(std::to_string(emitter.image_cols).data());
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.image_rows_is_null)
                {
                    auto value = document.NewElement("ImageRow");
                    value->InsertNewText(std::to_string(emitter.image_rows).data());
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.animated_is_null)
                {
                    auto value = document.NewElement("Animated");
                    value->InsertNewText(std::to_string(emitter.animated).data());
                    emitter_element->InsertEndChild(value);
                }
                auto flag = emitter.particles_flag;
                if ((flag & 0b1) != 0) {
                    auto value = document.NewElement("RandomLaunchSpin");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b10) != 0) {
                    auto value = document.NewElement("AlignLaunchSpin");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b100) != 0) {
                    auto value = document.NewElement("AlignToPixel");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b1000) != 0) {
                    auto value = document.NewElement("SystemLoops");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b10000) != 0) {
                    auto value = document.NewElement("ParticleLoops");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b100000) != 0) {
                    auto value = document.NewElement("ParticlesDontFollow");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b1000000) != 0) {
                    auto value = document.NewElement("RandomStartTime");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b10000000) != 0) {
                    auto value = document.NewElement("DieIfOverloaded");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b100000000) != 0) {
                    auto value = document.NewElement("Additive");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b1000000000) != 0) {
                    auto value = document.NewElement("FullScreen");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b10000000000) != 0) {
                    auto value = document.NewElement("SoftwareOnly");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if ((flag & 0b100000000000) != 0) {
                    auto value = document.NewElement("HardwareOnly");
                    value->InsertNewText("1");
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.on_duration_is_null)
                {
                    auto value = document.NewElement("OnDuration");
                    value->InsertNewText(emitter.on_duration.data());
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.cross_fade_duration_is_null)
                {
                    auto value = document.NewElement("CrossFadeDuration");
                    write_track_nodes(emitter.cross_fade_duration, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.spawn_rate_is_null)
                {
                    auto value = document.NewElement("SpawnRate");
                    write_track_nodes(emitter.spawn_rate, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.spawn_max_active_is_null)
                {
                    auto value = document.NewElement("SpawnMaxActive");
                    write_track_nodes(emitter.spawn_max_active, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.system_red_is_null)
                {
                    auto value = document.NewElement("SystemRed");
                    write_track_nodes(emitter.system_red, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.system_green_is_null)
                {
                    auto value = document.NewElement("SystemGreen");
                    write_track_nodes(emitter.system_green, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.system_blue_is_null)
                {
                    auto value = document.NewElement("SystemBlue");
                    write_track_nodes(emitter.system_blue, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.system_alpha_is_null)
                {
                    auto value = document.NewElement("SystemAlpha");
                    write_track_nodes(emitter.system_alpha, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.system_brightness_is_null)
                {
                    auto value = document.NewElement("SystemBrightness");
                    write_track_nodes(emitter.system_brightness, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.launch_speed_is_null)
                {
                    auto value = document.NewElement("LaunchSpeed");
                    write_track_nodes(emitter.launch_speed, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.launch_angle_is_null)
                {
                    auto value = document.NewElement("LaunchAngle");
                    write_track_nodes(emitter.launch_angle, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.collision_reflect_is_null)
                {
                    auto value = document.NewElement("CollisionReflect");
                    write_track_nodes(emitter.collision_reflect, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.collision_spin_is_null)
                {
                    auto value = document.NewElement("CollisionSpin");
                    write_track_nodes(emitter.collision_spin, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.clip_top_is_null)
                {
                    auto value = document.NewElement("ClipTop");
                    write_track_nodes(emitter.clip_top, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.clip_bottom_is_null)
                {
                    auto value = document.NewElement("ClipBottom");
                    write_track_nodes(emitter.clip_bottom, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.clip_left_is_null)
                {
                    auto value = document.NewElement("ClipLeft");
                    write_track_nodes(emitter.clip_left, value);
                    emitter_element->InsertEndChild(value);
                }
                if (!emitter.clip_right_is_null)
                {
                    auto value = document.NewElement("ClipRight");
                    write_track_nodes(emitter.clip_right, value);
                    emitter_element->InsertEndChild(value);
                }
                document.InsertEndChild(emitter_element);
            }
            return;
        }

    public:
        explicit ToXML(

            ) = default;

        ~ToXML(

            ) = default;

        inline auto process(
            Particles const &definition,
            XMLDocument &document) -> void
        {
            convert(definition, document);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto to_xml = ToXML{};
            auto definition = *FileSystem::read_json(source);
            auto document = XMLDocument{};
            to_xml.process(definition, document);
            FileSystem::write_xml(destination, &document);
            return;
        }
    };
}
