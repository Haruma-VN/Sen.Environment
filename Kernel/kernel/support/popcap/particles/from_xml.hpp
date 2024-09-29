#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/particles/definition.hpp"
#include "kernel/support/popcap/particles/common.hpp"

namespace Sen::Kernel::Support::PopCap::Particles
{
    using namespace Definition;

    class FromXML : Common
    {
    private:
        using XMLDocument = tinyxml2::XMLDocument;

        using XMLElement = tinyxml2::XMLElement;

    protected:
        inline auto read_track_nodes(
            std::vector<ParticlesTrackNode> &value,
            const std::string &data) -> bool
        {
            auto pos = 0_size;
            while (pos < data.size())
            {
                auto node = ParticlesTrackNode{};
                auto next = data[pos];
                if (next == '[')
                {
                    ++pos;
                    auto pos_temp = pos;
                    while (true)
                    {
                        ++pos_temp;
                        if (data[pos_temp] == ' ' || data[pos_temp] == ']')
                        {
                            break;
                        }
                    }
                    node.low_value = std::stof(data.substr(pos, pos_temp - pos));
                    node.low_value_is_null = node.low_value == 0.0;
                    if (data[pos_temp] == ']')
                    {
                        node.high_value = node.low_value;
                        node.high_value_is_null = node.high_value == 0.0;
                        node.distribution = 0;
                        node.distribution_is_null = false;
                        pos = pos_temp + 1;
                    }
                    else
                    {
                        pos = ++pos_temp;
                        if (data[pos] >= 'A' && data[pos] <= 'Z')
                        {
                            while (true)
                            {
                                ++pos_temp;
                                if (data[pos_temp] == ' ')
                                {
                                    break;
                                }
                            }
                            auto trail_string = data.substr(pos, pos_temp - pos);
                            if (auto it = std::find(trail_enum.begin(), trail_enum.end(), std::string_view(trail_string)); it != trail_enum.end())
                            {
                                node.distribution = static_cast<int>(std::distance(trail_enum.begin(), it));
                            }
                            else
                            {
                                trim(trail_string);
                                node.distribution = std::stoi(trail_string.substr("TodCurves("_sv.size(), trail_string.size() - "TodCurves()"_sv.size()));
                            }
                            node.distribution_is_null = false;
                            pos = ++pos_temp;
                        }
                        while (true)
                        {
                            ++pos_temp;
                            if (data[pos_temp] == ']')
                            {
                                break;
                            }
                        }
                        node.high_value = std::stof(data.substr(pos, pos_temp - pos));
                        node.high_value_is_null = node.high_value == 0.0;
                        pos = ++pos_temp;
                    }
                }
                else if (next == '.' || next == '-' || (next >= '0' && next <= '9'))
                {
                    auto pos_temp = pos;
                    while (true)
                    {
                        ++pos_temp;
                        if (pos_temp >= data.size())
                        {
                            break;
                        }
                        if (data[pos_temp] == ' ' || data[pos_temp] == ',')
                        {
                            break;
                        }
                    }
                    node.low_value = std::stof(data.substr(pos, pos_temp - pos));
                    node.high_value = node.low_value;
                    node.low_value_is_null = node.low_value == 0.0;
                    node.high_value_is_null = node.high_value == 0.0;
                    pos = pos_temp;
                }
                // Time
                if (pos >= data.size())
                {
                    node.time = -10000.0;
                    value.emplace_back(node);
                    break;
                }
                next = data[pos];
                if (next == ',')
                {
                    ++pos;
                    auto pos_temp = pos;
                    while (true)
                    {
                        ++pos_temp;
                        if (pos_temp >= data.size() || data[pos_temp] == ' ')
                        {
                            break;
                        }
                    }
                    node.time = std::stof(data.substr(pos, pos_temp - pos));
                    pos = pos_temp;
                }
                else
                {
                    node.time = -10000.0;
                }
                // CurveType
                if ((++pos) >= data.size())
                {
                    value.emplace_back(node);
                    break;
                }
                next = data[pos];
                if (!(next < 'A' || next > 'Z'))
                {
                    auto pos_temp = pos;
                    while (true)
                    {
                        ++pos_temp;
                        if (pos_temp >= data.size() || data[pos_temp] == ' ')
                        {
                            break;
                        }
                    }
                    auto trail_string = data.substr(pos, pos_temp - pos);
                    if (auto it = std::find(trail_enum.begin(), trail_enum.end(), std::string_view(trail_string)); it != trail_enum.end())
                    {
                        node.curve_type = static_cast<int>(std::distance(trail_enum.begin(), it));
                    }
                    else
                    {
                        trim(trail_string);
                        node.curve_type = std::stoi(trail_string.substr("TodCurves("_sv.size(), trail_string.size() - "TodCurves()"_sv.size()));
                    }
                    node.curve_type_is_null = false;
                    pos = ++pos_temp;
                }
                value.emplace_back(node);
            }
            if (value.size() == 0_size)
            {
                return true;
            }
            if (value.front().time < -1000.0)
            {
                value.front().time = 0.0;
            }
            if (value.size() != 1_size && value.back().time < -1000.0)
            {
                value.back().time = 100.0;
            }
            auto delta = 0.0;
            auto last = 0.0;
            for (auto i : Range(value.size()))
            {
                auto &node = value[i];
                if (node.time >= -1000.0)
                {
                    last = node.time;
                    if (i < value.size() - 1_size)
                    {
                        auto pos = i + 1;
                        while (value[pos].time < -1000)
                        {
                            ++pos;
                        }
                        delta = (value[pos].time - value[i].time) / delta;
                    }
                }
                else
                {
                    node.time = last + delta;
                }
                node.time /= 100;
            }
            return false;
        }

        inline auto convert(
            Particles &definition,
            XMLDocument *document) -> void
        {
            auto &emitters = definition.emitters;
            for (auto emitter_element = document->FirstChildElement("Emitter"); emitter_element != nullptr; emitter_element = emitter_element->NextSiblingElement("Emitter"))
            {
                auto emitter = ParticlesEmitter{};
                if (auto value = emitter_element->FirstChildElement("Name"); value != nullptr)
                {
                    emitter.name = std::string(value->FirstChild()->ToText()->Value());
                    emitter.name_is_null = false;
                }
                if (auto value = emitter_element->FirstChildElement("Image"); value != nullptr)
                {
                    emitter.image = std::string(value->FirstChild()->ToText()->Value());
                    emitter.image_is_null = false;
                }
                if (auto value = emitter_element->FirstChildElement("ImageResource"); value != nullptr)
                {
                    emitter.image_path = std::string(value->FirstChild()->ToText()->Value());
                    emitter.image_path_is_null = false;
                }
                if (auto value = emitter_element->FirstChildElement("ImageCol"); value != nullptr)
                {
                    emitter.image_cols = std::stoi(value->FirstChild()->ToText()->Value());
                    emitter.image_cols_is_null = emitter.image_cols == 0;
                }
                if (auto value = emitter_element->FirstChildElement("ImageRow"); value != nullptr)
                {
                    emitter.image_rows = std::stoi(value->FirstChild()->ToText()->Value());
                    emitter.image_rows_is_null = emitter.image_rows == 0;
                }
                if (auto value = emitter_element->FirstChildElement("ImageFrames"); value != nullptr)
                {
                    emitter.image_frames = std::stoi(value->FirstChild()->ToText()->Value());
                    emitter.image_frames_is_null = emitter.image_frames == 1;
                }
                if (auto value = emitter_element->FirstChildElement("Animated"); value != nullptr)
                {
                    emitter.animated = std::stoi(value->FirstChild()->ToText()->Value());
                    emitter.animated_is_null = emitter.animated == 0;
                }
                if (auto value = emitter_element->FirstChildElement("RandomLaunchSpin"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b1 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("AlignLaunchSpin"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b10 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("AlignToPixel"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b100 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("SystemLoops"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b1000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("ParticleLoops"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b10000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("ParticlesDontFollow"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b100000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("RandomStartTime"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b1000000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("DieIfOverloaded"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b10000000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("Additive"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b100000000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("FullScreen"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b1000000000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("SoftwareOnly"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b10000000000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("HardwareOnly"); value != nullptr)
                {
                    emitter.particles_flag |= std::string(value->FirstChild()->ToText()->Value()) == "1" ? 0b100000000000 : 0;
                }
                if (auto value = emitter_element->FirstChildElement("EmitterType"); value != nullptr)
                {
                    auto type = std::string(value->FirstChild()->ToText()->Value());
                    if (auto it = std::find(emitter_type.begin(), emitter_type.end(), std::string_view(type)); it != emitter_type.end())
                    {
                        emitter.emitter_type = static_cast<int>(std::distance(emitter_type.begin(), it));
                    }
                    else
                    {
                        trim(type);
                        emitter.emitter_type = std::stoi(type.substr("Emitter("_sv.size(), type.size() - "Emitter()"_sv.size()));
                    }
                    emitter.emitter_type_is_null = false;
                }
                if (auto value = emitter_element->FirstChildElement("OnDuration"); value != nullptr)
                {
                    emitter.on_duration = std::string(value->FirstChild()->ToText()->Value());
                    emitter.on_duration_is_null = false;
                }
                if (auto value = emitter_element->FirstChildElement("SystemDuration"); value != nullptr)
                {
                    emitter.system_duration_is_null = read_track_nodes(emitter.system_duration, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("CrossFadeDuration"); value != nullptr)
                {
                    emitter.cross_fade_duration_is_null = read_track_nodes(emitter.cross_fade_duration, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SpawnRate"); value != nullptr)
                {
                    emitter.spawn_rate_is_null = read_track_nodes(emitter.spawn_rate, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SpawnMinActive"); value != nullptr)
                {
                    emitter.spawn_min_active_is_null = read_track_nodes(emitter.spawn_min_active, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SpawnMaxActive"); value != nullptr)
                {
                    emitter.spawn_max_active_is_null = read_track_nodes(emitter.spawn_max_active, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SpawnMaxLaunched"); value != nullptr)
                {
                    emitter.spawn_max_launched_is_null = read_track_nodes(emitter.spawn_max_launched, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("EmitterRadius"); value != nullptr)
                {
                    emitter.emitter_radius_is_null = read_track_nodes(emitter.emitter_radius, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("EmitterOffsetX"); value != nullptr)
                {
                    emitter.emitter_offset_x_is_null = read_track_nodes(emitter.emitter_offset_x, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("EmitterOffsetY"); value != nullptr)
                {
                    emitter.emitter_offset_y_is_null = read_track_nodes(emitter.emitter_offset_y, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("EmitterBoxX"); value != nullptr)
                {
                    emitter.emitter_box_x_is_null = read_track_nodes(emitter.emitter_box_x, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("EmitterBoxY"); value != nullptr)
                {
                    emitter.emitter_box_y_is_null = read_track_nodes(emitter.emitter_box_y, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("EmitterPath"); value != nullptr)
                {
                    emitter.emitter_path_is_null = read_track_nodes(emitter.emitter_path, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("EmitterSkewX"); value != nullptr)
                {
                    emitter.emitter_skew_x_is_null = read_track_nodes(emitter.emitter_skew_x, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("EmitterSkewY"); value != nullptr)
                {
                    emitter.emitter_skew_y_is_null = read_track_nodes(emitter.emitter_skew_y, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleDuration"); value != nullptr)
                {
                    emitter.particle_duration_is_null = read_track_nodes(emitter.particle_duration, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SystemRed"); value != nullptr)
                {
                    emitter.system_red_is_null = read_track_nodes(emitter.system_red, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SystemGreen"); value != nullptr)
                {
                    emitter.system_green_is_null = read_track_nodes(emitter.system_green, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SystemBlue"); value != nullptr)
                {
                    emitter.system_blue_is_null = read_track_nodes(emitter.system_blue, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SystemAlpha"); value != nullptr)
                {
                    emitter.system_alpha_is_null = read_track_nodes(emitter.system_alpha, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("SystemBrightness"); value != nullptr)
                {
                    emitter.system_brightness_is_null = read_track_nodes(emitter.system_brightness, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("LaunchSpeed"); value != nullptr)
                {
                    emitter.launch_speed_is_null = read_track_nodes(emitter.launch_speed, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("LaunchAngle"); value != nullptr)
                {
                    emitter.launch_angle_is_null = read_track_nodes(emitter.launch_angle, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("Field"); value != nullptr)
                {
                    while (value != nullptr)
                    {
                        auto field = ParticlesField{};
                        if (auto field_type_element = value->FirstChildElement("FieldType"); field_type_element != nullptr)
                        {
                            auto type = std::string(field_type_element->FirstChild()->ToText()->Value());
                            if (auto it = std::find(field_type.begin(), field_type.end(), std::string_view(type)); it != field_type.end())
                            {
                                field.field_type = static_cast<int>(std::distance(field_type.begin(), it));
                            }
                            else
                            {
                                trim(type);
                                field.field_type = std::stoi(type.substr("Field("_sv.size(), type.size() - "Field()"_sv.size()));
                            }
                            field.field_type_is_null = false;
                        }
                        if (auto x_element = value->FirstChildElement("X"); x_element != nullptr)
                        {
                            field.x_is_null = read_track_nodes(field.x, std::string(x_element->FirstChild()->ToText()->Value()));
                        }
                        if (auto y_element = value->FirstChildElement("Y"); y_element != nullptr)
                        {
                            field.y_is_null = read_track_nodes(field.y, std::string(y_element->FirstChild()->ToText()->Value()));
                        }
                        emitter.field.emplace_back(field);
                        value = value->NextSiblingElement("Field");
                    }
                    emitter.field_is_null = emitter.field.size() == 0_size;
                }
                if (auto value = emitter_element->FirstChildElement("SystemField"); value != nullptr)
                {
                    
                    while (value != nullptr)
                    {
                        auto field = ParticlesField{};
                        if (auto field_type_element = value->FirstChildElement("FieldType"); field_type_element != nullptr)
                        {
                            auto type = std::string(field_type_element->FirstChild()->ToText()->Value());
                            if (auto it = std::find(field_type.begin(), field_type.end(), std::string_view(type)); it != field_type.end())
                            {
                                field.field_type = static_cast<int>(std::distance(field_type.begin(), it));
                            }
                            else
                            {
                                trim(type);
                                field.field_type = std::stoi(type.substr("Field("_sv.size(), type.size() - "Field()"_sv.size()));
                            }
                            field.field_type_is_null = false;
                        }
                        if (auto x_element = value->FirstChildElement("X"); x_element != nullptr)
                        {
                            field.x_is_null = read_track_nodes(field.x, std::string(x_element->FirstChild()->ToText()->Value()));
                        }
                        if (auto y_element = value->FirstChildElement("Y"); y_element != nullptr)
                        {
                            field.y_is_null = read_track_nodes(field.y, std::string(y_element->FirstChild()->ToText()->Value()));
                        }
                        emitter.system_field.emplace_back(field);
                        value = value->NextSiblingElement("SystemField");
                    }
                    emitter.system_field_is_null = emitter.system_field.size() == 0_size;
                }
                if (auto value = emitter_element->FirstChildElement("ParticleRed"); value != nullptr)
                {
                    emitter.particle_red_is_null = read_track_nodes(emitter.particle_red, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleGreen"); value != nullptr)
                {
                    emitter.particle_green_is_null = read_track_nodes(emitter.particle_green, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleBlue"); value != nullptr)
                {
                    emitter.particle_blue_is_null = read_track_nodes(emitter.particle_blue, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleAlpha"); value != nullptr)
                {
                    emitter.particle_alpha_is_null = read_track_nodes(emitter.particle_alpha, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleBrightness"); value != nullptr)
                {
                    emitter.particle_brightness_is_null = read_track_nodes(emitter.particle_brightness, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleSpinAngle"); value != nullptr)
                {
                    emitter.particle_spin_angle_is_null = read_track_nodes(emitter.particle_spin_angle, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleSpinSpeed"); value != nullptr)
                {
                    emitter.particle_spin_speed_is_null = read_track_nodes(emitter.particle_spin_speed, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleScale"); value != nullptr)
                {
                    emitter.particle_scale_is_null = read_track_nodes(emitter.particle_scale, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ParticleStretch"); value != nullptr)
                {
                    emitter.particle_stretch_is_null = read_track_nodes(emitter.particle_stretch, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("CollisionReflect"); value != nullptr)
                {
                    emitter.collision_reflect_is_null = read_track_nodes(emitter.collision_reflect, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("CollisionSpin"); value != nullptr)
                {
                    emitter.collision_spin_is_null = read_track_nodes(emitter.collision_spin, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ClipTop"); value != nullptr)
                {
                    emitter.clip_top_is_null = read_track_nodes(emitter.clip_top, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ClipBottom"); value != nullptr)
                {
                    emitter.clip_bottom_is_null = read_track_nodes(emitter.clip_bottom, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ClipLeft"); value != nullptr)
                {
                    emitter.clip_left_is_null = read_track_nodes(emitter.clip_left, std::string(value->FirstChild()->ToText()->Value()));
                }
                if (auto value = emitter_element->FirstChildElement("ClipRight"); value != nullptr)
                {
                    emitter.clip_right_is_null = read_track_nodes(emitter.clip_right, std::string(value->FirstChild()->ToText()->Value()));
                }
                emitters.emplace_back(emitter);
            }
            return;
        }

    public:
        explicit FromXML(

            ) = default;

        ~FromXML(

            ) = default;

        inline auto process(
            Particles &definition,
            XMLDocument &document) -> void
        {
            convert(definition, &document);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto from_xml = FromXML{};
            auto definition = Particles{};
            auto document = XMLDocument{};
            FileSystem::read_xml(source, &document);
            from_xml.process(definition, document);
            FileSystem::write_json(destination, definition);
            return;
        }
    };
}
