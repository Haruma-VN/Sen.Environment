#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/invoke.hpp"
#include "kernel/support/miscellaneous/shared.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::ResourceStreamBundle
{
    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using namespace Definition;

    using ImageFormat = Sen::Kernel::Support::Texture::Format;

    struct Common
    {
        inline constexpr static auto k_manifest_string = "__MANIFESTGROUP__";

        inline constexpr static auto k_package_string = "packages"_sv;

    };

}