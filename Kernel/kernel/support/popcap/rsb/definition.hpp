#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RSB
{
#pragma region AnimationSize
    struct RSB_HeadInfo
    {
        std::string_view magic = "1bsr"_sv;
        int version;
        int file_offset;
        int file_list_length;
        int file_list_begin;
        int rsg_list_length;
        int rsg_list_begin;
        int rsg_number;
        int rsg_info_begin;
        int rsg_info_each_length;
        int composite_number;
        int composite_info_begin;
        int composite_info_each_length;
        int composite_list_length;
        int composite_list_begin;
        int autopool_number;
        int autopool_info_begin;
        int autopool_info_each_length;
        int ptx_number;
        int ptx_info_begin;
        int ptx_info_each_length;
        int part1_begin;
        int part2_begin;
        int part3_begin;
    };
#pragma endregion

    struct RSB_Data
    {
        std::string name;
        int rsg_res;
        std::string composite_name;
        std::vector<uint8_t> data;
    };
}
