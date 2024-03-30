#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/reanim/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Reanim
{
    using namespace Definition;

    class Encode
    {
    private:
        using XMLDocument = tinyxml2::XMLDocument;

        using XMLElement = tinyxml2::XMLElement;

    protected:
         std::unique_ptr<DataStreamView> stream;
    public:

        explicit Encode() = default;

        ~Encode(

        ) = default;

        inline auto process(
            const ReanimInfo &reanim,
            ReanimPlatform platform) -> void
        {
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination,
            ReanimPlatform platform
        ) -> void
        {
            auto encode = Encode{};
            auto reanim_info = *FileSystem::read_json(source);
            encode.process(reanim_info, platform);
            encode.stream->out_file(destination);
            return;
        }



    };

}