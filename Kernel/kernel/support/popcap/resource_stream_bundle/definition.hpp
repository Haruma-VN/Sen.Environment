#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/definition.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle
{
    using namespace Sen::Kernel::Support::PopCap::ResourceStreamGroup;

    struct SimpleCategoryInformation
    {
        int resolution;
        std::string locale;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SimpleCategoryInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["resolution"] = nlohmann_json_t.resolution;
        if (!nlohmann_json_t.locale.empty())
        {
            nlohmann_json_j["locale"] = nlohmann_json_t.locale;
        }
        else {
            nlohmann_json_j["locale"] = nullptr;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SimpleCategoryInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("resolution").get_to(nlohmann_json_t.resolution);
        if (nlohmann_json_j.at("locale") != nullptr)
        {
            nlohmann_json_j.at("locale").get_to(nlohmann_json_t.locale);
        }
        return;
    }

    struct SubgroupInformation
    {
        SimpleCategoryInformation category;
        uint32_t compression;
        std::vector<Resource> resource;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SubgroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["category"] = nlohmann_json_t.category;
        nlohmann_json_j["compression"] = nlohmann_json_t.compression;
        nlohmann_json_j["resource"] = nlohmann_json_t.resource;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SubgroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("category").get_to(nlohmann_json_t.category);
        nlohmann_json_j.at("compression").get_to(nlohmann_json_t.compression);
        nlohmann_json_j.at("resource").get_to(nlohmann_json_t.resource);
        return;
    }

    struct GroupInformation
    {
        bool composite;
        std::map<std::string, SubgroupInformation> subgroup;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["composite"] = nlohmann_json_t.composite;
        nlohmann_json_j["subgroup"] = nlohmann_json_t.subgroup;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("composite").get_to(nlohmann_json_t.composite);
        nlohmann_json_j.at("subgroup").get_to(nlohmann_json_t.subgroup);
        return;
    }

    struct BundleStructure
    {
        uint32_t version;
        size_t texture_information_section_size;
        std::map<std::string, GroupInformation> group;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BundleStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["texture_information_version"] = Common::exchange_texture_information_version(nlohmann_json_t.texture_information_section_size);
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BundleStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_t.texture_information_section_size = Common::exchange_texture_information_version(nlohmann_json_j.at("texture_information_version").get<size_t>());
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        return;
    }

    struct ManifestResource {
        std::string path;
        uint16_t type;
        nlohmann::ordered_json property;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ManifestResource &nlohmann_json_t) -> void
    {
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["property"] = nlohmann_json_t.property;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ManifestResource &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        nlohmann_json_j.at("property").get_to(nlohmann_json_t.property);
        return;
    }

    struct ManifestSubgroupInformation {
        SimpleCategoryInformation category;
        std::map<std::string, ManifestResource> resource;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ManifestSubgroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["category"] = nlohmann_json_t.category;
        nlohmann_json_j["resource"] = nlohmann_json_t.resource;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ManifestSubgroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("category").get_to(nlohmann_json_t.category);
        nlohmann_json_j.at("resource").get_to(nlohmann_json_t.resource);
        return;
    }

    struct ManifestGroupInformation {
        bool composite;
        std::map<std::string, ManifestSubgroupInformation> subgroup;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ManifestGroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["composite"] = nlohmann_json_t.composite;
        nlohmann_json_j["subgroup"] = nlohmann_json_t.subgroup;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ManifestGroupInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("composite").get_to(nlohmann_json_t.composite);
        nlohmann_json_j.at("subgroup").get_to(nlohmann_json_t.subgroup);
        return;
    }

    struct ManifestStructure {
        bool manifest_has;
        std::map<std::string, ManifestGroupInformation> group;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ManifestStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ManifestStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        return;
    }
}
