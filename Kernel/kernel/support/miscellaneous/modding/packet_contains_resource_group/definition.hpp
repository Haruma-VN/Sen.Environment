#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::PacketContainsResourceGroup
{
    using namespace Definition;

    struct TextureResourceAdditional {
        bool use;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TextureResourceAdditional &nlohmann_json_t) -> void
    {
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TextureResourceAdditional &nlohmann_json_t) -> void
    {
        return;
    }

    struct GeneralResourceAdditional {
        bool use;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GeneralResourceAdditional &nlohmann_json_t) -> void
    {
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GeneralResourceAdditional &nlohmann_json_t) -> void
    {
        return;
    }


    struct Information {
        bool composite;
        GeneralResourceAdditional general;
        TextureResourceAdditional texture;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Information &nlohmann_json_t) -> void
    {
        nlohmann_json_j["composite"] = nlohmann_json_t.composite;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Information &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("composite").get_to(nlohmann_json_t.composite);
        return;
    }
}