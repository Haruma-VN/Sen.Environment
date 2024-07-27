#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Package
{
    inline static constexpr auto raw_package_platform_string = "Raw"_sv;

    inline static constexpr auto pc_package_platform_string = "PC"_sv;

    inline static constexpr auto tv_package_platform_string = "TV"_sv;

    inline static constexpr auto xbox_360_package_platform_string = "Xbox_360"_sv;

    struct PackagePlatformEnumeration
    {
        enum class Type : uint8_t
        {
            raw_package_platform_version,
            pc_package_platform_version,
            tv_package_platform_version,
            xbox_360_package_platform_version
        };
    };

    struct Resource
    {
        std::string path;
        uint64_t time;
    };

    
    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Resource &nlohmann_json_t) -> void
    {
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        nlohmann_json_j["time"] = nlohmann_json_t.time;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Resource &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        nlohmann_json_j.at("time").get_to(nlohmann_json_t.time);
        return;
    }

    struct PackageInfomartion
    {
        PackagePlatformEnumeration::Type package_platform;
        bool zlib_compression;
        // bool windows_path_separate;
        std::vector<Resource> resource;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PackageInfomartion &nlohmann_json_t) -> void
    {
        nlohmann_json_j["package_platform"] = nlohmann_json_t.package_platform;
        switch (nlohmann_json_t.package_platform)
        {
        case PackagePlatformEnumeration::Type::raw_package_platform_version:
        {
            nlohmann_json_j["package_platform"] = raw_package_platform_string;
            break;
        };
        case PackagePlatformEnumeration::Type::pc_package_platform_version:
        {
            nlohmann_json_j["package_platform"] = pc_package_platform_string;
            break;
        };
        case PackagePlatformEnumeration::Type::tv_package_platform_version:
        {
            nlohmann_json_j["package_platform"] = tv_package_platform_string;
            break;
        };
        case PackagePlatformEnumeration::Type::xbox_360_package_platform_version:
        {
            nlohmann_json_j["package_platform"] = xbox_360_package_platform_string;
            break;
        };
        default:
            assert_conditional(false, String::format(fmt::format("{}", Language::get("popcap.package.invalid_package_platform")), std::to_string(nlohmann_json_j["package_platform"].get<uint8_t>())), "to_json");
        }
        nlohmann_json_j["zlib_compression"] = nlohmann_json_t.zlib_compression;
        nlohmann_json_j["resource"] = nlohmann_json_t.resource;
        // nlohmann_json_j["windows_path_separate"] = nlohmann_json_t.windows_path_separate;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PackageInfomartion &nlohmann_json_t) -> void
    {
        auto package_platform_string = nlohmann_json_j["package_platform"].get<std::string>();
        switch (hash_sv(package_platform_string))
        {
        case hash_sv(raw_package_platform_string):
        {
            nlohmann_json_t.package_platform = PackagePlatformEnumeration::Type::raw_package_platform_version;
            break;
        };
        case hash_sv(pc_package_platform_string):
        {
            nlohmann_json_t.package_platform = PackagePlatformEnumeration::Type::pc_package_platform_version;
            break;
        };
        case hash_sv(tv_package_platform_string):
        {
            nlohmann_json_t.package_platform = PackagePlatformEnumeration::Type::tv_package_platform_version;
            break;
        };
        case hash_sv(xbox_360_package_platform_string):
        {
            nlohmann_json_t.package_platform = PackagePlatformEnumeration::Type::xbox_360_package_platform_version;
            break;
        };
        default:
            assert_conditional(false, String::format(fmt::format("{}", Language::get("popcap.package.invalid_package_platform")), package_platform_string), "from_json");
        }
        nlohmann_json_j.at("zlib_compression").get_to(nlohmann_json_t.zlib_compression);
        nlohmann_json_j.at("resource").get_to(nlohmann_json_t.resource);
        // nlohmann_json_j.at("windows_path_separate").get_to(nlohmann_json_t.windows_path_separate);
        return;
    }
}