#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank {

    #pragma region using

        using Sen::Kernel::Definition::DataStreamView;

    #pragma endregion
    
    struct Basic {

        protected:
            using uint = unsigned int;

        public:
        
    };

    #pragma region BKHD
	
    struct BKHD : public Basic {

        public:
            uint version;
            uint id;
            uint language;
            std::string head_expand;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const BKHD& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version; 
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["language"] = nlohmann_json_t.language; 
        nlohmann_json_j["head_expand"] = nlohmann_json_t.head_expand;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        BKHD& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version); 
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
        nlohmann_json_j.at("language").get_to(nlohmann_json_t.language); 
        nlohmann_json_j.at("head_expand").get_to(nlohmann_json_t.head_expand);
        return;
    }

    #pragma endregion

    #pragma region init

    struct INIT : public Basic {

    public:
        uint id;
        std::string name;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const INIT& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id; 
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j,
        INIT& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        return;
    };

    #pragma endregion

    #pragma region stmg

    struct STMGStageGroupData : public Basic
    {
        std::string default_transition_time;
        std::vector<std::string> custom_transition;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j,
        const STMGStageGroupData& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["default_transition_time"] = nlohmann_json_t.default_transition_time; 
        nlohmann_json_j["custom_transition"] = nlohmann_json_t.custom_transition;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j,
        STMGStageGroupData& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("default_transition_time").get_to(nlohmann_json_t.default_transition_time); 
        nlohmann_json_j.at("custom_transition").get_to(nlohmann_json_t.custom_transition);
        return;
    }

    struct STMGStageGroup : public Basic
    {
        uint id;
        struct STMGStageGroupData data;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const STMGStageGroup& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id; 
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j,
        STMGStageGroup& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct STMGSwitchGroupData : public Basic
    {
        uint parameter;
        uint parameter_category;
        std::vector<std::string> point;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const STMGSwitchGroupData& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["parameter"] = nlohmann_json_t.parameter; 
        nlohmann_json_j["parameter_category"] = nlohmann_json_t.parameter_category; 
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        STMGSwitchGroupData& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("parameter").get_to(nlohmann_json_t.parameter); 
        nlohmann_json_j.at("parameter_category").get_to(nlohmann_json_t.parameter_category); 
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        return;
    }

    struct STMGSwitchGroup : public Basic
    {
        uint id;
        STMGSwitchGroupData data;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const STMGSwitchGroup& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id; 
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        STMGSwitchGroup& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    };

    struct STMGGameParameter : public Basic
    {
        uint id;
        std::string data;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const STMGGameParameter& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id; 
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j,
        STMGGameParameter& nlohmann_json_t
    ) -> void 
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    #pragma endregion

    #pragma region hirc

    struct HIRC : public Basic
    {
        uint id;
        uint type;
        std::string data;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const HIRC& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id; 
        nlohmann_json_j["type"] = nlohmann_json_t.type; 
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j,
        HIRC& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type); 
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    };

    #pragma endregion

    #pragma region environment

    struct ENVSVolume : public Basic
    {
        std::string volume_value;
        std::vector<std::string> volume_point;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const ENVSVolume& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["volume_value"] = nlohmann_json_t.volume_value; 
        nlohmann_json_j["volume_point"] = nlohmann_json_t.volume_point;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        ENVSVolume& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("volume_value").get_to(nlohmann_json_t.volume_value); 
        nlohmann_json_j.at("volume_point").get_to(nlohmann_json_t.volume_point);
        return;
    }

    struct ENVSLowPassFilter : public Basic
    {
        std::string low_pass_filter_value;
        std::vector<std::string> low_pass_filter_point;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const ENVSLowPassFilter& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["low_pass_filter_value"] = nlohmann_json_t.low_pass_filter_value; 
        nlohmann_json_j["low_pass_filter_point"] = nlohmann_json_t.low_pass_filter_point;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        ENVSLowPassFilter& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("low_pass_filter_value").get_to(nlohmann_json_t.low_pass_filter_value); 
        nlohmann_json_j.at("low_pass_filter_point").get_to(nlohmann_json_t.low_pass_filter_point);
        return;
    }

    struct ENVSHighPassFilter : public Basic
    {
        std::string high_pass_filter_value;
        std::vector<std::string> high_pass_filter_point;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const ENVSHighPassFilter& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["high_pass_filter_value"] = nlohmann_json_t.high_pass_filter_value; 
        nlohmann_json_j["high_pass_filter_point"] = nlohmann_json_t.high_pass_filter_point;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        ENVSHighPassFilter& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("high_pass_filter_value").get_to(nlohmann_json_t.high_pass_filter_value); 
        nlohmann_json_j.at("high_pass_filter_point").get_to(nlohmann_json_t.high_pass_filter_point);
        return;
    }

    struct ENVSItem : public Basic
    {
        ENVSVolume volume;
        ENVSLowPassFilter low_pass_filter;
        ENVSHighPassFilter high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const ENVSItem& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["volume"] = nlohmann_json_t.volume; 
        nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter; 
        nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        return;
    } 

    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        ENVSItem& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume); 
        nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter); 
        nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        return;
    }

    struct ENVS : public Basic
    {
        ENVSItem obstruction;
        ENVSItem occlusion;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const ENVS& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["obstruction"] = nlohmann_json_t.obstruction; 
        nlohmann_json_j["occlusion"] = nlohmann_json_t.occlusion;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        ENVS& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("obstruction").get_to(nlohmann_json_t.obstruction); 
        nlohmann_json_j.at("occlusion").get_to(nlohmann_json_t.occlusion);
        return;
    }

    #pragma endregion

    #pragma region stid

    struct STIDData : public Basic
    {
        uint id;
        std::string name;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const STIDData& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id; 
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        STIDData& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        return;
    }

    #pragma endregion

    #pragma region plat

    struct PLAT : public Basic
    {
        std::string platform;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const PLAT& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["platform"] = nlohmann_json_t.platform;
        return;
    } 

    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        PLAT& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("platform").get_to(nlohmann_json_t.platform);
        return;
    }

    #pragma endregion

    #pragma region definition

    struct WEMDATATemp : public Basic
    {
        uint offset;
        uint length;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const WEMDATATemp& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["offset"] = nlohmann_json_t.offset; 
        nlohmann_json_j["length"] = nlohmann_json_t.length;
        return;
    } 
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        WEMDATATemp& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("offset").get_to(nlohmann_json_t.offset); 
        nlohmann_json_j.at("length").get_to(nlohmann_json_t.length);
        return;
    }


    struct STID : public Basic
    {
        std::vector<STIDData> data;
        uint unknown_type;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const STID& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["data"] = nlohmann_json_t.data; 
        nlohmann_json_j["unknown_type"] = nlohmann_json_t.unknown_type;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        STID& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data); 
        nlohmann_json_j.at("unknown_type").get_to(nlohmann_json_t.unknown_type);
        return;
    }

    struct STMG : public Basic
    {
        std::string volume_threshold;
        std::string max_voice_instances;
        uint unknown_type_1;
        std::vector<STMGStageGroup> stage_group;
        std::vector<STMGSwitchGroup> switch_group;
        std::vector<STMGGameParameter> game_parameter;
        uint unknown_type_2;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const STMG& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["volume_threshold"] = nlohmann_json_t.volume_threshold; 
        nlohmann_json_j["max_voice_instances"] = nlohmann_json_t.max_voice_instances; 
        nlohmann_json_j["unknown_type_1"] = nlohmann_json_t.unknown_type_1; 
        nlohmann_json_j["stage_group"] = nlohmann_json_t.stage_group; 
        nlohmann_json_j["switch_group"] = nlohmann_json_t.switch_group; 
        nlohmann_json_j["game_parameter"] = nlohmann_json_t.game_parameter; 
        nlohmann_json_j["unknown_type_2"] = nlohmann_json_t.unknown_type_2;
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        STMG& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("volume_threshold").get_to(nlohmann_json_t.volume_threshold); 
        nlohmann_json_j.at("max_voice_instances").get_to(nlohmann_json_t.max_voice_instances); 
        nlohmann_json_j.at("unknown_type_1").get_to(nlohmann_json_t.unknown_type_1); 
        nlohmann_json_j.at("stage_group").get_to(nlohmann_json_t.stage_group); 
        nlohmann_json_j.at("switch_group").get_to(nlohmann_json_t.switch_group); 
        nlohmann_json_j.at("game_parameter").get_to(nlohmann_json_t.game_parameter); 
        nlohmann_json_j.at("unknown_type_2").get_to(nlohmann_json_t.unknown_type_2);
        return;
    }

    struct SoundBankInformation : public Basic
    {
        BKHD bank_header;
        std::vector<INIT> initialization;
        STMG game_synchronization;
        std::vector<uint> embedded_media;
        std::vector<HIRC> hierarchy;
        ENVS environments;
        STID reference;
        PLAT platform_setting;
        bool has_initialization = false;
        bool has_game_synchronization = false;
        bool has_embedded_media = false;
        bool has_hierarchy = false;
        bool has_environments = false;
        bool has_reference = false;
        bool has_platform_setting = false;
    };

    inline auto to_json(
        nlohmann::ordered_json& nlohmann_json_j, 
        const SoundBankInformation& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j["bank_header"] = nlohmann_json_t.bank_header;
        #define sound_bank_has_or(check, _property, __property, or_value)\
        if (nlohmann_json_t.check) {\
            nlohmann_json_j[_property] = nlohmann_json_t.__property;\
        }\
        else {\
            nlohmann_json_j[_property] = or_value;\
        }
        sound_bank_has_or(has_initialization, "initialization", initialization, nullptr);
        sound_bank_has_or(has_game_synchronization, "game_synchronization", game_synchronization, nullptr);
        sound_bank_has_or(has_embedded_media, "embedded_media", embedded_media, nullptr);
        sound_bank_has_or(has_hierarchy, "hierarchy", hierarchy, nullptr);
        sound_bank_has_or(has_environments, "environments", environments, nullptr);
        sound_bank_has_or(has_reference, "reference", reference, nullptr);
        sound_bank_has_or(has_platform_setting, "platform_setting", platform_setting, nullptr);
        return;
    } 
    
    inline auto from_json(
        const nlohmann::ordered_json& nlohmann_json_j, 
        SoundBankInformation& nlohmann_json_t
    ) -> void
    {
        nlohmann_json_j.at("bank_header").get_to(nlohmann_json_t.bank_header);
        if (nlohmann_json_j["initialization"].is_null()) {
            nlohmann_json_t.has_initialization = false;
        }
        else {
            nlohmann_json_t.has_initialization = true;
            nlohmann_json_j.at("initialization").get_to(nlohmann_json_t.initialization);
        }
        if (nlohmann_json_j["game_synchronization"].is_null()) {
            nlohmann_json_t.has_game_synchronization = false;
        }
        else {
            nlohmann_json_t.has_game_synchronization = true;
            nlohmann_json_j.at("game_synchronization").get_to(nlohmann_json_t.game_synchronization);
        }
        if (nlohmann_json_j["embedded_media"].is_null()) {
            nlohmann_json_t.has_embedded_media = false;
        }
        else {
            nlohmann_json_t.has_embedded_media = true;
            nlohmann_json_j.at("embedded_media").get_to(nlohmann_json_t.embedded_media);
        }
        if (nlohmann_json_j["hierarchy"].is_null()) {
            nlohmann_json_t.has_hierarchy = false;
        }
        else {
            nlohmann_json_t.has_hierarchy = true;
            nlohmann_json_j.at("hierarchy").get_to(nlohmann_json_t.hierarchy);
        }
        if (nlohmann_json_j["environments"].is_null()) {
            nlohmann_json_t.has_environments = false;
        }
        else {
            nlohmann_json_t.has_environments = true;
            nlohmann_json_j.at("environments").get_to(nlohmann_json_t.environments);
        }
        if (nlohmann_json_j["reference"].is_null()) {
            nlohmann_json_t.has_reference = false;
        }
        else {
            nlohmann_json_t.has_reference = true;
            nlohmann_json_j.at("reference").get_to(nlohmann_json_t.reference);
        }
        if (nlohmann_json_j["platform_setting"].is_null()) {
            nlohmann_json_t.has_platform_setting = false;
        }
        else {
            nlohmann_json_t.has_platform_setting = true;
            nlohmann_json_j.at("platform_setting").get_to(nlohmann_json_t.platform_setting);
        }
        return;
    }

    #pragma endregion
}