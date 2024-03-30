#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::PAK
{
    inline static constexpr auto info_end = 0x80_byte;
    
        inline static constexpr auto pak_pc_head = 1295498551;

        inline static constexpr auto pak_default_head = -1161803072;

        inline static constexpr auto pak_tv_head = 67324752;

        inline static constexpr auto pak_version = 0;

        inline static constexpr auto forward_slash = std::string_view{"/"};

        inline static constexpr auto double_forward_slash = std::string_view{"//"};

        inline static constexpr auto back_slash = std::string_view{"\\"};

        inline static constexpr auto double_back_slash = std::string_view{"\\\\"};

        inline static constexpr auto empty_back_slash  = std::string_view{" \\"};

        inline static constexpr auto empty_forward_slash  = std::string_view{" /"};

    struct PakInfo {
        std::string pak_platform; //"PC" "Xbox_360" "TV";
        bool windows_path_separate;
        bool zlib_compress;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PakInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["pak_platform"] = nlohmann_json_t.pak_platform;
        nlohmann_json_j["windows_path_separate"] = nlohmann_json_t.windows_path_separate;
        nlohmann_json_j["zlib_compress"] = nlohmann_json_t.zlib_compress;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PakInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("pak_platform").get_to(nlohmann_json_t.pak_platform);
        nlohmann_json_j.at("windows_path_separate").get_to(nlohmann_json_t.windows_path_separate);
        nlohmann_json_j.at("zlib_compress").get_to(nlohmann_json_t.zlib_compress);
        return;
    }

    struct FileInfo {
        std::string file_name;
        uint32_t zlib_size;
        uint32_t size;
        int64_t file_time = 129146222018596744ll;
    };

}