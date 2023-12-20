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
                auto sen = SenBuffer{file_in};
                auto head_info = read_head(sen);
                // test rsb unpack
                FileSystem::createDirectory(folder_out);
                auto composite_number = head_info["composite_number"].get<UINT32>();
                auto composite_begin = head_info["composite_info_begin"].get<UINT32>();
                auto rsg_info_each_length = head_info["rsg_info_each_length"].get<UINT32>();
                auto composite_size = head_info["composite_info_each_length"].get<UINT32>() * composite_number;
                for (auto i = 0; i < composite_size; i += composite_number) {
                    auto this_pos = composite_begin + i;
                    sen.changePosition(this_pos);
                    auto composite_name  = sen.readString(0x80);
                    composite_name.erase(std::find(composite_name.begin(), composite_name.end(), '\0'), composite_name.end());
                    sen.readString(this_pos + 1024);
                    auto rsg_number = sen.readUint32LE();
                    FileSystem::createDirectory(Path::normalize(fmt::format("{}/{}", folder_out, composite_name)));
                    for (auto k = 0; k < rsg_number; k++) {
                        auto rsg_pos = i + 0x80 + k * 16;
                        sen.changePosition(rsg_pos);
                        auto rsg_info_set = sen.readUint32LE();
                        auto rsg_info_pos = rsg_info_set * rsg_info_each_length;
                        sen.changePosition(rsg_info_set);
                        auto packet_name = sen.readString(0x80);
                        packet_name.erase(std::find(packet_name.begin(), packet_name.end(), '\0'), packet_name.end());
                        auto packet_pos = sen.readUint32LE();
                        auto packet_size = sen.readUint32LE();
                        auto packet_data = sen.get_raw(packet_pos, packet_pos + packet_size);
                        FileSystem::writeBinary(Path::normalize(fmt::format("{}/{}/{}", folder_out, composite_name, packet_name)), packet_data);
                    }

                }
                return;
            }

            auto read_head(
                SenBuffer &sen
            ) -> nlohmann::json
            {
                auto magic = sen.readString(4);
                if (magic != "1bsr")
                {
                    throw std::runtime_error("Invaild RSB head");
                }
                auto version = sen.readUint32LE();
                sen.readString(4);
                auto file_offset = sen.readUint32LE();
                auto file_list_length = sen.readUint32LE();
                auto file_list_begin = sen.readUint32LE();
                sen.readString(8);
                auto rsg_list_length = sen.readUint32LE();
                auto rsg_list_begin = sen.readUint32LE();
                auto rsg_number = sen.readUint32LE();
                auto rsg_info_begin = sen.readUint32LE();
                auto rsg_info_each_length = sen.readUint32LE();
                auto composite_number = sen.readUint32LE();
                auto composite_info_begin = sen.readUint32LE();
                auto composite_info_each_length = sen.readUint32LE();
                auto composite_list_length = sen.readUint32LE();
                auto composite_list_begin = sen.readUint32LE();
                auto autopool_number = sen.readUint32LE();
                auto autopool_info_begin = sen.readUint32LE();
                auto autopool_info_each_length = sen.readUint32LE();
                auto ptx_number = sen.readUint32LE();
                auto ptx_info_begin = sen.readUint32LE();
                auto ptx_info_each_length = sen.readUint32LE();
                auto part1_begin = sen.readUint32LE();
                auto part2_begin = sen.readUint32LE();
                auto part3_begin = sen.readUint32LE();
                if (version >= 4)
                {
                    file_offset = sen.readUint32LE();
                }
                auto result = nlohmann::json{
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