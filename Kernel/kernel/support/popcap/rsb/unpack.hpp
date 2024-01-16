#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RSB
{
    using namespace Definition;

    struct Unpack
    {
        private:
            auto unpack_rsb(
                const std::string & file_in,
                const std::string & folder_out
            ) -> void
            {
            }

            auto read_head(
                DataStreamView &sen
            ) -> nlohmann::ordered_json
            {
                auto magic = sen.readString(4);
                if (magic != "1bsr")
                {
                    throw Exception("Invalid RSB head");
                }
                auto version = sen.readUint32();
                sen.readString(4);
                auto file_offset = sen.readUint32();
                auto file_list_length = sen.readUint32();
                auto file_list_begin = sen.readUint32();
                sen.readString(8);
                auto rsg_list_length = sen.readUint32();
                auto rsg_list_begin = sen.readUint32();
                auto rsg_number = sen.readUint32();
                auto rsg_info_begin = sen.readUint32();
                auto rsg_info_each_length = sen.readUint32();
                auto composite_number = sen.readUint32();
                auto composite_info_begin = sen.readUint32();
                auto composite_info_each_length = sen.readUint32();
                auto composite_list_length = sen.readUint32();
                auto composite_list_begin = sen.readUint32();
                auto autopool_number = sen.readUint32();
                auto autopool_info_begin = sen.readUint32();
                auto autopool_info_each_length = sen.readUint32();
                auto ptx_number = sen.readUint32();
                auto ptx_info_begin = sen.readUint32();
                auto ptx_info_each_length = sen.readUint32();
                auto part1_begin = sen.readUint32();
                auto part2_begin = sen.readUint32();
                auto part3_begin = sen.readUint32();
                if (version >= 4)
                {
                    file_offset = sen.readUint32();
                }
                auto result = nlohmann::ordered_json{
                    {"version", version},
                    {"file_offset", file_offset},
                    {"file_list_length", file_list_length},
                    {"file_list_begin", file_list_begin},
                    {"rsg_list_length", rsg_list_length},
                    {"rsg_list_begin", rsg_list_begin},
                    {"rsg_number", rsg_number},
                    {"rsg_info_begin", rsg_info_begin},
                    {"rsg_info_each_length", rsg_info_each_length},
                    {"composite_number", composite_number},
                    {"composite_info_begin", composite_info_begin},
                    {"composite_info_each_length", composite_info_each_length},
                    {"composite_list_length", composite_list_length},
                    {"composite_list_begin", composite_list_begin},
                    {"autopool_number", autopool_number},
                    {"autopool_info_begin", autopool_info_begin},
                    {"autopool_info_each_length", autopool_info_each_length},
                    {"ptx_number", ptx_number},
                    {"ptx_info_begin", ptx_info_begin},
                    {"ptx_info_each_length", ptx_info_each_length},
                    {"part1_begin", part1_begin},
                    {"part2_begin", part2_begin},
                    {"part3_begin", part3_begin}
                };
                return result;
            }
    };
}