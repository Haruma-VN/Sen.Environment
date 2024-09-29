#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Marmalade::DZip
{
    enum class ChunkFlagEnum
    {
        combuf,
        dzip,
        zlib,
        bzip2,
        mp3,
        jpeg,
        zerod_out,
        copy_coded,
        lzma,
        random_access
    };
    
    struct Chunk
    {
        ChunkFlagEnum flag;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Chunk &nlohmann_json_t) -> void
    {
        nlohmann_json_j["flag"] = magic_enum::enum_name(nlohmann_json_t.flag);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Chunk &nlohmann_json_t) -> void
    {
        nlohmann_json_t.flag = magic_enum_cast<ChunkFlagEnum>(nlohmann_json_j.at("flag").get<std::string>());
        return;
    }

    struct Resource
    {
        std::string path;
        std::vector<Chunk> chunk;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Resource &nlohmann_json_t) -> void
    {
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        nlohmann_json_j["chunk"] = nlohmann_json_t.chunk;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Resource &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        nlohmann_json_j.at("chunk").get_to(nlohmann_json_t.chunk);
        return;
    }

    struct Package
    {
        std::vector<Resource> resource;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Package &nlohmann_json_t) -> void
    {
        nlohmann_json_j["resource"] = nlohmann_json_t.resource;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Package &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("resource").get_to(nlohmann_json_t.resource);
        return;
    }
}