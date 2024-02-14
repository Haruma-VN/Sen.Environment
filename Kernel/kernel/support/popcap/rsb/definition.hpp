#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RSB
{
#pragma region RSB_HeadInfo

    template <typename T> requires std::is_integral<T>::value
    struct RSB_HeadInfo
    {
        std::string_view magic = "1bsr"_sv;
        T version;
        T file_offset;
        T file_list_length;
        T file_list_begin;
        T rsg_list_length;
        T rsg_list_begin;
        T rsg_number;
        T rsg_info_begin;
        T rsg_info_each_length;
        T composite_number;
        T composite_info_begin;
        T composite_info_each_length;
        T composite_list_length;
        T composite_list_begin;
        T autopool_number;
        T autopool_info_begin;
        T autopool_info_each_length;
        T ptx_number;
        T ptx_info_begin;
        T ptx_info_each_length;
        T part1_begin;
        T part2_begin;
        T part3_begin;
    };

    template <typename T> requires std::is_integral<T>::value
    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const RSB_HeadInfo<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["magic"] = nlohmann_json_t.magic; 
        nlohmann_json_j["version"] = nlohmann_json_t.version; 
        nlohmann_json_j["file_offset"] = nlohmann_json_t.file_offset; 
        nlohmann_json_j["file_list_length"] = nlohmann_json_t.file_list_length; 
        nlohmann_json_j["file_list_begin"] = nlohmann_json_t.file_list_begin; 
        nlohmann_json_j["rsg_list_length"] = nlohmann_json_t.rsg_list_length; 
        nlohmann_json_j["rsg_list_begin"] = nlohmann_json_t.rsg_list_begin; 
        nlohmann_json_j["rsg_number"] = nlohmann_json_t.rsg_number; 
        nlohmann_json_j["rsg_info_begin"] = nlohmann_json_t.rsg_info_begin; 
        nlohmann_json_j["rsg_info_each_length"] = nlohmann_json_t.rsg_info_each_length; 
        nlohmann_json_j["composite_number"] = nlohmann_json_t.composite_number; 
        nlohmann_json_j["composite_info_begin"] = nlohmann_json_t.composite_info_begin; 
        nlohmann_json_j["composite_info_each_length"] = nlohmann_json_t.composite_info_each_length; 
        nlohmann_json_j["composite_list_length"] = nlohmann_json_t.composite_list_length; 
        nlohmann_json_j["composite_list_begin"] = nlohmann_json_t.composite_list_begin; 
        nlohmann_json_j["autopool_number"] = nlohmann_json_t.autopool_number; 
        nlohmann_json_j["autopool_info_begin"] = nlohmann_json_t.autopool_info_begin; 
        nlohmann_json_j["autopool_info_each_length"] = nlohmann_json_t.autopool_info_each_length; 
        nlohmann_json_j["ptx_number"] = nlohmann_json_t.ptx_number; 
        nlohmann_json_j["ptx_info_begin"] = nlohmann_json_t.ptx_info_begin; 
        nlohmann_json_j["ptx_info_each_length"] = nlohmann_json_t.ptx_info_each_length; 
        nlohmann_json_j["part1_begin"] = nlohmann_json_t.part1_begin; 
        nlohmann_json_j["part2_begin"] = nlohmann_json_t.part2_begin; 
        nlohmann_json_j["part3_begin"] = nlohmann_json_t.part3_begin;
        return;
    } 
    
    template <typename T> requires std::is_integral<T>::value
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        RSB_HeadInfo<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("magic").get_to(nlohmann_json_t.magic); 
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version); 
        nlohmann_json_j.at("file_offset").get_to(nlohmann_json_t.file_offset); 
        nlohmann_json_j.at("file_list_length").get_to(nlohmann_json_t.file_list_length); 
        nlohmann_json_j.at("file_list_begin").get_to(nlohmann_json_t.file_list_begin); 
        nlohmann_json_j.at("rsg_list_length").get_to(nlohmann_json_t.rsg_list_length); 
        nlohmann_json_j.at("rsg_list_begin").get_to(nlohmann_json_t.rsg_list_begin); 
        nlohmann_json_j.at("rsg_number").get_to(nlohmann_json_t.rsg_number); 
        nlohmann_json_j.at("rsg_info_begin").get_to(nlohmann_json_t.rsg_info_begin); 
        nlohmann_json_j.at("rsg_info_each_length").get_to(nlohmann_json_t.rsg_info_each_length); 
        nlohmann_json_j.at("composite_number").get_to(nlohmann_json_t.composite_number); 
        nlohmann_json_j.at("composite_info_begin").get_to(nlohmann_json_t.composite_info_begin); 
        nlohmann_json_j.at("composite_info_each_length").get_to(nlohmann_json_t.composite_info_each_length); 
        nlohmann_json_j.at("composite_list_length").get_to(nlohmann_json_t.composite_list_length); 
        nlohmann_json_j.at("composite_list_begin").get_to(nlohmann_json_t.composite_list_begin); 
        nlohmann_json_j.at("autopool_number").get_to(nlohmann_json_t.autopool_number); 
        nlohmann_json_j.at("autopool_info_begin").get_to(nlohmann_json_t.autopool_info_begin); 
        nlohmann_json_j.at("autopool_info_each_length").get_to(nlohmann_json_t.autopool_info_each_length); 
        nlohmann_json_j.at("ptx_number").get_to(nlohmann_json_t.ptx_number); 
        nlohmann_json_j.at("ptx_info_begin").get_to(nlohmann_json_t.ptx_info_begin); 
        nlohmann_json_j.at("ptx_info_each_length").get_to(nlohmann_json_t.ptx_info_each_length); 
        nlohmann_json_j.at("part1_begin").get_to(nlohmann_json_t.part1_begin); 
        nlohmann_json_j.at("part2_begin").get_to(nlohmann_json_t.part2_begin); 
        nlohmann_json_j.at("part3_begin").get_to(nlohmann_json_t.part3_begin);
        return;
    }
#pragma endregion


#pragma region NameDict

    template <typename T> requires std::is_integral<T>::value
    struct NameDict
    {
    public:
        std::string name_path;
        T pos;
        explicit NameDict(

            ) = default;

        explicit NameDict(
            std::string name_path,
            T pos
        ) : name_path(name_path), pos(pos)
        {
        }

        ~NameDict(

        ) = default;
    };

    template <typename T> requires std::is_integral<T>::value
    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const NameDict<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["name_path"] = nlohmann_json_t.name_path; 
        nlohmann_json_j["pos"] = nlohmann_json_t.pos;
        return;
    } 
    
    template <typename T> requires std::is_integral<T>::value
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        NameDict<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("name_path").get_to(nlohmann_json_t.name_path); 
        nlohmann_json_j.at("pos").get_to(nlohmann_json_t.pos);
        return;
    };

#pragma endregion
/*
#pragma region RSG_PacketInfo
    template <typename T> requires std::is_integral<T>::value
    struct RSG_PTXInfo
    {
        public:
        T id;
        T format;
        T width;
        T height;

        explicit RSG_PTXInfo(

            ) = default;

        explicit RSG_PTXInfo(
            T id,
            T width,
            T height
        ) : id(id), width(width), height(height)
        {
        }

        ~RSG_PTXInfo(

            ) = default;
    };

    template <typename T> requires std::is_integral<T>::value
    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j,
        const RSG_PTXInfo<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id; 
        nlohmann_json_j["format"] = nlohmann_json_t.format; 
        nlohmann_json_j["width"] = nlohmann_json_t.width; 
        nlohmann_json_j["height"] = nlohmann_json_t.height;
        return;
    } 
    
    template <typename T> requires std::is_integral<T>::value
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j,
        RSG_PTXInfo<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
        nlohmann_json_j.at("format").get_to(nlohmann_json_t.format); 
        nlohmann_json_j.at("width").get_to(nlohmann_json_t.width); 
        nlohmann_json_j.at("height").get_to(nlohmann_json_t.height);
        return;
    }

#pragma endregion


#pragma region RSG_ResInfo

    template <typename T> requires std::is_integral<T>::value
    struct RSG_ResInfo
    {
    public:
        std::string path;
        RSG_PTXInfo<T> ptx_info;
        bool ptx_info_is_null;

        explicit RSG_ResInfo(

        ) : ptx_info_is_null(true)
        {

        }

        explicit RSG_ResInfo(
            std::string path
        ) : path(path), ptx_info_is_null(true)
        {
        }

        ~RSG_ResInfo(

         ) = default;
    };

    template <typename T> requires std::is_integral<T>::value
    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const RSG_ResInfo<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["path"] = nlohmann_json_t.path; 
        if (nlohmann_json_t.ptx_info_is_null) {
            nlohmann_json_j["ptx_info"] = nullptr;
        }
        else {
            nlohmann_json_j["ptx_info"] = nlohmann_json_t.ptx_info;
        }
        return;
    } 
    
    template <typename T> requires std::is_integral<T>::value
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        RSG_ResInfo<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path); 
        if (nlohmann_json_j["ptx_info"].is_null()) {
            nlohmann_json_t.ptx_info_is_null = true;
        }
        else {
            nlohmann_json_j["ptx_info"] = nlohmann_json_t.ptx_info;
            nlohmann_json_t.ptx_info_is_null = false;
        }
        return;
    }

#pragma endregion
*/


#pragma region RSG_PacketInfo

    template <typename T> requires std::is_integral<T>::value
    struct RSG_PacketInfo
    {
    public:
        T compression_flags;
        std::vector<nlohmann::ordered_json> res;

        explicit RSG_PacketInfo(

        ) = default;

        explicit RSG_PacketInfo(
            T compression_flags
        ) : compression_flags(compression_flags)
        {
        }

        ~RSG_PacketInfo(

        ) = default;
    };

    template <typename T> requires std::is_integral<T>::value
    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const RSG_PacketInfo<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["compression_flags"] = nlohmann_json_t.compression_flags; 
        nlohmann_json_j["res"] = nlohmann_json_t.res;
        return;
    } 
    
    template <typename T> requires std::is_integral<T>::value
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        RSG_PacketInfo<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("compression_flags").get_to(nlohmann_json_t.compression_flags); 
        nlohmann_json_j.at("res").get_to(nlohmann_json_t.res);
        return;
    }

#pragma endregion


#pragma region RSG_Info

    template <typename T> requires std::is_integral<T>::value
    struct RSG_Info
    {
    public:
        nlohmann::ordered_json category;
        RSG_PacketInfo<T> packet_info;

        
        explicit RSG_Info(

        ) = default;

        ~RSG_Info(

         ) = default;
    };

    template <typename T> requires std::is_integral<T>::value
    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const RSG_Info<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["category"] = nlohmann_json_t.category; 
        nlohmann_json_j["packet_info"] = nlohmann_json_t.packet_info;
        return;
    } 
    
    template <typename T> requires std::is_integral<T>::value
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        RSG_Info<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("category").get_to(nlohmann_json_t.category); 
        nlohmann_json_j.at("packet_info").get_to(nlohmann_json_t.packet_info);
        return;
    }

#pragma endregion


#pragma region RSG_Group

    template <typename T> requires std::is_integral<T>::value
    struct RSG_Group
    {
    public:
        bool is_composite;
        std::map<std::string, RSG_Info<T>> subgroup;

        explicit RSG_Group(

        ) = default;

        explicit RSG_Group(
            bool is_composite) : is_composite(is_composite)
        {
        }

        ~RSG_Group(

        ) = default;
    };

    template <typename T> requires std::is_integral<T>::value
    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const RSG_Group<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["is_composite"] = nlohmann_json_t.is_composite; 
        nlohmann_json_j["subgroup"] = nlohmann_json_t.subgroup;
        return;
    } 
    
    template <typename T> requires std::is_integral<T>::value
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        RSG_Group<T>& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("is_composite").get_to(nlohmann_json_t.is_composite); 
        nlohmann_json_j.at("subgroup").get_to(nlohmann_json_t.subgroup);
        return;
    }

#pragma endregion


#pragma region Manifest

    template <typename T> requires std::is_integral<T>::value
    struct Manifest
    {
        T version;
        std::map<std::string, RSG_Group<T>> group;
    };

    template <typename T> requires std::is_integral<T>::value
    inline static auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Manifest<T> &nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        return;
    };

    template <typename T> requires std::is_integral<T>::value
    inline static auto from_json(
        const nlohmann::ordered_json &json,
        Manifest<T> &info
    ) -> void
    {
        json.at("version").get_to(info.version);
        json.at("group").get_to(info.group);
        return;
    }

#pragma endregion

#pragma region PropertiesPtxInfo
    struct PropertiesPtxInfo
    {
        std::string imagetype;
        std::string aflags;
        std::string x;
        std::string y;
        std::string ax;
        std::string ay;
        std::string aw;
        std::string ah;
        std::string rows;
        std::string cols;
        std::string parent;
    };

#pragma endregion

#pragma region DescriptionResources
    template <typename T> requires std::is_integral<T>::value
    struct DescriptionResources
    {
        T type;
        std::string path;
        PropertiesPtxInfo ptx_info;
        std::map<std::string, std::string> properties;
    };

#pragma endregion

#pragma region DescriptionSubGroup
    template <typename T> requires std::is_integral<T>::value
    struct DescriptionSubGroup
    {
        std::string res;
        std::string language;
        std::map<std::string, DescriptionResources<T>> resources;
    };

#pragma endregion

#pragma region DescriptionGroup
    template <typename T> requires std::is_integral<T>::value
    struct DescriptionGroup
    {
        bool composite;
        std::map<std::string, DescriptionSubGroup<T>> subgroups;
    };

#pragma endregion

#pragma region Description
    template <typename T> requires std::is_integral<T>::value
    struct Description
    {
        std::map<std::string, DescriptionGroup<T>> groups;
    };

#pragma endregion


}
