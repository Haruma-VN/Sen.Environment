#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/invoke.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/miscellaneous/custom/stream_compressed_group/definition.hpp"

namespace Sen::Kernel::Support::Miscellaneous::Custom::StreamCompressedGroup
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    using ImageFormat = Sen::Kernel::Support::Texture::Format;

    struct Common
    {

        inline static constexpr auto k_magic_identifier = 1935894321_ui;

        inline static constexpr auto k_version_list = std::array<int, 3>{1, 3, 4};

        inline static constexpr auto k_vaild_resolution_list = std::array<int, 5>{1536, 1200, 768, 640, 384};

        inline static constexpr auto k_default_coordinate_offset = 0;

        inline static constexpr auto k_default_layout_offset = 1;

        inline static constexpr auto k_resource_content_information_magic_identifier = 1936876393_ui;

        inline static constexpr auto k_resource_content_information_version = 1_ui;

        inline static constexpr auto k_subgroup_information_section_block_size = 144_size;

        inline static constexpr auto k_compsite_flag_count = 4_size;

        inline static constexpr auto k_empty_section_block_size = 0x08_size;

        inline static constexpr auto string_block_size = 128_size;

        inline static constexpr auto k_composite_flag_offset = 0x10_size;

        inline static constexpr auto k_data_block_padding_size = 0x10_size;

        inline static constexpr auto k_program_path = "!program"_sv;

        inline static constexpr auto k_atlases_folder_string = "atlases"_sv;

        inline static constexpr auto k_default_texture_packet_id = "ATLASIMAGE_ATLAS"_sv;

        inline static auto const k_windows_style = std::string{"\\"};

        inline static auto const k_posix_style = std::string{"/"};

        struct CompositeFlagPos
        {
            inline static constexpr auto composite = 0_size;

            inline static constexpr auto is_android = 1_size;

            inline static constexpr auto is_ios = 2_size;

            inline static constexpr auto is_chinese = 3_size;
        };

        struct CompositeFlag
        {
            bool composite;
            bool is_android;
            bool is_ios;
            bool is_chinese;
        };
        inline static auto exchange_composite_flag(
            CompositeFlag const &value,
            uint32_t &data) -> void
        {
            auto data_bit = std::bitset<k_compsite_flag_count>{};
            data_bit.set(CompositeFlagPos::composite, value.composite);
            data_bit.set(CompositeFlagPos::is_android, value.is_android);
            data_bit.set(CompositeFlagPos::is_ios, value.is_ios);
            data_bit.set(CompositeFlagPos::is_chinese, value.is_chinese);
            data = static_cast<uint32_t>(data_bit.to_ullong());
            return;
        }

        inline static auto exchange_composite_flag(
            uint32_t const &data,
            CompositeFlag &value) -> void
        {
            auto data_bit = std::bitset<k_compsite_flag_count>(static_cast<uint8_t>(data));
            value.composite = data_bit.test(CompositeFlagPos::composite);
            value.is_android = data_bit.test(CompositeFlagPos::is_android);
            value.is_ios = data_bit.test(CompositeFlagPos::is_ios);
            value.is_chinese = data_bit.test(CompositeFlagPos::is_chinese);
            return;
        }

        inline static auto get_expand_path(
            nlohmann::ordered_json const &data) -> ExpandPath
        {
            try
            {
                auto count = k_begin_index;
                for (auto &element : data["groups"])
                {
                    if (count > 5_size)
                    {
                        break;
                    }
                    if (element.find("resources") != element.end())
                    {
                        for (auto &resource : element["resources"])
                        {
                            resource["path"].get<std::vector<std::string>>();
                        }
                    }
                }
                return ExpandPath::Array;
            }
            catch (nlohmann::ordered_json::exception &e)
            {
                return ExpandPath::String;
            }
        }

        template <auto use_string_for_style>
        inline static auto convert_general(
            nlohmann::ordered_json const &data,
            GenenalSubgroupCompressedInfo &value) -> void
        {
            static_assert(sizeof(use_string_for_style) == sizeof(bool));
            static_assert(use_string_for_style == true or use_string_for_style == false);
            for (auto &resource : data)
            {
                auto id = resource["id"].get<std::string>();
                value.data[id].type = exchange_data_type(resource["type"].get<std::string>());
                if constexpr (use_string_for_style)
                {
                    value.data[id].path = String::replaceAll(resource["path"].get<std::string>(), k_windows_style, k_posix_style);
                }
                else
                {
                    value.data[id].path = String::join(resource["path"].get<std::vector<std::string>>(), k_posix_style);
                }
            }
            return;
        }

        template <auto use_string_for_style>
        inline static auto convert_texture(
            nlohmann::ordered_json const &data,
            TextureSubgroupCompressedInfo &value) -> void
        {
            static_assert(sizeof(use_string_for_style) == sizeof(bool));
            static_assert(use_string_for_style == true or use_string_for_style == false);
            auto atlas = std::vector<nlohmann::ordered_json>{};
            for (auto &element : data)
            {
                if (element.find("atlas") != element.end() && element["atlas"].get<bool>())
                {
                    atlas.emplace_back(element);
                }
            }
            for (auto &parent : atlas)
            {
                auto &atlas_data = value.packet[parent["id"].get<std::string>()];
                if constexpr (use_string_for_style)
                {
                    atlas_data.path = String::replaceAll(parent["path"].get<std::string>(), k_windows_style, k_posix_style);
                }
                else
                {
                    atlas_data.path = String::join(parent["path"].get<std::vector<std::string>>(), k_posix_style);
                }
                atlas_data.dimension.width = parent["width"].get<int>();
                atlas_data.dimension.height = parent["height"].get<int>();
                auto children_in_current_parent = std::vector<nlohmann::ordered_json>{};
                for (auto &element : data)
                {
                    if (element.find("parent") != element.end() and element["parent"].get<std::string>() == parent["id"].get<std::string>())
                    {
                        children_in_current_parent.emplace_back(element);
                    }
                }
                for (auto &element : children_in_current_parent)
                {
                    auto &children_data = atlas_data.data[element["id"].get<std::string>()];
                    if constexpr (use_string_for_style)
                    {
                        children_data.path = String::replaceAll(element["path"].get<std::string>(), k_windows_style, k_posix_style);
                    }
                    else
                    {
                        children_data.path = String::join(element["path"].get<std::vector<std::string>>(), k_posix_style);
                    }
                    children_data.type = exchange_data_type(element["type"].get<std::string>());
                    children_data.texture_info.ax = element["ax"].get<int>();
                    children_data.texture_info.ay = element["ay"].get<int>();
                    children_data.texture_info.ah = element["ah"].get<int>();
                    children_data.texture_info.aw = element["aw"].get<int>();
                    if (element.find("x") != element.end() and element["x"] != k_default_coordinate_offset)
                    {
                        children_data.texture_info.x = element["x"].get<int>();
                    }
                    if (element.find("y") != element.end() and element["y"] != k_default_coordinate_offset)
                    {
                        children_data.texture_info.y = element["y"].get<int>();
                    }
                    if (element.find("rows") != element.end() and element["rows"] != k_default_layout_offset)
                    {
                        children_data.texture_info.rows = element["rows"].get<int>();
                    }
                    if (element.find("cols") != element.end() and element["cols"] != k_default_layout_offset)
                    {
                        children_data.texture_info.cols = element["cols"].get<int>();
                    }
                }
            }
            return;
        }

        template <auto use_string_for_style>
        inline static auto exchange_custom_resource_info(
            nlohmann::ordered_json &data,
            CustomResourceInformation &value) -> void
        {
            static_assert(sizeof(use_string_for_style) == sizeof(bool));
            static_assert(use_string_for_style == true or use_string_for_style == false);
            auto get_res = [](nlohmann::ordered_json const &res) -> int
            {
                if (res.is_string())
                {
                    return std::stoi(res.get<std::string>());
                }
                else if (res.is_number())
                {
                    return res.get<int>();
                }
                else if (res == nullptr)
                {
                    return 0;
                }
                assert_conditional(false, fmt::format("{}", Language::get("popcap.rsb.project.invalid_res_type")), "exchange_custom_resource_info");
            };
            auto first_where = [](
                                   nlohmann::ordered_json const &data,
                                   std::string const &id) -> size_t
            {
                for (auto index : Range(data.size()))
                {
                    if (data[index]["id"].get<std::string>() == id)
                    {
                        return index;
                    }
                }
                return std::string::npos;
            };
            auto index = k_begin_index;
            while (index < data["groups"].size())
            {
                auto &element = data["groups"][index];
                if (element.find("subgroups") != element.end())
                {
                    auto group_id = element["id"].get<std::string>();
                    for (auto &subgroup : element["subgroups"])
                    {
                        auto subgroup_id = subgroup["id"].get<std::string>();
                        try
                        {
                            auto resolution = get_res(subgroup["res"]);
                            if (subgroup.find("res") != subgroup.end() && resolution != static_cast<int>(k_none_size))
                            {
                                if (auto element_index = first_where(data["groups"], subgroup_id); element_index != std::string::npos)
                                {
                                    auto texture_subgroup = TextureSubgroupCompressedInfo{};
                                    convert_texture<use_string_for_style>(data["groups"][element_index]["resources"], texture_subgroup);
                                    if (texture_subgroup.packet.size() == k_none_size)
                                    {
                                        continue;
                                    }
                                    texture_subgroup.resolution = resolution;
                                    value.group[group_id].subgroup[subgroup_id].texture = texture_subgroup;
                                    if (element_index < index)
                                    {
                                        --index;
                                    }
                                    data["groups"].erase(element_index);
                                }
                            }
                            else
                            {
                                if (auto element_index = first_where(data["groups"], subgroup_id); element_index != std::string::npos)
                                {
                                    auto general_subgroup = GenenalSubgroupCompressedInfo{};
                                    if (data["groups"][element_index].find("loc") != data["groups"][element_index].end())
                                    {
                                        general_subgroup.locale = data["groups"][element_index]["loc"].template get<std::string>();
                                    }
                                    convert_general<use_string_for_style>(data["groups"][element_index]["resources"], general_subgroup);
                                    if (general_subgroup.data.size() == k_none_size)
                                    {
                                        continue;
                                    }
                                    value.group[group_id].subgroup[subgroup_id].general = general_subgroup;
                                    if (element_index < index)
                                    {
                                        --index;
                                    }
                                    data["groups"].erase(element_index);
                                }
                            }
                        }
                        catch (nlohmann::ordered_json::exception &e)
                        {
                            assert_conditional(false, fmt::format("{}: {}", subgroup_id, e.what()), "convert_texture");
                        }
                    }
                    if (value.group[group_id].subgroup.size() != k_none_size)
                    {
                        value.group[group_id].composite = true;
                    }
                    data["groups"].erase(index);
                }
                else if (element.find("parent") == element.end() && element.find("resources") != element.end() && element["resources"] != nullptr)
                {
                    try
                    {
                        auto general_subgroup = GenenalSubgroupCompressedInfo{};
                        if (element.find("loc") != element.end())
                        {
                            general_subgroup.locale = element["loc"].get<std::string>();
                        }
                        convert_general<use_string_for_style>(element["resources"], general_subgroup);
                        if (general_subgroup.data.size() == k_none_size)
                        {
                            continue;
                        }
                        auto id = element["id"].get<std::string>();
                        value.group[id].subgroup[id].general = general_subgroup;
                        data["groups"].erase(index);
                    }
                    catch (nlohmann::ordered_json::exception &e)
                    {
                        assert_conditional(false, fmt::format("{}: {}", element["id"].get<std::string>(), e.what()), "convert_general");
                    }
                }
                else
                {
                    ++index;
                }
            }
            return;
        }

        template <auto use_string_for_style>
        inline static auto convert_general(
            GenenalSubgroupCompressedInfo const &data,
            nlohmann::ordered_json &value) -> void
        {
            static_assert(sizeof(use_string_for_style) == sizeof(bool));
            static_assert(use_string_for_style == true or use_string_for_style == false);
            for (auto &[data_id, data_value] : data.data)
            {
                auto resource = nlohmann::ordered_json{
                    {"type", exchange_data_type(data_value.type)},
                    {"slot", 0},
                    {"id", data_id}};
                if constexpr (use_string_for_style)
                {
                    resource["path"] = String::replaceAll(data_value.path, k_posix_style, k_windows_style);
                }
                else
                {
                    resource["path"] = String::split(data_value.path, k_posix_style);
                }
                value["resources"].emplace_back(resource);
            }
            return;
        }

        template <auto use_string_for_style>
        inline static auto convert_texture(
            TextureSubgroupCompressedInfo const &data,
            nlohmann::ordered_json &value) -> void
        {
            static_assert(sizeof(use_string_for_style) == sizeof(bool));
            static_assert(use_string_for_style == true or use_string_for_style == false);
            for (auto &[packet_id, packet_value] : data.packet)
            {
                auto resource = nlohmann::ordered_json{
                    {"type", exchange_data_type(DataType::Image)},
                    {"slot", 0},
                    {"id", packet_id},
                    {"atlas", true},
                    {"runtime", true},
                    {"width", packet_value.dimension.width},
                    {"height", packet_value.dimension.height}};
                if constexpr (use_string_for_style)
                {
                    resource["path"] = String::replaceAll(packet_value.path, k_posix_style, k_windows_style);
                }
                else
                {
                    resource["path"] = String::split(packet_value.path, k_posix_style);
                }
                value["resources"].emplace_back(resource);
                for (auto &[data_id, data_value] : packet_value.data)
                {
                    auto sub_resource = nlohmann::ordered_json{
                        {"type", exchange_data_type(data_value.type)},
                        {"slot", 0},
                        {"id", data_id},
                        {"parent", packet_id}};
                    if constexpr (use_string_for_style)
                    {
                        sub_resource["path"] = String::replaceAll(data_value.path, k_posix_style, k_windows_style);
                    }
                    else
                    {
                        sub_resource["path"] = String::split(data_value.path, k_posix_style);
                    }
                    sub_resource["ax"] = data_value.texture_info.ax;
                    sub_resource["ay"] = data_value.texture_info.ay;
                    sub_resource["aw"] = data_value.texture_info.aw;
                    sub_resource["ah"] = data_value.texture_info.ah;
                    if (data_value.texture_info.x != k_default_coordinate_offset)
                    {
                        sub_resource["x"] = data_value.texture_info.x;
                    }
                    if (data_value.texture_info.y != k_default_coordinate_offset)
                    {
                        sub_resource["y"] = data_value.texture_info.y;
                    }
                    if (data_value.texture_info.rows != k_default_layout_offset)
                    {
                        sub_resource["rows"] = data_value.texture_info.rows;
                    }
                    if (data_value.texture_info.cols != k_default_layout_offset)
                    {
                        sub_resource["cols"] = data_value.texture_info.cols;
                    }
                    value["resources"].emplace_back(sub_resource);
                }
            }
            return;
        }

        template <auto use_string_for_style>
        inline static auto exchange_custom_resource_info(
            CustomResourceInformation const &data,
            nlohmann::ordered_json &value) -> void
        {
            static_assert(sizeof(use_string_for_style) == sizeof(bool));
            static_assert(use_string_for_style == true or use_string_for_style == false);
            value = nlohmann::ordered_json{
                {"version", 1},
                {"content_version", 1},
                {"slot_count", 0},
                {"groups", nlohmann::ordered_json::array()}};
            for (auto &[group_id, group_value] : data.group)
            {
                if (group_value.composite)
                {
                    auto composite_result = nlohmann::ordered_json{
                        {"type", "composite"},
                        {"id", group_id},
                        {"subgroups", nlohmann::ordered_json::array()}};
                    for (auto &[subgroup_id, subgroup_value] : group_value.subgroup)
                    {
                        auto subgroup = nlohmann::ordered_json{{"id", subgroup_id}};
                        if (subgroup_value.texture.resolution != static_cast<int>(k_none_size))
                        {
                            subgroup["res"] = std::to_string(subgroup_value.texture.resolution);
                        }
                        if (!subgroup_value.general.locale.empty())
                        {
                            subgroup["loc"] = subgroup_value.general.locale;
                        }
                        composite_result["subgroups"].emplace_back(subgroup);
                    }
                    value["groups"].emplace_back(composite_result);
                    for (auto &[subgroup_id, subgroup_value] : group_value.subgroup)
                    {
                        if (subgroup_value.texture.resolution != static_cast<int>(k_none_size))
                        {
                            auto result = nlohmann::ordered_json{
                                {"type", "simple"},
                                {"id", subgroup_id},
                                {"res", std::to_string(subgroup_value.texture.resolution)},
                                {"parent", group_id},
                                {"resources", nlohmann::ordered_json::array()}};
                            convert_texture<use_string_for_style>(subgroup_value.texture, result);
                            value["groups"].emplace_back(result);
                        }
                        else
                        {
                            auto result = nlohmann::ordered_json{
                                {"type", "simple"},
                                {"id", subgroup_id}};
                            if (!subgroup_value.general.locale.empty())
                            {
                                result["loc"] = subgroup_value.general.locale;
                            }
                            result["parent"] = group_id;
                            result["resources"] = nlohmann::ordered_json::array();
                            convert_general<use_string_for_style>(subgroup_value.general, result);
                            value["groups"].emplace_back(result);
                        }
                    }
                }
                else
                {
                    for (auto &[subgroup_id, subgroup_value] : group_value.subgroup)
                    {
                        auto result = nlohmann::ordered_json{
                            {"type", "simple"},
                            {"id", subgroup_id}};
                        if (!subgroup_value.general.locale.empty())
                        {
                            result["loc"] = subgroup_value.general.locale;
                        }
                        result["resources"] = nlohmann::ordered_json::array();
                        convert_general<use_string_for_style>(subgroup_value.general, result);
                        value["groups"].emplace_back(result);
                    }
                }
            }
            Sen::Kernel::Support::PopCap::ResourceGroup::RewriteSlot::rewrite_slot_count(value);
            return;
        }

        inline static auto exchange_subgroup_compression_info(
            nlohmann::ordered_json const &data,
            SubgroupCompressedInfo &value) -> void
        {
            if (data["type"] != nullptr)
            {
                value.texture.resolution = std::stoi(data["type"].get<string>());
                for (auto &[packet_id, packet_value] : data["packet"].items())
                {
                    auto &packet = value.texture.packet[packet_id];
                    assert_conditional(packet_value["type"].get<std::string>() == "Image"_sv, String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_image_type")), packet_id), "exchange_subgroup_compression_info");
                    packet.path = packet_value["path"].get<std::string>();
                    packet.dimension.width = packet_value["dimension"]["width"].get<int>();
                    packet.dimension.height = packet_value["dimension"]["height"].get<int>();
                    for (auto &[data_id, data_value] : packet_value["data"].items())
                    {
                        auto &data_info = packet.data[data_id];
                        data_info.type = exchange_data_type(data_value["type"].get<std::string>());
                        data_info.path = data_value["path"].get<std::string>();
                        if (data_value["default"].find("x") != data_value["default"].end() && data_value["default"]["x"].get<int>() != k_default_coordinate_offset)
                        {
                            data_info.texture_info.x = data_value["default"]["x"].get<int>();
                        }
                        if (data_value["default"].find("y") != data_value["default"].end() && data_value["default"]["y"].get<int>() != k_default_coordinate_offset)
                        {
                            data_info.texture_info.y = data_value["default"]["y"].get<int>();
                        }
                        if (data_value["default"].find("rows") != data_value["default"].end() && data_value["default"]["rows"].get<int>() != k_default_layout_offset)
                        {
                            data_info.texture_info.rows = data_value["default"]["rows"].get<int>();
                        }
                        if (data_value["default"].find("cols") != data_value["default"].end() && data_value["default"]["cols"].get<int>() != k_default_layout_offset)
                        {
                            data_info.texture_info.cols = data_value["default"]["cols"].get<int>();
                        }
                        data_info.texture_info.ax = data_value["default"]["ax"].get<int>();
                        data_info.texture_info.ay = data_value["default"]["ay"].get<int>();
                        data_info.texture_info.aw = data_value["default"]["aw"].get<int>();
                        data_info.texture_info.ah = data_value["default"]["ah"].get<int>();
                    }
                }
            }
            else
            {
                for (auto &[data_id, data_value] : data["packet"]["data"].items())
                {
                    auto &data_info = value.general.data[data_id];
                    data_info.type = exchange_data_type(data_value["type"].get<std::string>());
                    data_info.path = data_value["path"].get<std::string>();
                }
            }
            return;
        }

        inline static auto check_resource_in_subgroup_compression_info(
            SubgroupCompressedInfo const &data) -> bool
        {
            if (data.texture.resolution != static_cast<int>(k_none_size))
            {
                for (auto &[packet_id, packet_value] : data.texture.packet)
                {
                    if (packet_value.data.size() == k_none_size)
                    {
                        return false;
                    }
                }
            }
            else
            {
                return data.general.data.size() != k_none_size;
            }
            return true;
        }

        inline static auto exchange_subgroup_compression_info(
            SubgroupCompressedInfo const &data,
            nlohmann::ordered_json &value) -> void
        {
            if (data.texture.resolution != static_cast<int>(k_none_size))
            { // don't set resolution if texture unuse.
                value["type"] = std::to_string(data.texture.resolution);
                auto &packet = value["packet"];
                for (auto &[packet_id, packet_value] : data.texture.packet)
                {
                    packet[packet_id] = nlohmann::ordered_json{
                        {"type", "Image"_sv}, // Always Image
                        {"path", packet_value.path},
                        {"dimension", {{"width", packet_value.dimension.width}, {"height", packet_value.dimension.height}}}};
                    for (auto &[data_id, data_value] : packet_value.data)
                    {
                        auto &data_info = packet[packet_id]["data"][data_id] = nlohmann::ordered_json{
                            {"type", exchange_data_type(data_value.type)}, // Always Image
                            {"path", data_value.path},
                            {"default", {{"ax", data_value.texture_info.ax}, {"ay", data_value.texture_info.ay}, {"aw", data_value.texture_info.aw}, {"ah", data_value.texture_info.ah}, {"x", data_value.texture_info.x}, {"y", data_value.texture_info.y}}}};
                        if (data_value.texture_info.rows != k_default_layout_offset)
                        {
                            data_info["default"]["rows"] = data_value.texture_info.rows;
                        }
                        if (data_value.texture_info.cols != k_default_layout_offset)
                        {
                            data_info["default"]["cols"] = data_value.texture_info.cols;
                        }
                    }
                }
            }
            else
            {
                value["type"] = nullptr;
                auto &packet = value["packet"];
                packet["type"] = "File"_sv; // Always File
                for (auto &[data_id, data_value] : data.general.data)
                {
                    packet["data"][data_id] = nlohmann::ordered_json{
                        {"type", exchange_data_type(data_value.type)},
                        {"path", data_value.path}};
                }
            }
            return;
        }

        //------------------------------------------------------------------------------------------------

        struct ImageSpriteInfo
        {
            bool is_use;
            Definition::Image<int> data;
            std::string subgroup_id;
            std::string path;
            Sen::Kernel::Support::PopCap::Animation::Convert::ImageAdditional additional;
        };

        struct PacketInformation
        {
            bool is_image;
            SubgroupCompressedInfo subgroup_content_information;
            std::map<std::string, std::vector<uint8_t>> resource_data_section_view_stored;
            PacketStructure packet_structure;
        };

        struct HeaderInformaiton
        {
            uint32_t magic;
            uint32_t version;
            uint32_t composite_flag; // uint32_t
            uint32_t subgroup_information_section_block_count;
            uint32_t subgroup_information_section_offset;
            uint32_t subgroup_information_section_block_size;
            uint32_t information_section_size;
        };

        inline static auto exchange_head_information(
            DataStreamView &stream,
            HeaderInformaiton &value) -> void
        {
            stream.read_pos = k_begin_index;
            value.magic = stream.readUint32();
            value.version = stream.readUint32();
            stream.read_pos += static_cast<size_t>(0x08);
            value.composite_flag = stream.readUint32();
            value.subgroup_information_section_block_count = stream.readUint32();
            value.subgroup_information_section_offset = stream.readUint32();
            value.subgroup_information_section_block_size = stream.readUint32();
            value.information_section_size = stream.readUint32();
            return;
        }

        inline static auto exchange_head_information(
            HeaderInformaiton const &value,
            DataStreamView &stream) -> void
        {
            stream.write_pos = k_begin_index;
            stream.writeUint32(value.magic);
            stream.writeUint32(value.version);
            stream.writeNull(static_cast<size_t>(0x08));
            stream.writeUint32(value.composite_flag);
            stream.writeUint32(value.subgroup_information_section_block_count);
            stream.writeUint32(value.subgroup_information_section_offset);
            stream.writeUint32(value.subgroup_information_section_block_size);
            stream.writeUint32(value.information_section_size);
            return;
        }

        inline static auto exchange_padding_block(
            DataStreamView &stream) -> void
        {
            stream.writeNull(compute_padding_size(stream.write_pos, k_data_block_padding_size));
            return;
        }

        struct ResourceContentInformation
        {
            uint32_t magic;
            uint32_t version;
            uint32_t information_compressed_size;
            uint32_t information_string_size;
        };

        inline static auto exchange_resouce_content_information(
            DataStreamView &stream,
            ResourceContentInformation &value) -> void
        {
            value.magic = stream.readUint32();
            value.version = stream.readUint32();
            value.information_compressed_size = stream.readUint32();
            value.information_string_size = stream.readUint32();
            return;
        }

        inline static auto exchange_resouce_content_information(
            ResourceContentInformation const &data,
            DataStreamView &stream) -> void
        {
            // TODO : Add localization
            assert_conditional(data.magic == k_resource_content_information_magic_identifier, "invalid_magic", "exchange_resouce_content_information");
            assert_conditional(data.version == k_resource_content_information_version, "invalid_version", "exchange_resouce_content_information");
            stream.writeUint32(data.magic);
            stream.writeUint32(data.version);
            stream.writeUint32(data.information_compressed_size);
            stream.writeUint32(data.information_string_size);
            return;
        }

        struct SubgroupInformation
        {
            string id;
            bool is_image;
            uint32_t resource_group_stream_section_offset;
            uint32_t resource_group_stream_section_block_size;
            uint32_t resource_content_information_offset;
        };

        inline static auto exchange_to_subgroup(
            DataStreamView &stream,
            SubgroupInformation &value) -> void
        {
            value.id = Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::exchange_string_block<Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::k_subgroup_name_string_block_size>(stream);
            value.is_image = static_cast<bool>(stream.readUint32());
            value.resource_group_stream_section_offset = stream.readUint32();
            value.resource_group_stream_section_block_size = stream.readUint32();
            value.resource_content_information_offset = stream.readUint32();
            return;
        }

        inline static auto exchange_from_subgroup(
            DataStreamView &stream,
            SubgroupInformation const &data) -> void
        {
            Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::exchange_string_block<Sen::Kernel::Support::PopCap::ResourceStreamBundle::Common::k_subgroup_name_string_block_size>(stream, data.id);
            stream.writeUint32(static_cast<uint32_t>(data.is_image));
            stream.writeUint32(data.resource_group_stream_section_offset);
            stream.writeUint32(data.resource_group_stream_section_block_size);
            stream.writeUint32(data.resource_content_information_offset);
            return;
        }

        inline static auto k_image_format_ios = std::map<ImageFormat, uint32_t>{
            {ImageFormat::ARGB_8888, 0_ui}, {ImageFormat::RGBA_4444, 1_ui}, {ImageFormat::RGB_565, 2_ui}, {ImageFormat::RGBA_5551, 3_ui}, {ImageFormat::RGBA_4444_TILED, 21_ui}, {ImageFormat::RGB_565_TILED, 22_ui}, {ImageFormat::RGBA_5551_TILED, 23_ui}, {ImageFormat::RGB_PVRTC_4BPP, 30_ui}, {ImageFormat::RGB_PVRTC_4BPP_A_8, 148_ui}};

        inline static auto k_image_format_android = std::map<ImageFormat, uint32_t>{
            {ImageFormat::RGBA_8888, 0_ui}, {ImageFormat::RGBA_4444, 1_ui}, {ImageFormat::RGB_565, 2_ui}, {ImageFormat::RGBA_5551, 3_ui}, {ImageFormat::RGBA_4444_TILED, 21_ui}, {ImageFormat::RGB_565_TILED, 22_ui}, {ImageFormat::RGBA_5551_TILED, 23_ui}, {ImageFormat::RGB_ETC1_A_8, 147_ui}};

        inline static auto k_image_format_chinese = std::map<ImageFormat, uint32_t>{
            {ImageFormat::RGBA_8888, 0_ui}, {ImageFormat::RGBA_4444, 1_ui}, {ImageFormat::RGB_565, 2_ui}, {ImageFormat::RGBA_5551, 3_ui}, {ImageFormat::RGBA_4444_TILED, 21_ui}, {ImageFormat::RGB_565_TILED, 22_ui}, {ImageFormat::RGBA_5551_TILED, 23_ui}, {ImageFormat::RGB_ETC1_A_PALETTE, 147_ui}};

        inline static auto exchange_image_format(
            TextureFormatCategory const &texture_format_category,
            uint32_t const &data) -> ImageFormat
        {
            switch (texture_format_category)
            {
            case TextureFormatCategory::Android:
            {
                if (auto it = search_element_in_map(k_image_format_android, data); it != k_image_format_android.end())
                {
                    return it->first;
                }
            }
            case TextureFormatCategory::IOS:
            {
                if (auto it = search_element_in_map(k_image_format_ios, data); it != k_image_format_ios.end())
                {
                    return it->first;
                }
            }
            case TextureFormatCategory::Chinese:
            {
                if (auto it = search_element_in_map(k_image_format_chinese, data); it != k_image_format_chinese.end())
                {
                    return it->first;
                }
            }
            default:
                assert_conditional(false, String::format(fmt::format("{}", Language::get("pvz2.scg.invalid_image_format")), std::to_string(data)), "exchange_image_format");
            }
        }

        inline static auto exchange_image_format(
            TextureFormatCategory const &texture_format_category,
            ImageFormat const &data) -> uint32_t
        {
            switch (texture_format_category)
            {
            case TextureFormatCategory::Android:
            {
                if (auto it = search_element_in_map(k_image_format_android, data); it != k_image_format_android.end())
                {
                    return it->second;
                }
            }
            case TextureFormatCategory::IOS:
            {
                if (auto it = search_element_in_map(k_image_format_ios, data); it != k_image_format_ios.end())
                {
                    return it->second;
                }
            }
            case TextureFormatCategory::Chinese:
            {
                if (auto it = search_element_in_map(k_image_format_chinese, data); it != k_image_format_chinese.end())
                {
                    return it->second;
                }
            }
            default:
                assert_conditional(false, String::format(fmt::format("{}", Language::get("pvz2.scg.invalid_image_format")), std::to_string(data)), "exchange_image_format");
            }
        }

        inline static auto exchange_path(
            std::string &path,
            std::string_view before,
            std::string_view after) -> void
        {
            path = fmt::format("{}{}", path.substr(k_begin_index, path.size() - before.size()), after);
            return;
        }

        inline static auto is_program_path(
            std::string const &data) -> bool
        {
            return compare_string(k_program_path, data);
        }

        inline static auto try_fix_popanim(
            DataStreamView &stream) -> bool
        {
            try
            {
                auto animation = Sen::Kernel::Support::PopCap::Animation::SexyAnimation{};
                Sen::Kernel::Support::PopCap::Animation::Decode::process_whole(stream, animation);
                stream.read_pos = k_begin_index;
                return true;
            }
            catch (Exception &ex)
            {
                // try fix this
                try
                {
                    stream.read_pos = k_begin_index;
                    stream.writeNull(8); // max size of number;
                    auto animation = Sen::Kernel::Support::PopCap::Animation::SexyAnimation{};
                    Sen::Kernel::Support::PopCap::Animation::Decode::process_whole(stream, animation);
                    stream.read_pos = k_begin_index;

                    return true;
                }
                catch (Exception &ex)
                {
                    return false;
                }
            }
        }

        inline static auto exchange_image_id_index(
            std::string const &subgroup_id,
            size_t const &image_index) -> std::string
        {
            return (image_index < 10_size) ? fmt::format("{}_0{}", subgroup_id, image_index) : fmt::format("{}_{}", subgroup_id, image_index);
        }

        inline static auto exchange_image_path(
            std::string const &path) -> std::string
        {
            auto string_list = Sen::Kernel::String(path).split("/"_sv);
            if (string_list.size() == k_none_size && string_list.size() < 2_size)
            {
                return path;
            }
            if (compare_string(string_list.front(), "images"_sv))
            {
                string_list.erase(string_list.begin() + 1);
            }
            return String::join(string_list, "/"_sv);
        }

        inline static auto exchange_animation_path(
            std::string const &path) -> std::string
        {
            auto pam_path = exchange_image_path(path);
            return fmt::format("{}/{}", Path::getParents(Path::getParents(pam_path)), Path::getFileName(pam_path));
        }

        inline static auto restore_image_path(
            std::string const &path,
            int const &resolution) -> std::string
        {
            auto string_list = Sen::Kernel::String(path).split("/"_sv);
            if (compare_string(string_list.front(), "images"_sv))
            {
                string_list.insert(string_list.begin() + 1, std::to_string(resolution));
            }
            return String::join(string_list, "/"_sv);
        }

        inline static auto restore_animation_path(
            std::string const &path,
            int const &resolution) -> std::string
        {
            auto pam_path = restore_image_path(path, resolution);
            if (Path::getExtension(pam_path) != ""_sv)
            {
                exchange_path(pam_path, Path::getExtension(pam_path), ""_sv);
            }
            return fmt::format("{}/{}.pam", pam_path, Path::getFileName(pam_path));
        }

        inline static auto exchange_image_split(
            std::map<string, ImageSpriteInfo> &texture_sprite_view_stored,
            Definition::Image<int> const &image,
            std::string const &subgroup_id,
            TexturePacketCompressedInfo const &image_info) -> void
        {
            for (auto &[data_id, data_value] : image_info.data)
            {
                assert_conditional(data_value.type == DataType::Image, String::format(fmt::format("{}", Language::get("pvz2.scg.must_be_image_type")), data_id), "exchange_image");
                auto rectangle = Definition::RectangleFileIO<int>(
                    data_value.texture_info.ax,
                    data_value.texture_info.ay,
                    data_value.texture_info.aw,
                    data_value.texture_info.ah);
                if (rectangle.x + rectangle.width > image_info.dimension.width)
                {
                    rectangle.width = image_info.dimension.width - rectangle.x;
                }
                if (rectangle.y + rectangle.height > image_info.dimension.height)
                {
                    rectangle.height = image_info.dimension.height - rectangle.y;
                }
                auto image_cut = Definition::Image<int>::cut(image, rectangle);
                texture_sprite_view_stored[data_id].subgroup_id = subgroup_id;
                texture_sprite_view_stored[data_id].data.width = image_cut.width;
                texture_sprite_view_stored[data_id].data.height = image_cut.height;
                texture_sprite_view_stored[data_id].data.set_data(image_cut.data());
                texture_sprite_view_stored[data_id].path = exchange_image_path(data_value.path);
                texture_sprite_view_stored[data_id].additional = Sen::Kernel::Support::PopCap::Animation::Convert::ImageAdditional{
                    .x = data_value.texture_info.x,
                    .y = data_value.texture_info.y,
                    .rows = data_value.texture_info.rows,
                    .cols = data_value.texture_info.cols};
            }
            return;
        }

        inline static auto exchange_image_additional(
            ImageFormat const &image_format,
            size_t const &texture_size,
            TexturePacketCompressedInfo &image_info) -> void
        {
            switch (image_format)
            {
            case ImageFormat::RGB_PVRTC_4BPP:
            {
                image_info.additional.pitch = image_info.dimension.width / 2_ui;
                image_info.additional.alpha_size = static_cast<int>(100);
                break;
            }
            case ImageFormat::ARGB_4444:
            case ImageFormat::RGBA_4444:
            case ImageFormat::RGB_565:
            case ImageFormat::RGBA_5551:
            case ImageFormat::RGBA_4444_TILED:
            case ImageFormat::RGB_565_TILED:
            case ImageFormat::RGBA_5551_TILED:
            {
                image_info.additional.pitch = image_info.dimension.width * 2_ui;
                break;
            }
            case ImageFormat::RGB_ETC1_A_PALETTE:
            {
                image_info.additional.pitch = image_info.dimension.width * 4_ui;
                image_info.additional.alpha_size = texture_size - static_cast<size_t>(image_info.dimension.width * image_info.dimension.height / 2);
                image_info.additional.scale = static_cast<int>(100);
                break;
            }
            default:
            {
                image_info.additional.pitch = image_info.dimension.width * 4_ui;
                image_info.additional.alpha_size = static_cast<int>(100);
                break;
            }
            }
            return;
        }

        // ---------------------------------------------------------------------------------------

        struct SubgroupRawContent
        {
            bool is_image;
            // uint32_t compression_flag;
            std::vector<uint8_t> packet_data;
            SubgroupCompressedInfo info;
        };

        struct PacketOriginalInformation
        {
            uint32_t version;
            bool composite;
            TextureFormatCategory texture_format_category;
            std::map<std::string, SubgroupRawContent> subgroup;
        };

        inline static auto exchange_stream_resource_group(
            DataStreamView &stream,
            PacketOriginalInformation const &packet_information) -> void
        {
            auto resource_packet_view_stored = DataStreamView{};
            auto subgroup_information_list = std::vector<SubgroupInformation>{};
            for (auto &[subgroup_id, subgroup_value] : packet_information.subgroup)
            {
                auto subgroup_information = SubgroupInformation{
                    .id = subgroup_id,
                    .is_image = subgroup_value.is_image,
                    .resource_group_stream_section_offset = static_cast<uint32_t>(resource_packet_view_stored.write_pos),
                    .resource_group_stream_section_block_size = static_cast<uint32_t>(subgroup_value.packet_data.size())};
                resource_packet_view_stored.writeBytes(subgroup_value.packet_data);
                exchange_padding_block(resource_packet_view_stored);
                auto content_data_string = dump_json(subgroup_value.info);
                auto resource_content_information = ResourceContentInformation{
                    .magic = k_resource_content_information_magic_identifier,
                    .version = k_resource_content_information_version,
                    .information_string_size = static_cast<uint32_t>(content_data_string.size())};
                subgroup_information.resource_content_information_offset = static_cast<uint32_t>(resource_packet_view_stored.write_pos);
                auto compressed_data = Sen::Kernel::Definition::Encryption::Base64::encode(content_data_string);
                resource_content_information.information_compressed_size = static_cast<uint32_t>(compressed_data.size());
                exchange_resouce_content_information(resource_content_information, resource_packet_view_stored);
                resource_packet_view_stored.writeString(compressed_data);
                exchange_padding_block(resource_packet_view_stored);
                subgroup_information_list.emplace_back(subgroup_information);
            }
            auto header_information = HeaderInformaiton{
                .magic = k_magic_identifier,
                .version = static_cast<uint32_t>(packet_information.version),
                .subgroup_information_section_block_count = static_cast<uint32_t>(subgroup_information_list.size()),
                .subgroup_information_section_block_size = k_subgroup_information_section_block_size};
            auto composite_flag = CompositeFlag{
                .composite = packet_information.composite,
                .is_android = packet_information.texture_format_category == TextureFormatCategory::Android,
                .is_ios = packet_information.texture_format_category == TextureFormatCategory::IOS,
                .is_chinese = packet_information.texture_format_category == TextureFormatCategory::Chinese};
            exchange_composite_flag(composite_flag, header_information.composite_flag);
            exchange_head_information(header_information, stream);
            exchange_padding_block(stream);
            header_information.subgroup_information_section_offset = static_cast<uint32_t>(stream.write_pos);
            auto information_section_size = static_cast<uint32_t>(stream.size()) + static_cast<uint32_t>(subgroup_information_list.size()) * k_subgroup_information_section_block_size;
            for (auto &subgroup_information : subgroup_information_list)
            {
                subgroup_information.resource_group_stream_section_offset += information_section_size;
                subgroup_information.resource_content_information_offset += information_section_size;
            }
            exchange_list(stream, subgroup_information_list, &exchange_from_subgroup);
            header_information.information_section_size = information_section_size;
            stream.writeBytes(resource_packet_view_stored.toBytes());
            exchange_head_information(header_information, stream);
            return;
        }

        inline static auto exchange_stream_resource_group(
            PacketOriginalInformation &packet_information,
            DataStreamView &stream) -> void
        {
            auto header_information = HeaderInformaiton{};
            exchange_head_information(stream, header_information);
            assert_conditional(header_information.magic == k_magic_identifier, fmt::format("{}", Language::get("pvz2.scg.invalid_scg_magic")), "exchange_stream_resource_group");
            auto index = std::find(k_version_list.begin(), k_version_list.end(), static_cast<int>(header_information.version));
            assert_conditional((index != k_version_list.end()), String::format(fmt::format("{}", Language::get("pvz2.scg.invalid_scg_version")), std::to_string(header_information.version)), "exchange_stream_resource_group");
            packet_information.version = static_cast<int>(header_information.version);
            auto composite_flag = CompositeFlag{};
            exchange_composite_flag(header_information.composite_flag, composite_flag);
            packet_information.composite = composite_flag.composite;
            if (composite_flag.is_android)
            {
                packet_information.texture_format_category = TextureFormatCategory::Android;
            }
            else if (composite_flag.is_ios)
            {
                packet_information.texture_format_category = TextureFormatCategory::IOS;
            }
            else if (composite_flag.is_chinese)
            {
                packet_information.texture_format_category = TextureFormatCategory::Chinese;
            }
            else
            {
                assert_conditional(false, fmt::format("{}", Language::get("pvz2.scg.invalid_texture_format_category")), "exchange_stream_resource_group");
            }
            auto subgroup_information_list = std::vector<SubgroupInformation>{};
            stream.read_pos = header_information.subgroup_information_section_offset;
            subgroup_information_list.resize(header_information.subgroup_information_section_block_count);
            exchange_list(stream, subgroup_information_list, &exchange_to_subgroup);
            for (auto &subgroup : subgroup_information_list)
            {
                auto &packet_subgroup = packet_information.subgroup[subgroup.id];
                packet_subgroup.is_image = subgroup.is_image;
                // auto packet_stream = DataStreamView{stream.readBytes(static_cast<size_t>(subgroup.resource_group_stream_section_block_size), static_cast<size_t>(subgroup.resource_group_stream_section_offset))};
                // packet_subgroup.compression_flag = packet_stream.readUint32(0x10_size); // get_compression_flag.
                packet_subgroup.packet_data = std::move(stream.readBytes(static_cast<size_t>(subgroup.resource_group_stream_section_block_size), static_cast<size_t>(subgroup.resource_group_stream_section_offset)));
                stream.read_pos = static_cast<size_t>(subgroup.resource_content_information_offset);
                auto resource_content_information = ResourceContentInformation{};
                exchange_resouce_content_information(stream, resource_content_information);
                assert_conditional(resource_content_information.magic == k_resource_content_information_magic_identifier, String::format(fmt::format("{}", Language::get("popcap.rsb.project.invalid_resource_content_magic")), std::to_string(resource_content_information.magic), std::to_string(k_resource_content_information_magic_identifier)), "exchange_stream_resource_group");
                assert_conditional(resource_content_information.version == k_resource_content_information_version, String::format(fmt::format("{}", Language::get("pvz2.scg.invalid_resource_content_version")), std::to_string(resource_content_information.version), std::to_string(k_resource_content_information_version)), "exchange_stream_resource_group");
                auto compressed_data = stream.readString(static_cast<size_t>(resource_content_information.information_compressed_size));
                auto content_data_string = Sen::Kernel::Definition::Encryption::Base64::decode(compressed_data);
                assert_conditional(content_data_string.size() == static_cast<size_t>(resource_content_information.information_string_size), String::format(fmt::format("{}", Language::get("pvz2.scg.invalid_resource_content_size")), std::to_string(resource_content_information.version), std::to_string(k_resource_content_information_version)), "exchange_stream_resource_group");
                packet_subgroup.info = nlohmann::ordered_json::parse(content_data_string);
            }
            return;
        }

        inline static auto check_scg_composite(
            std::string_view source) -> bool
        {
            auto stream = DataStreamView{source};
            auto composite_flag = CompositeFlag{};
            auto composite_flag_uint = stream.readUint32(k_composite_flag_offset);
            exchange_composite_flag(composite_flag_uint, composite_flag);
            return composite_flag.composite;
        }
    };
}