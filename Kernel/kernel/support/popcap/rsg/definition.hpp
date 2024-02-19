#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RSG
{

    enum class CompressionFlag : uint8_t
    {
        NO_COMPRESSION,
        DEFAULT_COMPRESSION,
        ATLAS_COMPRESSION,
        BEST_COMPRESSION,
    };


#pragma region RSG_HeadInfo
    struct RSG_HeadInfo
    {
        inline static auto constexpr magic = "pgsr"_sv;
        uint32_t version;
        CompressionFlag flags;
        uint32_t file_pos;
        uint32_t part0_pos;
        uint32_t part0_zlib;
        uint32_t part0_size;
        uint32_t part1_pos;
        uint32_t part1_zlib;
        uint32_t part1_size;
        uint32_t file_list_length;
        uint32_t file_list_pos;
    };

#pragma endregion

#pragma region NameDict
    struct NameDict
    {
    public:
        std::string name_path;
        uint32_t pos;
        explicit NameDict(

            ) = default;

        explicit NameDict(
            std::string name_path,
            uint32_t pos) : name_path(name_path), pos(pos)
        {
        }

        ~NameDict(

            ) = default;
    };

#pragma endregion

#pragma region RSG_PacketInfo

    struct RSG_PacketInfo
    {
    public:
        uint32_t compression_flags;
        std::vector<nlohmann::ordered_json> res;

        explicit RSG_PacketInfo(

            ) = default;

        explicit RSG_PacketInfo(
            uint32_t compression_flags) : compression_flags(compression_flags)
        {
        }

        ~RSG_PacketInfo(

            ) = default;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const RSG_PacketInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["compression_flags"] = nlohmann_json_t.compression_flags;
        nlohmann_json_j["res"] = nlohmann_json_t.res;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        RSG_PacketInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("compression_flags").get_to(nlohmann_json_t.compression_flags);
        nlohmann_json_j.at("res").get_to(nlohmann_json_t.res);
        return;
    }

#pragma endregion

}
