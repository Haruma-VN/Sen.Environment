#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Reanim
{
    inline static constexpr auto transform_tfloat_end = -10000.0f;

    // inline static constexpr auto transform_wp_tfloat_end = -99999.0f;

    inline static constexpr auto reanim_compress_head = -559022380;

    inline static constexpr auto reanim_pc_head = -1282165568;

    inline static constexpr auto reanim_xmb_head = 1833061976;

    inline static constexpr auto reanim_xmb_version = 5;

    inline static constexpr auto reanim_xmb_info = std::array<uint8_t, 0x26>{0x01, 0x1E, 0x53, 0x65, 0x78, 0x79, 0x2E, 0x54, 0x6F, 0x64, 0x4C, 0x69, 0x62, 0x2E, 0x52, 0x65, 0x61, 0x6E, 0x69, 0x6D, 0x52, 0x65, 0x61, 0x64, 0x65, 0x72, 0x2C, 0x20, 0x4C, 0x41, 0x57, 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

    enum class ReanimPlatform : uint8_t
    {
        PC_Compile,
        TV_Compile,
        Phone32_Compile,
        Phone64_Compile,
        GameConsole_Compile,
        RawXML,
        WP_XNB, // xnb extension; doesn't support 
    };

    // template <typename T>
    struct ReanimTransform
    {
    public:
        float x = transform_tfloat_end;
        float y = transform_tfloat_end;
        float kx = transform_tfloat_end;
        float ky = transform_tfloat_end;
        float sx = transform_tfloat_end;
        float sy = transform_tfloat_end;
        float f = transform_tfloat_end;
        float a = transform_tfloat_end;
        std::string i;
        std::string resource;
        std::string i2;
        std::string resource2;
        std::string font;
        std::string text;
        explicit ReanimTransform(

            ) = default;

        explicit ReanimTransform(
            float x,
            float y,
            float kx,
            float ky,
            float sx,
            float sy,
            float f,
            float a) : x(x), y(y), kx(kx), ky(ky), sx(sx), sy(sy), f(f), a(a)
        {
        }

        ~ReanimTransform(

            ) = default;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ReanimTransform &nlohmann_json_t) -> void
    {
        nlohmann_json_j["x"] = nlohmann_json_t.x;
        nlohmann_json_j["y"] = nlohmann_json_t.y;
        nlohmann_json_j["kx"] = nlohmann_json_t.kx;
        nlohmann_json_j["ky"] = nlohmann_json_t.ky;
        nlohmann_json_j["sx"] = nlohmann_json_t.sx;
        nlohmann_json_j["sy"] = nlohmann_json_t.sy;
        nlohmann_json_j["f"] = nlohmann_json_t.f;
        nlohmann_json_j["a"] = nlohmann_json_t.a;
        nlohmann_json_j["i"] = nlohmann_json_t.i;
        nlohmann_json_j["resource"] = nlohmann_json_t.resource;
        nlohmann_json_j["i2"] = nlohmann_json_t.i2;
        nlohmann_json_j["resource2"] = nlohmann_json_t.resource2;
        nlohmann_json_j["font"] = nlohmann_json_t.font;
        nlohmann_json_j["text"] = nlohmann_json_t.text;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ReanimTransform &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
        nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
        nlohmann_json_j.at("kx").get_to(nlohmann_json_t.kx);
        nlohmann_json_j.at("ky").get_to(nlohmann_json_t.ky);
        nlohmann_json_j.at("sx").get_to(nlohmann_json_t.sx);
        nlohmann_json_j.at("sy").get_to(nlohmann_json_t.sy);
        nlohmann_json_j.at("f").get_to(nlohmann_json_t.f);
        nlohmann_json_j.at("a").get_to(nlohmann_json_t.a);
        nlohmann_json_j.at("i").get_to(nlohmann_json_t.i);
        nlohmann_json_j.at("resource").get_to(nlohmann_json_t.resource);
        nlohmann_json_j.at("i2").get_to(nlohmann_json_t.i2);
        nlohmann_json_j.at("resource2").get_to(nlohmann_json_t.resource2);
        nlohmann_json_j.at("font").get_to(nlohmann_json_t.font);
        nlohmann_json_j.at("text").get_to(nlohmann_json_t.text);
        return;
    }

    struct ReanimTrack
    {
        std::string name;
        std::vector<ReanimTransform> transforms;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ReanimTrack &nlohmann_json_t) -> void
    {
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        nlohmann_json_j["transforms"] = nlohmann_json_t.transforms;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ReanimTrack &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        nlohmann_json_j.at("transforms").get_to(nlohmann_json_t.transforms);
        return;
    }

    struct ReanimInfo
    {
        int8_t do_scale;
        float fps;
        std::vector<ReanimTrack> tracks;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ReanimInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["do_scale"] = nlohmann_json_t.do_scale;
        nlohmann_json_j["fps"] = nlohmann_json_t.fps;
        nlohmann_json_j["tracks"] = nlohmann_json_t.tracks;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ReanimInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("do_scale").get_to(nlohmann_json_t.do_scale);
        nlohmann_json_j.at("fps").get_to(nlohmann_json_t.fps);
        nlohmann_json_j.at("tracks").get_to(nlohmann_json_t.tracks);
        return;
    }
}