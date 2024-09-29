#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/miscellaneous/custom/resource_stream_bundle/definition.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Custom::ResourceStreamBundle
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using DataSectionViewStored = std::map<std::string, std::vector<uint8_t>>;

    struct Common
    {
        inline static constexpr auto k_rton_extension_string = ".rton"_sv;

        inline static constexpr auto k_newton_extension_string = ".newton"_sv;

        inline static constexpr auto k_packages_info_flag_count = 2_size;

        inline static constexpr auto k_highest_compression_method = 3_ui;

        inline static constexpr auto k_high_resolution_list = std::array<int, 2>{1536, 1200}; // 1200 for only 2c

        struct PackagesInfoFlagPos
        {
            inline static constexpr auto automatically_converter = 0_size;

            inline static constexpr auto encrypted = 1_size;
        };

        struct PackagesInfoFlag
        {
            bool automatically_converter;
            bool encrypted;
        };

        inline static auto exchange_packages_info_flag(
            PackagesInfoFlag const &value,
            uint32_t &data) -> void
        {
            auto data_bit = std::bitset<k_packages_info_flag_count>{};
            data_bit.set(PackagesInfoFlagPos::automatically_converter, value.automatically_converter);
            data_bit.set(PackagesInfoFlagPos::encrypted, value.encrypted);
            data = static_cast<uint32_t>(data_bit.to_ullong());
            return;
        }

        inline static auto exchange_packages_info_flag(
            uint32_t const &data,
            PackagesInfoFlag &value) -> void
        {
            auto data_bit = std::bitset<k_packages_info_flag_count>(static_cast<uint8_t>(data));
            value.automatically_converter = data_bit.test(PackagesInfoFlagPos::automatically_converter);
            value.encrypted = data_bit.test(PackagesInfoFlagPos::encrypted);
            return;
        }

        template <typename T>
        inline static auto find_duplicate_elements(
            std::vector<T> const &data,
            std::vector<T> &value) -> void
        {
            auto new_data = std::move(data);
            std::sort(new_data.begin(), new_data.end());
            for (auto i : Range(1_size, new_data.size(), 1_size))
            {
                if (new_data[i - 1_size] == new_data[i])
                {
                    if (std::find(value.begin(), value.end(), new_data[i]) == value.end())
                    {
                        value.emplace_back(new_data[i]);
                    }
                }
            }
            return;
        }

        inline static auto unpack_rsg_without_definition(
            std::vector<uint8_t> const &data,
            DataSectionViewStored &value
        ) -> void
        {
            auto packet_definition = PacketStructure{};
            auto packet_stream = DataStreamView{data};
            Sen::Kernel::Support::PopCap::ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_definition, value);
            return;
        }
    };
}