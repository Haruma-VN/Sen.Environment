#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{

    using namespace Sen::Kernel::Definition;

    struct HeadValue
    {
        uint32_t values;
        uint32_t project_id;
        uint32_t soundbank_type = 0;
        std::string bank_hash;
        uint32_t padding = 0;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const HeadValue &nlohmann_json_t) -> void
    {
        nlohmann_json_j["values"] = nlohmann_json_t.values;
        nlohmann_json_j["project_id"] = nlohmann_json_t.project_id;
        if (!nlohmann_json_t.soundbank_type != 0)
        {
            nlohmann_json_j["soundbank_type"] = nlohmann_json_t.soundbank_type;
        }
        if (!nlohmann_json_t.bank_hash.empty())
        {
            nlohmann_json_j["bank_hash"] = nlohmann_json_t.bank_hash;
        }
        nlohmann_json_j["padding"] = nlohmann_json_t.padding;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        HeadValue &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("values").get_to(nlohmann_json_t.values);
        nlohmann_json_j.at("project_id").get_to(nlohmann_json_t.project_id);
        try
        {
            nlohmann_json_j.at("soundbank_type").get_to(nlohmann_json_t.soundbank_type);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("bank_hash").get_to(nlohmann_json_t.bank_hash);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        nlohmann_json_j.at("padding").get_to(nlohmann_json_t.padding);
        return;
    }

    struct BKHD
    {
        uint32_t version;
        uint32_t soundbank_id;
        uint32_t language_id;
        HeadValue head_info;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BKHD &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["soundbank_id"] = nlohmann_json_t.soundbank_id;
        nlohmann_json_j["language_id"] = nlohmann_json_t.language_id;
        nlohmann_json_j["head_info"] = nlohmann_json_t.head_info;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BKHD &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("soundbank_id").get_to(nlohmann_json_t.soundbank_id);
        nlohmann_json_j.at("language_id").get_to(nlohmann_json_t.language_id);
        nlohmann_json_j.at("head_info").get_to(nlohmann_json_t.head_info);
        return;
    }

    struct INIT
    {
        uint32_t id;
        std::string name;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const INIT &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        INIT &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        return;
    }

    struct StateTransition
    {
        uint32_t state_from;
        uint32_t state_to;
        uint32_t transition_time;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StateTransition &nlohmann_json_t) -> void
    {
        nlohmann_json_j["state_from"] = nlohmann_json_t.state_from;
        nlohmann_json_j["state_to"] = nlohmann_json_t.state_to;
        nlohmann_json_j["transition_time"] = nlohmann_json_t.transition_time;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StateTransition &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("state_from").get_to(nlohmann_json_t.state_from);
        nlohmann_json_j.at("state_to").get_to(nlohmann_json_t.state_to);
        nlohmann_json_j.at("transition_time").get_to(nlohmann_json_t.transition_time);
        return;
    }

    struct STMGStageGroupData
    {
        uint32_t default_transition_time;
        std::vector<StateTransition> state_transition;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGStageGroupData &nlohmann_json_t) -> void
    {
        nlohmann_json_j["default_transition_time"] = nlohmann_json_t.default_transition_time;
        nlohmann_json_j["state_transition"] = nlohmann_json_t.state_transition;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGStageGroupData &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("default_transition_time").get_to(nlohmann_json_t.default_transition_time);
        nlohmann_json_j.at("state_transition").get_to(nlohmann_json_t.state_transition);
        return;
    }

    struct STMGStageGroup
    {
        uint32_t id;
        std::string name;
        STMGStageGroupData data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGStageGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (!nlohmann_json_t.name.empty())
        {
            nlohmann_json_j["#name"] = nlohmann_json_t.name;
        }
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGStageGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct GraphPoint
    {
        float from;
        float to;
        uint32_t interp;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GraphPoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j["from"] = nlohmann_json_t.from;
        nlohmann_json_j["to"] = nlohmann_json_t.to;
        nlohmann_json_j["interp"] = nlohmann_json_t.interp;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GraphPoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("from").get_to(nlohmann_json_t.from);
        nlohmann_json_j.at("to").get_to(nlohmann_json_t.to);
        nlohmann_json_j.at("interp").get_to(nlohmann_json_t.interp);
        return;
    }

    struct STMGSwitchGroupData
    {
        uint32_t rtpc_id;
        uint8_t rtpc_type;
        std::vector<GraphPoint> graph_point;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGSwitchGroupData &nlohmann_json_t) -> void
    {
        nlohmann_json_j["rtpc_id"] = nlohmann_json_t.rtpc_id;
        nlohmann_json_j["rtpc_type"] = nlohmann_json_t.rtpc_type;
        nlohmann_json_j["graph_point"] = nlohmann_json_t.graph_point;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGSwitchGroupData &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("rtpc_id").get_to(nlohmann_json_t.rtpc_id);
        nlohmann_json_j.at("rtpc_type").get_to(nlohmann_json_t.rtpc_type);
        nlohmann_json_j.at("graph_point").get_to(nlohmann_json_t.graph_point);
        return;
    }

    struct STMGSwitchGroup
    {
        uint32_t id;
        std::string name;
        STMGSwitchGroupData data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGSwitchGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (!nlohmann_json_t.name.empty())
        {
            nlohmann_json_j["#name"] = nlohmann_json_t.name;
        }
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGSwitchGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct STMGGameParameterData
    {
        float value;
        uint32_t ramp_type = 0;
        float ramp_up = 0;
        float ramp_down = 0;
        uint8_t built_in_param = 0;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGGameParameterData &nlohmann_json_t) -> void
    {
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        nlohmann_json_j["ramp_type"] = nlohmann_json_t.ramp_type;
        nlohmann_json_j["ramp_up"] = nlohmann_json_t.ramp_up;
        nlohmann_json_j["ramp_down"] = nlohmann_json_t.ramp_down;
        nlohmann_json_j["built_in_param"] = nlohmann_json_t.built_in_param;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGGameParameterData &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        nlohmann_json_j.at("ramp_type").get_to(nlohmann_json_t.ramp_type);
        nlohmann_json_j.at("ramp_up").get_to(nlohmann_json_t.ramp_up);
        nlohmann_json_j.at("ramp_down").get_to(nlohmann_json_t.ramp_down);
        nlohmann_json_j.at("built_in_param").get_to(nlohmann_json_t.built_in_param);
        return;
    }

    struct STMGGameParameter
    {
        uint32_t id;
        std::string name;
        STMGGameParameterData data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGGameParameter &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (!nlohmann_json_t.name.empty())
        {
            nlohmann_json_j["#name"] = nlohmann_json_t.name;
        }
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGGameParameter &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct STMGAcousticTextureData
    {
        std::array<uint16_t, 3> on_off_band;
        std::array<uint16_t, 3> filter_type_band;
        std::array<float, 3> frequecy_band;
        std::array<float, 3> factor_band;
        std::array<float, 3> gain_band;
        float output_gain;
        float absorption_offset = 0;
        float absorption_low = 0;
        float absorption_mid_low;
        float absorption_mid_high;
        float absorption_high;
        float scattering;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGAcousticTextureData &nlohmann_json_t) -> void
    {
        nlohmann_json_j["on_off_band"] = nlohmann_json_t.on_off_band;
        nlohmann_json_j["filter_type_band"] = nlohmann_json_t.filter_type_band;
        nlohmann_json_j["frequecy_band"] = nlohmann_json_t.frequecy_band;
        nlohmann_json_j["factor_band"] = nlohmann_json_t.factor_band;
        nlohmann_json_j["gain_band"] = nlohmann_json_t.gain_band;
        nlohmann_json_j["output_gain"] = nlohmann_json_t.output_gain;
        nlohmann_json_j["absorption_offset"] = nlohmann_json_t.absorption_offset;
        nlohmann_json_j["absorption_low"] = nlohmann_json_t.absorption_low;
        nlohmann_json_j["absorption_mid_low"] = nlohmann_json_t.absorption_mid_low;
        nlohmann_json_j["absorption_mid_high"] = nlohmann_json_t.absorption_mid_high;
        nlohmann_json_j["absorption_high"] = nlohmann_json_t.absorption_high;
        nlohmann_json_j["scattering"] = nlohmann_json_t.scattering;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGAcousticTextureData &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("on_off_band").get_to(nlohmann_json_t.on_off_band);
        nlohmann_json_j.at("filter_type_band").get_to(nlohmann_json_t.filter_type_band);
        nlohmann_json_j.at("frequecy_band").get_to(nlohmann_json_t.frequecy_band);
        nlohmann_json_j.at("factor_band").get_to(nlohmann_json_t.factor_band);
        nlohmann_json_j.at("gain_band").get_to(nlohmann_json_t.gain_band);
        nlohmann_json_j.at("output_gain").get_to(nlohmann_json_t.output_gain);
        nlohmann_json_j.at("absorption_offset").get_to(nlohmann_json_t.absorption_offset);
        nlohmann_json_j.at("absorption_low").get_to(nlohmann_json_t.absorption_low);
        nlohmann_json_j.at("absorption_mid_low").get_to(nlohmann_json_t.absorption_mid_low);
        nlohmann_json_j.at("absorption_mid_high").get_to(nlohmann_json_t.absorption_mid_high);
        nlohmann_json_j.at("absorption_high").get_to(nlohmann_json_t.absorption_high);
        nlohmann_json_j.at("scattering").get_to(nlohmann_json_t.scattering);
        return;
    }

    struct STMGAcousticTexture
    {
        uint32_t id;
        std::string name;
        STMGAcousticTextureData data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGAcousticTexture &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (!nlohmann_json_t.name.empty())
        {
            nlohmann_json_j["#name"] = nlohmann_json_t.name;
        }
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGAcousticTexture &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct STMGDiffuseReverberatorData
    {
        float time;
        float hf_ratio;
        float dry_level;
        float wet_level;
        float spread;
        float density;
        float quality;
        float diffusion;
        float scale;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGDiffuseReverberatorData &nlohmann_json_t) -> void
    {
        nlohmann_json_j["time"] = nlohmann_json_t.time;
        nlohmann_json_j["hf_ratio"] = nlohmann_json_t.hf_ratio;
        nlohmann_json_j["dry_level"] = nlohmann_json_t.dry_level;
        nlohmann_json_j["wet_level"] = nlohmann_json_t.wet_level;
        nlohmann_json_j["spread"] = nlohmann_json_t.spread;
        nlohmann_json_j["density"] = nlohmann_json_t.density;
        nlohmann_json_j["diffusion"] = nlohmann_json_t.diffusion;
        nlohmann_json_j["scale"] = nlohmann_json_t.scale;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGDiffuseReverberatorData &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("time").get_to(nlohmann_json_t.time);
        nlohmann_json_j.at("hf_ratio").get_to(nlohmann_json_t.hf_ratio);
        nlohmann_json_j.at("dry_level").get_to(nlohmann_json_t.dry_level);
        nlohmann_json_j.at("wet_level").get_to(nlohmann_json_t.wet_level);
        nlohmann_json_j.at("spread").get_to(nlohmann_json_t.spread);
        nlohmann_json_j.at("density").get_to(nlohmann_json_t.density);
        nlohmann_json_j.at("diffusion").get_to(nlohmann_json_t.diffusion);
        nlohmann_json_j.at("scale").get_to(nlohmann_json_t.scale);
        return;
    }

    struct STMGDiffuseReverberator
    {
        uint32_t id;
        std::string name;
        STMGDiffuseReverberatorData data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMGDiffuseReverberator &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (!nlohmann_json_t.name.empty())
        {
            nlohmann_json_j["#name"] = nlohmann_json_t.name;
        }
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMGDiffuseReverberator &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct STMG
    {
        uint16_t filter_behavior = 0;
        float volume_threshold;
        uint16_t max_num_voices_limit_internal = 0;
        uint16_t max_num_dangerous_virt_voices_limit_internal = 0;
        std::vector<STMGStageGroup> state_group;
        std::vector<STMGSwitchGroup> switch_group;
        std::vector<STMGGameParameter> game_parameter;
        std::vector<STMGAcousticTexture> acoustic_texture;
        std::vector<STMGDiffuseReverberator> diffuse_reverberator;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STMG &nlohmann_json_t) -> void
    {
        if (nlohmann_json_t.filter_behavior != 0)
        {
            nlohmann_json_j["filter_behavior"] = nlohmann_json_t.filter_behavior;
        }
        nlohmann_json_j["volume_threshold"] = nlohmann_json_t.volume_threshold;
        if (nlohmann_json_t.max_num_voices_limit_internal != 0)
        {
            nlohmann_json_j["max_num_voices_limit_internal"] = nlohmann_json_t.max_num_voices_limit_internal;
        }
        if (nlohmann_json_t.max_num_dangerous_virt_voices_limit_internal != 0)
        {
            nlohmann_json_j["max_num_dangerous_virt_voices_limit_internal"] = nlohmann_json_t.max_num_dangerous_virt_voices_limit_internal;
        }
        nlohmann_json_j["state_group"] = nlohmann_json_t.state_group;
        nlohmann_json_j["switch_group"] = nlohmann_json_t.switch_group;
        nlohmann_json_j["game_parameter"] = nlohmann_json_t.game_parameter;
        nlohmann_json_j["acoustic_texture"] = nlohmann_json_t.acoustic_texture;
        nlohmann_json_j["diffuse_reverberator"] = nlohmann_json_t.diffuse_reverberator;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STMG &nlohmann_json_t) -> void
    {
        try
        {
            nlohmann_json_j.at("filter_behavior").get_to(nlohmann_json_t.filter_behavior);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        nlohmann_json_j.at("volume_threshold").get_to(nlohmann_json_t.volume_threshold);
        try
        {
            nlohmann_json_j.at("max_num_voices_limit_internal").get_to(nlohmann_json_t.max_num_voices_limit_internal);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("max_num_dangerous_virt_voices_limit_internal").get_to(nlohmann_json_t.max_num_dangerous_virt_voices_limit_internal);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        nlohmann_json_j.at("state_group").get_to(nlohmann_json_t.state_group);
        nlohmann_json_j.at("switch_group").get_to(nlohmann_json_t.switch_group);
        nlohmann_json_j.at("game_parameter").get_to(nlohmann_json_t.game_parameter);
        nlohmann_json_j.at("acoustic_texture").get_to(nlohmann_json_t.acoustic_texture);
        nlohmann_json_j.at("diffuse_reverberator").get_to(nlohmann_json_t.diffuse_reverberator);
        return;
    }

    /*
    struct DIDX
    {
        uint32_t id;
        std::string name;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const DIDX &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (!nlohmann_json_t.name.empty()) {
            nlohmann_json_j["#name"] = nlohmann_json_t.name;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        DIDX &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        return;
    }
    */
    struct DATA
    {
        uint32_t pos;
        uint32_t size;
    };

    struct HIRC
    {
        uint32_t id;
        std::string name;
        std::string type;
        std::string data;
        std::vector<uint32_t> child;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const HIRC &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (!nlohmann_json_t.name.empty())
        {
            nlohmann_json_j["#name"] = nlohmann_json_t.name;
        }
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        if (nlohmann_json_t.type == "event")
        {
            nlohmann_json_j["child"] = nlohmann_json_t.child;
        }
        else
        {
            nlohmann_json_j["data"] = nlohmann_json_t.data;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        HIRC &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        if (nlohmann_json_t.type == "event")
        {
            nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        }
        else
        {
            nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        }
        return;
    }

    struct ENVSData
    {
        bool curve_enabled;
        uint8_t curve_scaling;
        std::vector<GraphPoint> graph_point;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ENVSData &nlohmann_json_t) -> void
    {
        nlohmann_json_j["curve_enabled"] = nlohmann_json_t.curve_enabled;
        nlohmann_json_j["curve_scaling"] = nlohmann_json_t.curve_scaling;
        nlohmann_json_j["graph_point"] = nlohmann_json_t.graph_point;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ENVSData &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("curve_enabled").get_to(nlohmann_json_t.curve_enabled);
        nlohmann_json_j.at("curve_scaling").get_to(nlohmann_json_t.curve_scaling);
        nlohmann_json_j.at("graph_point").get_to(nlohmann_json_t.graph_point);
        return;
    }

    struct ENVSItem
    {
        ENVSData volume;
        ENVSData low_pass_filter;
        ENVSData high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ENVSItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
        nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ENVSItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
        nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        return;
    }

    struct ENVS
    {
        ENVSItem obstruction;
        ENVSItem occlusion;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ENVS &nlohmann_json_t) -> void
    {
        nlohmann_json_j["obstruction"] = nlohmann_json_t.obstruction;
        nlohmann_json_j["occlusion"] = nlohmann_json_t.occlusion;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ENVS &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("obstruction").get_to(nlohmann_json_t.obstruction);
        nlohmann_json_j.at("occlusion").get_to(nlohmann_json_t.occlusion);
        return;
    }

    struct STIDDATA
    {
        uint32_t id;
        std::string name;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STIDDATA &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STIDDATA &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        return;
    }

    struct STID
    {
        uint32_t type;
        std::vector<STIDDATA> data;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const STID &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["data"] = nlohmann_json_t.data;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        STID &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        nlohmann_json_j.at("data").get_to(nlohmann_json_t.data);
        return;
    }

    struct PLAT
    {
        std::string platform;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PLAT &nlohmann_json_t) -> void
    {
        nlohmann_json_j["platform"] = nlohmann_json_t.platform;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PLAT &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("platform").get_to(nlohmann_json_t.platform);
        return;
    }

    struct SoundBankInformation
    {
        BKHD bank_header;
        std::vector<INIT> initialization;
        STMG game_synchronization;
        std::vector<uint32_t> embedded_media;
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
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBankInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["bank_header"] = nlohmann_json_t.bank_header;
#define sound_bank_has_or(check, _property, __property, or_value) \
    if (nlohmann_json_t.check)                                    \
    {                                                             \
        nlohmann_json_j[_property] = nlohmann_json_t.__property;  \
    }                                                             \
    else                                                          \
    {                                                             \
        nlohmann_json_j[_property] = or_value;                    \
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
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBankInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("bank_header").get_to(nlohmann_json_t.bank_header);
        if (nlohmann_json_j["initialization"].is_null())
        {
            nlohmann_json_t.has_initialization = false;
        }
        else
        {
            nlohmann_json_t.has_initialization = true;
            nlohmann_json_j.at("initialization").get_to(nlohmann_json_t.initialization);
        }
        if (nlohmann_json_j["game_synchronization"].is_null())
        {
            nlohmann_json_t.has_game_synchronization = false;
        }
        else
        {
            nlohmann_json_t.has_game_synchronization = true;
            nlohmann_json_j.at("game_synchronization").get_to(nlohmann_json_t.game_synchronization);
        }
        if (nlohmann_json_j["embedded_media"].is_null())
        {
            nlohmann_json_t.has_embedded_media = false;
        }
        else
        {
            nlohmann_json_t.has_embedded_media = true;
            nlohmann_json_j.at("embedded_media").get_to(nlohmann_json_t.embedded_media);
        }
        if (nlohmann_json_j["hierarchy"].is_null())
        {
            nlohmann_json_t.has_hierarchy = false;
        }
        else
        {
            nlohmann_json_t.has_hierarchy = true;
            nlohmann_json_j.at("hierarchy").get_to(nlohmann_json_t.hierarchy);
        }
        if (nlohmann_json_j["environments"].is_null())
        {
            nlohmann_json_t.has_environments = false;
        }
        else
        {
            nlohmann_json_t.has_environments = true;
            nlohmann_json_j.at("environments").get_to(nlohmann_json_t.environments);
        }
        if (nlohmann_json_j["reference"].is_null())
        {
            nlohmann_json_t.has_reference = false;
        }
        else
        {
            nlohmann_json_t.has_reference = true;
            nlohmann_json_j.at("reference").get_to(nlohmann_json_t.reference);
        }
        if (nlohmann_json_j["platform_setting"].is_null())
        {
            nlohmann_json_t.has_platform_setting = false;
        }
        else
        {
            nlohmann_json_t.has_platform_setting = true;
            nlohmann_json_j.at("platform_setting").get_to(nlohmann_json_t.platform_setting);
        }
        return;
    }

}