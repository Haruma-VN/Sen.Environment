#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/invoke.hpp"
#include "kernel/support/miscellaneous/shared.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Modding::ResourceStreamBundle
{
    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using namespace Definition;

    struct Pack
    {
    private:
        inline static auto process_package(

        ) -> void
        {
            return;
        }

    public:
        inline static auto process_whole(

        ) -> void
        {
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            return;
        }
    };

}