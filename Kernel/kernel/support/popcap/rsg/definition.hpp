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
            const std::string &name_path,
            uint32_t pos) : name_path(name_path), pos(pos)
        {
        }

        ~NameDict(

            ) = default;
    };

#pragma endregion

#pragma region PTXInfo

    struct PTXInfo
    {
    public:
        uint32_t id;
        uint32_t width;
        uint32_t height;

        explicit PTXInfo(

            ) = default;

        explicit PTXInfo(
            uint32_t id, uint32_t width, uint32_t height) : id(id), width(width), height(height)
        {
        }

        ~PTXInfo(

            ) = default;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PTXInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["width"] = nlohmann_json_t.width;
        nlohmann_json_j["height"] = nlohmann_json_t.height;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PTXInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("width").get_to(nlohmann_json_t.width);
        nlohmann_json_j.at("height").get_to(nlohmann_json_t.height);
        return;
    }

#pragma endregion

#pragma region ResInfo

    struct ResInfo
    {
    public:
        std::string path;
        bool use_ptx_info;
        PTXInfo ptx_info;

        explicit ResInfo(

            ) = default;

        explicit ResInfo(
            const std::string &path, 
            bool use_ptx_info) : path(path), use_ptx_info(use_ptx_info)
        {
        }

        ~ResInfo(

            ) = default;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ResInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        if (nlohmann_json_t.use_ptx_info)
        {
            nlohmann_json_j["ptx_info"] = nlohmann_json_t.ptx_info;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ResInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        try {
            nlohmann_json_j.at("ptx_info").get_to(nlohmann_json_t.ptx_info);
            nlohmann_json_t.use_ptx_info = true;
        }
        catch (const nlohmann::ordered_json::out_of_range& e) {
            nlohmann_json_t.use_ptx_info = false;
        }
        return;
    }

#pragma endregion

#pragma region RSG_PacketInfo

    struct PacketInfo
    {
    public:
        uint32_t version;
        uint32_t compression_flags;
        std::vector<ResInfo> res;

        explicit PacketInfo(

            ) = default;

        explicit PacketInfo(
            uint32_t version,
            uint32_t compression_flags) : version(version), compression_flags(compression_flags)
        {
        }

        ~PacketInfo(

            ) = default;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PacketInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["compression_flags"] = nlohmann_json_t.compression_flags;
        nlohmann_json_j["res"] = nlohmann_json_t.res;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PacketInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("compression_flags").get_to(nlohmann_json_t.compression_flags);
        nlohmann_json_j.at("res").get_to(nlohmann_json_t.res);
        return;
    }

#pragma endregion

}
