#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::ResourceStreamBundle
{
    using GroupList = std::vector<std::string>;

    enum ExpandPath {
        String,
        Array
    };

    struct InfoStructure {
        uint32_t version;
        size_t texture_information_section_size;
        ExpandPath expand_path;
        bool is_ios_texture_format;
        GroupList group;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const InfoStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["texture_information_section_size"] = nlohmann_json_t.texture_information_section_size;
        nlohmann_json_j["expand_path"] = nlohmann_json_t.expand_path == String ? "string" : "array";
        nlohmann_json_j["is_ios_texture_format"] = nlohmann_json_t.is_ios_texture_format;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        InfoStructure &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("texture_information_section_size").get_to(nlohmann_json_t.texture_information_section_size);
        auto expand_path_string = nlohmann_json_j["expand_path"].get<std::string>();
        assert_conditional(!(expand_path_string != "string" && expand_path_string != "array"), "invaild_expand_path", "from_json"); //TODO: add to localization.
        nlohmann_json_t.expand_path = expand_path_string == "string" ? ExpandPath::String : ExpandPath::Array;
        nlohmann_json_j.at("is_ios_texture_format").get_to(nlohmann_json_t.is_ios_texture_format);
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        return;
    }

}