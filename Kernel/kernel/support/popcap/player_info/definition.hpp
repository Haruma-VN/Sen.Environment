#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::PlayerInfo
{

    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    inline static auto k_version = 0;

    enum class SeedType : uint8_t
    {
        seed_peashooter = 0,
        seed_sunflower = 1,
        seed_cherrybomb = 2,
        seed_wallnut = 3,
        seed_potatomine = 4,
        seed_snowpea = 5,
        seed_chomper = 6,
        seed_repeater = 7,
        seed_puffshroom = 8,
        seed_sunshroom = 9,
        seed_fumeshroom = 10,
        seed_gravebuster = 11,
        seed_hypnoshroom = 12,
        seed_scaredyshroom = 13,
        seed_iceshroom = 14,
        seed_doomshroom = 15,
        seed_lilypad = 16,
        seed_squash = 17,
        seed_threepeater = 18,
        seed_tanglekelp = 19,
        seed_jalapeno = 20,
        seed_spikeweed = 21,
        seed_torchwood = 22,
        seed_tallnut = 23,
        seed_seashroom = 24,
        seed_plantern = 25,
        seed_cactus = 26,
        seed_blover = 27,
        seed_splitpea = 28,
        seed_starfruit = 29,
        seed_pumpkinshell = 30,
        seed_magnetshroom = 31,
        seed_cabbagepult = 32,
        seed_flowerpot = 33,
        seed_kernelpult = 34,
        seed_instant_coffee = 35,
        seed_garlic = 36,
        seed_umbrella = 37,
        seed_marigold = 38,
        seed_melonpult = 39,
        seed_gatlingpea = 40,
        seed_twinsunflower = 41,
        seed_gloomshroom = 42,
        seed_cattail = 43,
        seed_wintermelon = 44,
        seed_gold_magnet = 45,
        seed_spikerock = 46,
        seed_cobcannon = 47,
        seed_imitater = 48,
        seed_explode_o_nut,
        seed_giant_wallnut,
        seed_sprout,
        seed_leftpeater,
        num_seed_types,
        seed_beghouled_button_shuffle,
        seed_beghouled_button_crater,
        seed_slot_machine_sun,
        seed_slot_machine_diamond,
        seed_zombiquarium_snorkle,
        seed_zombiquarium_trophy,
        seed_zombie_normal,
        seed_zombie_traffic_cone,
        seed_zombie_polevaulter,
        seed_zombie_pail,
        seed_zombie_ladder,
        seed_zombie_digger,
        seed_zombie_bungee,
        seed_zombie_football,
        seed_zombie_balloon,
        seed_zombie_screen_door,
        seed_zomboni,
        seed_zombie_pogo,
        seed_zombie_dancer,
        seed_zombie_gargantuar,
        seed_zombie_imp,
        num_seeds_in_chooser = 49,
        seed_none = 255
    };

    enum class GerdenType : uint8_t
    {
        zen_garden,
        mushroom_garden,
        wheel_barrow,
        aquarium_garden
    };

    enum class DrawVariation : uint8_t
    {
        normal,
        imitater,
        marigold_white,
        marigold_magenta,
        marigold_orange,
        marigold_pink,
        marigold_light_blue,
        marigold_red,
        marigold_blue,
        marigold_violet,
        marigold_lavender,
        marigold_yellow,
        marigold_light_green,
        zen_garden,
        zen_garden_water,
        sprout_no_flower,
        imitater_less,
        aquarium
    };

    enum class PottedPlantAge : uint8_t
    {
        sprout = 0,
        small = 1,
        medium = 2,
        full = 3
    };

    enum class PottedPlantNeed : uint8_t
    {
        none = 0,
        water = 1,
        fertilizer = 2,
        bugspray = 3,
        phonograph = 4
    };

    enum class StoreItem : uint8_t
    {
        plant_gatlingpea,
        plant_twinsunflower,
        plant_gloomshroom,
        plant_cattail,
        plant_wintermelon,
        plant_gold_magnet,
        plant_spikerock,
        plant_cobcannon,
        plant_imitater,
        bonus_lawn_mower,
        potted_marigold_1,
        potted_marigold_2,
        potted_marigold_3,
        gold_wateringcan,
        fertilizer,
        bug_spray,
        phonograph,
        gardening_glove,
        mushroom_garden,
        wheel_barrow,
        stinky_the_snail,
        packet_upgrade,
        pool_cleaner,
        roof_cleaner,
        rake,
        aquarium_garden,
        chocolate,
        tree_of_wisdom,
        tree_food,
        firstaid,
        pvz,
        basic_giftbox,
        bronze_giftbox,
        silver_giftbox,
        gold_giftbox,
        zombotany,
        minigame_pack_1,
        minigame_pack_2,
        minigame_pack_3,
        minigame_pack_4,
        minigame_pack_5,
        i_zombie_pack,
        scary_potter_pack,
        bacon,
        bacon_2,
        last_stand_pack,
        hammer
    };

    enum class FacingDirection : uint8_t
    {
        facing_right,
        facing_left
    };

    enum class GameMode : uint8_t
    {
        adventure,
        survival_normal_stage_1,
        survival_normal_stage_2,
        survival_normal_stage_3,
        survival_normal_stage_4,
        survival_normal_stage_5,
        survival_hard_stage_1,
        survival_hard_stage_2,
        survival_hard_stage_3,
        survival_hard_stage_4,
        survival_hard_stage_5,
        survival_endless_stage_1,
        survival_endless_stage_2,
        survival_endless_stage_3,
        survival_endless_stage_4,
        survival_endless_stage_5,
        challenge_war_and_peas,
        challenge_wallnut_bowling,
        challenge_slot_machine,
        challenge_raining_seeds,
        challenge_beghouled,
        challenge_invisighoul,
        challenge_seeing_stars,
        challenge_zombiquarium,
        challenge_beghouled_twist,
        challenge_little_trouble,
        challenge_portal_combat,
        challenge_column,
        challenge_bobsled_bonanza,
        challenge_speed,
        challenge_whack_a_zombie,
        challenge_last_stand,
        challenge_war_and_peas_2,
        challenge_wallnut_bowling_2,
        challenge_pogo_party,
        challenge_final_boss,
        challenge_art_challenge_wallnut,
        challenge_sunny_day,
        challenge_resodded,
        challenge_big_time,
        challenge_art_challenge_sunflower,
        challenge_air_raid,
        challenge_ice,
        challenge_zen_garden,
        challenge_high_gravity,
        challenge_grave_danger,
        challenge_shovel,
        challenge_stormy_night,
        challenge_bungee_blitz,
        challenge_squirrel,
        tree_of_wisdom,
        vasebreaker,
        scary_potter_1,
        scary_potter_2,
        scary_potter_3,
        scary_potter_4,
        scary_potter_5,
        scary_potter_6,
        scary_potter_7,
        scary_potter_8,
        scary_potter_9,
        scary_potter_endless,
        puzzle_i_zombie_1,
        puzzle_i_zombie_2,
        puzzle_i_zombie_3,
        puzzle_i_zombie_4,
        puzzle_i_zombie_5,
        puzzle_i_zombie_6,
        puzzle_i_zombie_7,
        puzzle_i_zombie_8,
        puzzle_i_zombie_9,
        puzzle_i_zombie_endless,
        upsell,
        intro,
        last_stand_stage_1,
        last_stand_stage_2,
        last_stand_stage_3,
        last_stand_stage_4,
        last_stand_stage_5,
        last_stand_endless,
        unknown
    };

    enum class AchievementId : uint8_t
    {
        home_security,
        nobel_peas_prize,
        better_off_dead,
        china_shop,
        spudow,
        explodonator,
        morticulturalist,
        dont_pea,
        roll_some_heads,
        grounded,
        zombologist,
        penny_pincher,
        sunny_days,
        popcorn_party,
        good_morning,
        no_fungus_amongus,
        beyond_the_grave,
        immortal,
        towering_wisdom,
        mustache_mode,
        last_mown_stading,
        below_zero,
        flower_power,
        pyromaniac,
        lawn_mowner_man,
        chill_out,
        defcorn_five,
        monster_mash,
        blind_faith,
        pool_closed,
        melony_lane,
        second_life,
        lucky_spin,
        chilli_free,
        enlightened,
        diamond_beghouler,
        greenfingers,
        wallnot_attack,
        peking_express,
        shooting_star,
        sol_invictus,
        sproing_sproing,
        i_win,
        where_the_sun_dont_shine,
        undead_space,
        disco_is_undead,
        smashing,
        gotcha,
        still_standing,
        the_stuff_of_legends,
        hammer_time
    };

    struct MiniGameRecord
    {
        bool zombotany;
        bool wallnull_bowling;
        bool slot_machine;
        bool raining_seeds;
        bool beghouled;
        bool invisi_ghoul;
        bool seeing_stars;
        bool zombiquarium;
        bool beghouled_twist;
        bool little_trouble;
        bool portal_combat;
        bool column_like_see_you_em;
        bool bobsled_bonanza;
        bool zombie_nimble_zombie_quick;
        bool whack_a_zombie;
        int last_stand;
        bool zombotany_2;
        bool wallnut_bowling_2;
        bool pogo_party;
        bool dr_zomboss_revenge;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MiniGameRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j["zombotany"] = nlohmann_json_t.zombotany;
        if (k_version < 11)
        {
            nlohmann_json_j["beghouled"] = nlohmann_json_t.beghouled;
        }
        else
        {
            if (k_version < 30) {
                nlohmann_json_j["wallnull_bowling"] = nlohmann_json_t.wallnull_bowling;
            }
        }
        nlohmann_json_j["slot_machine"] = nlohmann_json_t.slot_machine;
        nlohmann_json_j["raining_seeds"] = nlohmann_json_t.raining_seeds;
        if (k_version < 11)
        {
            nlohmann_json_j["wallnull_bowling"] = nlohmann_json_t.wallnull_bowling;
        }
        else
        {
            nlohmann_json_j["beghouled"] = nlohmann_json_t.beghouled;
        }
        nlohmann_json_j["invisi_ghoul"] = nlohmann_json_t.invisi_ghoul;
        nlohmann_json_j["seeing_stars"] = nlohmann_json_t.seeing_stars;
        nlohmann_json_j["zombiquarium"] = nlohmann_json_t.zombiquarium;
        nlohmann_json_j["beghouled_twist"] = nlohmann_json_t.beghouled_twist;
        nlohmann_json_j["little_trouble"] = nlohmann_json_t.little_trouble;
        nlohmann_json_j["portal_combat"] = nlohmann_json_t.portal_combat;
        nlohmann_json_j["column_like_see_you_em"] = nlohmann_json_t.column_like_see_you_em;
        nlohmann_json_j["bobsled_bonanza"] = nlohmann_json_t.bobsled_bonanza;
        nlohmann_json_j["zombie_nimble_zombie_quick"] = nlohmann_json_t.zombie_nimble_zombie_quick;
        if (k_version < 30) {
            nlohmann_json_j["whack_a_zombie"] = nlohmann_json_t.whack_a_zombie;
        }   
        if (k_version < 11)
        {
            nlohmann_json_j["last_stand"] = nlohmann_json_t.last_stand;
        }
        else
        {
            nlohmann_json_j["last_stand"] = static_cast<bool>(nlohmann_json_t.last_stand);
        }
        nlohmann_json_j["zombotany_2"] = nlohmann_json_t.zombotany_2;
        nlohmann_json_j["wallnut_bowling_2"] = nlohmann_json_t.wallnut_bowling_2;
        nlohmann_json_j["pogo_party"] = nlohmann_json_t.pogo_party;
        if (k_version < 30) {
            nlohmann_json_j["dr_zomboss_revenge"] = nlohmann_json_t.dr_zomboss_revenge;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MiniGameRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("zombotany").get_to(nlohmann_json_t.zombotany);
        if (k_version < 11)
        {
            nlohmann_json_j.at("beghouled").get_to(nlohmann_json_t.beghouled);
        }
        else
        {
            if (k_version < 30) {
                nlohmann_json_j.at("wallnull_bowling").get_to(nlohmann_json_t.wallnull_bowling);
            }
        }
        nlohmann_json_j.at("slot_machine").get_to(nlohmann_json_t.slot_machine);
        nlohmann_json_j.at("raining_seeds").get_to(nlohmann_json_t.raining_seeds);
        if (k_version < 11)
        {
            nlohmann_json_j.at("wallnull_bowling").get_to(nlohmann_json_t.wallnull_bowling);
        }
        else
        {
            nlohmann_json_j.at("beghouled").get_to(nlohmann_json_t.beghouled);
        }
        nlohmann_json_j.at("invisi_ghoul").get_to(nlohmann_json_t.invisi_ghoul);
        nlohmann_json_j.at("seeing_stars").get_to(nlohmann_json_t.seeing_stars);
        nlohmann_json_j.at("zombiquarium").get_to(nlohmann_json_t.zombiquarium);
        nlohmann_json_j.at("beghouled_twist").get_to(nlohmann_json_t.beghouled_twist);
        nlohmann_json_j.at("little_trouble").get_to(nlohmann_json_t.little_trouble);
        nlohmann_json_j.at("portal_combat").get_to(nlohmann_json_t.portal_combat);
        nlohmann_json_j.at("column_like_see_you_em").get_to(nlohmann_json_t.column_like_see_you_em);
        nlohmann_json_j.at("bobsled_bonanza").get_to(nlohmann_json_t.bobsled_bonanza);
        nlohmann_json_j.at("zombie_nimble_zombie_quick").get_to(nlohmann_json_t.zombie_nimble_zombie_quick);
        if (k_version < 30) {
            nlohmann_json_j.at("whack_a_zombie").get_to(nlohmann_json_t.whack_a_zombie);
        }
        nlohmann_json_j.at("last_stand").get_to(nlohmann_json_t.last_stand);
        nlohmann_json_j.at("zombotany_2").get_to(nlohmann_json_t.zombotany_2);
        nlohmann_json_j.at("wallnut_bowling_2").get_to(nlohmann_json_t.wallnut_bowling_2);
        nlohmann_json_j.at("pogo_party").get_to(nlohmann_json_t.pogo_party);
        if (k_version < 30) {
            nlohmann_json_j.at("dr_zomboss_revenge").get_to(nlohmann_json_t.dr_zomboss_revenge);
        }
        return;
    }

    struct LastStandRecord
    {
        int day_flag;
        int night_flag;
        int pool_flag;
        int fog_flag;
        int roof_flag;
        int endless_flag;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const LastStandRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j["day_flag"] = nlohmann_json_t.day_flag;
        nlohmann_json_j["night_flag"] = nlohmann_json_t.night_flag;
        nlohmann_json_j["pool_flag"] = nlohmann_json_t.pool_flag;
        nlohmann_json_j["fog_flag"] = nlohmann_json_t.fog_flag;
        nlohmann_json_j["roof_flag"] = nlohmann_json_t.roof_flag;
        nlohmann_json_j["endless_flag"] = nlohmann_json_t.endless_flag;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        LastStandRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("day_flag").get_to(nlohmann_json_t.day_flag);
        nlohmann_json_j.at("night_flag").get_to(nlohmann_json_t.night_flag);
        nlohmann_json_j.at("pool_flag").get_to(nlohmann_json_t.pool_flag);
        nlohmann_json_j.at("fog_flag").get_to(nlohmann_json_t.fog_flag);
        nlohmann_json_j.at("roof_flag").get_to(nlohmann_json_t.roof_flag);
        nlohmann_json_j.at("endless_flag").get_to(nlohmann_json_t.endless_flag);
        return;
    }

    struct LimboRecord
    {
        bool art_challenge_wallnut;
        bool sunny_day;
        bool unsodded;
        bool big_time;
        bool art_challenge_sunflower;
        bool air_raid;
        bool ice_level;
        bool zen_garden;
        bool high_gravity;
        bool grave_danger;
        bool can_you_dig_it;
        bool dark_stormy_night;
        bool bungee_blitz;
        bool squirrel;
        bool intro;
        bool tree_of_wisdom;
        bool upsell;
        int survival_day_endless_flag;
        int survival_night_endless_flag;
        int survival_fog_endless_flag;
        int survival_roof_endless_flag;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const LimboRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j["art_challenge_wallnut"] = nlohmann_json_t.art_challenge_wallnut;
        nlohmann_json_j["sunny_day"] = nlohmann_json_t.sunny_day;
        nlohmann_json_j["unsodded"] = nlohmann_json_t.unsodded;
        nlohmann_json_j["big_time"] = nlohmann_json_t.big_time;
        nlohmann_json_j["art_challenge_sunflower"] = nlohmann_json_t.art_challenge_sunflower;
        nlohmann_json_j["air_raid"] = nlohmann_json_t.air_raid;
        nlohmann_json_j["ice_level"] = nlohmann_json_t.ice_level;
        nlohmann_json_j["zen_garden"] = nlohmann_json_t.zen_garden;
        nlohmann_json_j["high_gravity"] = nlohmann_json_t.high_gravity;
        nlohmann_json_j["grave_danger"] = nlohmann_json_t.grave_danger;
        nlohmann_json_j["can_you_dig_it"] = nlohmann_json_t.can_you_dig_it;
        nlohmann_json_j["dark_stormy_night"] = nlohmann_json_t.dark_stormy_night;
        nlohmann_json_j["bungee_blitz"] = nlohmann_json_t.bungee_blitz;
        nlohmann_json_j["squirrel"] = nlohmann_json_t.squirrel;
        if (k_version >= 11 && k_version < 13)
        {
            nlohmann_json_j["intro"] = nlohmann_json_t.intro;
            nlohmann_json_j["tree_of_wisdom"] = nlohmann_json_t.tree_of_wisdom;
            nlohmann_json_j["upsell"] = nlohmann_json_t.upsell;
        }
        nlohmann_json_j["survival_day_endless_flag"] = nlohmann_json_t.survival_day_endless_flag;
        nlohmann_json_j["survival_night_endless_flag"] = nlohmann_json_t.survival_night_endless_flag;
        nlohmann_json_j["survival_fog_endless_flag"] = nlohmann_json_t.survival_fog_endless_flag;
        nlohmann_json_j["survival_roof_endless_flag"] = nlohmann_json_t.survival_roof_endless_flag;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        LimboRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("art_challenge_wallnut").get_to(nlohmann_json_t.art_challenge_wallnut);
        nlohmann_json_j.at("sunny_day").get_to(nlohmann_json_t.sunny_day);
        nlohmann_json_j.at("unsodded").get_to(nlohmann_json_t.unsodded);
        nlohmann_json_j.at("big_time").get_to(nlohmann_json_t.big_time);
        nlohmann_json_j.at("art_challenge_sunflower").get_to(nlohmann_json_t.art_challenge_sunflower);
        nlohmann_json_j.at("air_raid").get_to(nlohmann_json_t.air_raid);
        nlohmann_json_j.at("high_gravity").get_to(nlohmann_json_t.high_gravity);
        nlohmann_json_j.at("grave_danger").get_to(nlohmann_json_t.grave_danger);
        nlohmann_json_j.at("can_you_dig_it").get_to(nlohmann_json_t.can_you_dig_it);
        nlohmann_json_j.at("dark_stormy_night").get_to(nlohmann_json_t.dark_stormy_night);
        nlohmann_json_j.at("bungee_blitz").get_to(nlohmann_json_t.bungee_blitz);
        nlohmann_json_j.at("squirrel").get_to(nlohmann_json_t.squirrel);
        if (k_version >= 11 && k_version < 13)
        {
            nlohmann_json_j.at("intro").get_to(nlohmann_json_t.intro);
            nlohmann_json_j.at("tree_of_wisdom").get_to(nlohmann_json_t.tree_of_wisdom);
            nlohmann_json_j.at("upsell").get_to(nlohmann_json_t.upsell);
        }
        nlohmann_json_j.at("survival_day_endless_flag").get_to(nlohmann_json_t.survival_day_endless_flag);
        nlohmann_json_j.at("survival_night_endless_flag").get_to(nlohmann_json_t.survival_night_endless_flag);
        nlohmann_json_j.at("survival_fog_endless_flag").get_to(nlohmann_json_t.survival_fog_endless_flag);
        nlohmann_json_j.at("survival_roof_endless_flag").get_to(nlohmann_json_t.survival_roof_endless_flag);
        return;
    }

    struct PuzzleRecord
    {
        bool vasebreaker_1;
        bool vasebreaker_2;
        bool vasebreaker_3;
        bool vasebreaker_4;
        bool vasebreaker_5;
        bool vasebreaker_6;
        bool vasebreaker_7;
        bool vasebreaker_8;
        bool vasebreaker_9;
        int vasebreaker_endless_flag;
        bool i_zombie_1;
        bool i_zombie_2;
        bool i_zombie_3;
        bool i_zombie_4;
        bool i_zombie_5;
        bool i_zombie_6;
        bool i_zombie_7;
        bool i_zombie_8;
        bool i_zombie_9;
        int i_zombie_endless_flag;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PuzzleRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j["vasebreaker_1"] = nlohmann_json_t.vasebreaker_1;
        nlohmann_json_j["vasebreaker_2"] = nlohmann_json_t.vasebreaker_2;
        nlohmann_json_j["vasebreaker_3"] = nlohmann_json_t.vasebreaker_3;
        nlohmann_json_j["vasebreaker_4"] = nlohmann_json_t.vasebreaker_4;
        nlohmann_json_j["vasebreaker_5"] = nlohmann_json_t.vasebreaker_5;
        nlohmann_json_j["vasebreaker_6"] = nlohmann_json_t.vasebreaker_6;
        nlohmann_json_j["vasebreaker_7"] = nlohmann_json_t.vasebreaker_7;
        nlohmann_json_j["vasebreaker_8"] = nlohmann_json_t.vasebreaker_8;
        nlohmann_json_j["vasebreaker_9"] = nlohmann_json_t.vasebreaker_9;
        nlohmann_json_j["vasebreaker_endless_flag"] = nlohmann_json_t.vasebreaker_endless_flag;
        nlohmann_json_j["i_zombie_1"] = nlohmann_json_t.i_zombie_1;
        nlohmann_json_j["i_zombie_2"] = nlohmann_json_t.i_zombie_2;
        nlohmann_json_j["i_zombie_3"] = nlohmann_json_t.i_zombie_3;
        nlohmann_json_j["i_zombie_4"] = nlohmann_json_t.i_zombie_4;
        nlohmann_json_j["i_zombie_5"] = nlohmann_json_t.i_zombie_5;
        nlohmann_json_j["i_zombie_6"] = nlohmann_json_t.i_zombie_6;
        nlohmann_json_j["i_zombie_7"] = nlohmann_json_t.i_zombie_7;
        nlohmann_json_j["i_zombie_8"] = nlohmann_json_t.i_zombie_8;
        nlohmann_json_j["i_zombie_9"] = nlohmann_json_t.i_zombie_9;
        nlohmann_json_j["i_zombie_endless_flag"] = nlohmann_json_t.i_zombie_endless_flag;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PuzzleRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("vasebreaker_1").get_to(nlohmann_json_t.vasebreaker_1);
        nlohmann_json_j.at("vasebreaker_2").get_to(nlohmann_json_t.vasebreaker_2);
        nlohmann_json_j.at("vasebreaker_3").get_to(nlohmann_json_t.vasebreaker_3);
        nlohmann_json_j.at("vasebreaker_4").get_to(nlohmann_json_t.vasebreaker_4);
        nlohmann_json_j.at("vasebreaker_5").get_to(nlohmann_json_t.vasebreaker_5);
        nlohmann_json_j.at("vasebreaker_6").get_to(nlohmann_json_t.vasebreaker_6);
        nlohmann_json_j.at("vasebreaker_7").get_to(nlohmann_json_t.vasebreaker_7);
        nlohmann_json_j.at("vasebreaker_8").get_to(nlohmann_json_t.vasebreaker_8);
        nlohmann_json_j.at("vasebreaker_9").get_to(nlohmann_json_t.vasebreaker_9);
        nlohmann_json_j.at("vasebreaker_endless_flag").get_to(nlohmann_json_t.vasebreaker_endless_flag);
        nlohmann_json_j.at("i_zombie_1").get_to(nlohmann_json_t.i_zombie_1);
        nlohmann_json_j.at("i_zombie_2").get_to(nlohmann_json_t.i_zombie_2);
        nlohmann_json_j.at("i_zombie_3").get_to(nlohmann_json_t.i_zombie_3);
        nlohmann_json_j.at("i_zombie_4").get_to(nlohmann_json_t.i_zombie_4);
        nlohmann_json_j.at("i_zombie_5").get_to(nlohmann_json_t.i_zombie_5);
        nlohmann_json_j.at("i_zombie_6").get_to(nlohmann_json_t.i_zombie_6);
        nlohmann_json_j.at("i_zombie_7").get_to(nlohmann_json_t.i_zombie_7);
        nlohmann_json_j.at("i_zombie_8").get_to(nlohmann_json_t.i_zombie_8);
        nlohmann_json_j.at("i_zombie_9").get_to(nlohmann_json_t.i_zombie_9);
        nlohmann_json_j.at("i_zombie_endless_flag").get_to(nlohmann_json_t.i_zombie_endless_flag);
        return;
    }

    struct SurvivalRecord
    {
        int day_normal_flag;
        int night_normal_flag;
        int pool_normal_flag;
        int fog_normal_flag;
        int roof_normal_flag;
        int day_hard_flag;
        int night_hard_flag;
        int pool_hard_flag;
        int fog_hard_flag;
        int roof_hard_flag;
        int survival_endless_flag;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SurvivalRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j["day_normal_flag"] = nlohmann_json_t.day_normal_flag;
        nlohmann_json_j["night_normal_flag"] = nlohmann_json_t.night_normal_flag;
        nlohmann_json_j["pool_normal_flag"] = nlohmann_json_t.pool_normal_flag;
        nlohmann_json_j["fog_normal_flag"] = nlohmann_json_t.fog_normal_flag;
        nlohmann_json_j["roof_normal_flag"] = nlohmann_json_t.roof_normal_flag;
        nlohmann_json_j["day_hard_flag"] = nlohmann_json_t.day_hard_flag;
        nlohmann_json_j["night_hard_flag"] = nlohmann_json_t.night_hard_flag;
        nlohmann_json_j["pool_hard_flag"] = nlohmann_json_t.pool_hard_flag;
        nlohmann_json_j["fog_hard_flag"] = nlohmann_json_t.fog_hard_flag;
        nlohmann_json_j["roof_hard_flag"] = nlohmann_json_t.roof_hard_flag;
        nlohmann_json_j["survival_endless_flag"] = nlohmann_json_t.survival_endless_flag;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SurvivalRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("day_normal_flag").get_to(nlohmann_json_t.day_normal_flag);
        nlohmann_json_j.at("night_normal_flag").get_to(nlohmann_json_t.night_normal_flag);
        nlohmann_json_j.at("pool_normal_flag").get_to(nlohmann_json_t.pool_normal_flag);
        nlohmann_json_j.at("fog_normal_flag").get_to(nlohmann_json_t.fog_normal_flag);
        nlohmann_json_j.at("roof_normal_flag").get_to(nlohmann_json_t.roof_normal_flag);
        nlohmann_json_j.at("day_hard_flag").get_to(nlohmann_json_t.day_hard_flag);
        nlohmann_json_j.at("night_hard_flag").get_to(nlohmann_json_t.night_hard_flag);
        nlohmann_json_j.at("pool_hard_flag").get_to(nlohmann_json_t.pool_hard_flag);
        nlohmann_json_j.at("fog_hard_flag").get_to(nlohmann_json_t.fog_hard_flag);
        nlohmann_json_j.at("roof_hard_flag").get_to(nlohmann_json_t.roof_hard_flag);
        nlohmann_json_j.at("survival_endless_flag").get_to(nlohmann_json_t.survival_endless_flag);
        return;
    }

    struct ChallengeRecord
    {
        MiniGameRecord minigame;
        PuzzleRecord puzzle;
        SurvivalRecord survival;
        LastStandRecord last_stand;
        LimboRecord canceled_minigame;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ChallengeRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j["minigame"] = nlohmann_json_t.minigame;
        nlohmann_json_j["puzzle"] = nlohmann_json_t.puzzle;
        nlohmann_json_j["survival"] = nlohmann_json_t.survival;
        if (k_version >= 30)
        {
            nlohmann_json_j["last_stand"] = nlohmann_json_t.last_stand;
        }
        if (k_version < 13)
        {
            nlohmann_json_j["canceled_minigame"] = nlohmann_json_t.canceled_minigame;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ChallengeRecord &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("minigame").get_to(nlohmann_json_t.minigame);
        nlohmann_json_j.at("puzzle").get_to(nlohmann_json_t.puzzle);
        nlohmann_json_j.at("survival").get_to(nlohmann_json_t.survival);
        if (k_version >= 30)
        {
            nlohmann_json_j.at("last_stand").get_to(nlohmann_json_t.last_stand);
        }
        if (k_version < 13)
        {
            nlohmann_json_j.at("canceled_minigame").get_to(nlohmann_json_t.canceled_minigame);
        }
        return;
    }

    struct Unlocked
    {
        bool minigame;
        bool puzzle_mode;
        bool first_time_play_minigame;
        bool first_time_play_i_zombie;
        bool first_time_play_vasebreaker;
        // bool survival_mode;
        bool first_time_play_survival;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Unlocked &nlohmann_json_t) -> void
    {
        nlohmann_json_j["minigame"] = nlohmann_json_t.minigame;
        nlohmann_json_j["puzzle_mode"] = nlohmann_json_t.puzzle_mode;
        nlohmann_json_j["first_time_play_minigame"] = nlohmann_json_t.first_time_play_minigame;
        nlohmann_json_j["first_time_play_i_zombie"] = nlohmann_json_t.first_time_play_i_zombie;
        nlohmann_json_j["first_time_play_vasebreaker"] = nlohmann_json_t.first_time_play_vasebreaker;
        nlohmann_json_j["first_time_play_survival"] = nlohmann_json_t.first_time_play_survival;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Unlocked &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("minigame").get_to(nlohmann_json_t.minigame);
        nlohmann_json_j.at("puzzle_mode").get_to(nlohmann_json_t.puzzle_mode);
        nlohmann_json_j.at("first_time_play_minigame").get_to(nlohmann_json_t.first_time_play_minigame);
        nlohmann_json_j.at("first_time_play_i_zombie").get_to(nlohmann_json_t.first_time_play_i_zombie);
        nlohmann_json_j.at("first_time_play_vasebreaker").get_to(nlohmann_json_t.first_time_play_vasebreaker);
        nlohmann_json_j.at("first_time_play_survival").get_to(nlohmann_json_t.first_time_play_survival);
        return;
    }

    struct General
    {
        int adventure_level;
        int coins;
        bool adventure_compeled;
        int completed_adventure_time;
        int player_time_active_player;
        int player_time_inactive_player;
        int place_holder_player_stats;
        bool has_magic_taco_reward;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const General &nlohmann_json_t) -> void
    {
        nlohmann_json_j["adventure_level"] = nlohmann_json_t.adventure_level;
        nlohmann_json_j["coins"] = nlohmann_json_t.coins;
        nlohmann_json_j["adventure_compeled"] = nlohmann_json_t.adventure_compeled;
        nlohmann_json_j["completed_adventure_time"] = nlohmann_json_t.completed_adventure_time;
        nlohmann_json_j["player_time_active_player"] = nlohmann_json_t.player_time_active_player;
        nlohmann_json_j["player_time_inactive_player"] = nlohmann_json_t.player_time_inactive_player;
        nlohmann_json_j["place_holder_player_stats"] = nlohmann_json_t.place_holder_player_stats;
        // nlohmann_json_j["need_message_on_game_selector"] = nlohmann_json_t.need_message_on_game_selector;
        nlohmann_json_j["has_magic_taco_reward"] = nlohmann_json_t.has_magic_taco_reward;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        General &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("adventure_level").get_to(nlohmann_json_t.adventure_level);
        nlohmann_json_j.at("coins").get_to(nlohmann_json_t.coins);
        nlohmann_json_j.at("adventure_compeled").get_to(nlohmann_json_t.adventure_compeled);
        nlohmann_json_j.at("completed_adventure_time").get_to(nlohmann_json_t.completed_adventure_time);
        nlohmann_json_j.at("player_time_active_player").get_to(nlohmann_json_t.player_time_active_player);
        nlohmann_json_j.at("player_time_inactive_player").get_to(nlohmann_json_t.player_time_inactive_player);
        nlohmann_json_j.at("place_holder_player_stats").get_to(nlohmann_json_t.place_holder_player_stats);
        nlohmann_json_j.at("has_magic_taco_reward").get_to(nlohmann_json_t.has_magic_taco_reward);
        return;
    }

    struct StinkyInfo
    { 
        bool sleeping;
        int last_awoken;
        int last_chocolate;
        // int has_seen_upsell; // unknown.
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StinkyInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["sleeping"] = nlohmann_json_t.sleeping;
        nlohmann_json_j["last_awoken"] = nlohmann_json_t.last_awoken;
        nlohmann_json_j["last_chocolate"] = nlohmann_json_t.last_chocolate;
        // nlohmann_json_j["has_seen_upsell"] = nlohmann_json_t.has_seen_upsell;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StinkyInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("sleeping").get_to(nlohmann_json_t.sleeping);
        nlohmann_json_j.at("last_awoken").get_to(nlohmann_json_t.last_awoken);
        nlohmann_json_j.at("last_chocolate").get_to(nlohmann_json_t.last_chocolate);
        return;
    }

    struct Position
    {
        int x;
        int y;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Position &nlohmann_json_t) -> void
    {
        nlohmann_json_j["x"] = nlohmann_json_t.x;
        nlohmann_json_j["y"] = nlohmann_json_t.y;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Position &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
        nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
        return;
    }

    struct StinkyTheSnail
    {
        bool purchased;
        StinkyInfo status;
        Position position;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StinkyTheSnail &nlohmann_json_t) -> void
    {
        nlohmann_json_j["purchased"] = nlohmann_json_t.purchased;
        nlohmann_json_j["status"] = nlohmann_json_t.status;
        nlohmann_json_j["position"] = nlohmann_json_t.position;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StinkyTheSnail &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("purchased").get_to(nlohmann_json_t.purchased);
        nlohmann_json_j.at("status").get_to(nlohmann_json_t.status);
        nlohmann_json_j.at("position").get_to(nlohmann_json_t.position);
        return;
    }

    struct PottedPlantTime
    {
        int last_watered_time;
        int last_need_ful_filled_time;
        int last_fertilized_time;
        int last_chocolate_time;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PottedPlantTime &nlohmann_json_t) -> void
    {
        nlohmann_json_j["last_watered_time"] = nlohmann_json_t.last_watered_time;
        nlohmann_json_j["last_need_ful_filled_time"] = nlohmann_json_t.last_need_ful_filled_time;
        nlohmann_json_j["last_fertilized_time"] = nlohmann_json_t.last_fertilized_time;
        nlohmann_json_j["last_chocolate_time"] = nlohmann_json_t.last_chocolate_time;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PottedPlantTime &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("last_watered_time").get_to(nlohmann_json_t.last_watered_time);
        nlohmann_json_j.at("last_need_ful_filled_time").get_to(nlohmann_json_t.last_need_ful_filled_time);
        nlohmann_json_j.at("last_fertilized_time").get_to(nlohmann_json_t.last_fertilized_time);
        nlohmann_json_j.at("last_chocolate_time").get_to(nlohmann_json_t.last_chocolate_time);
        return;
    }

    struct PottedPlantInfo
    {
        FacingDirection orientation;
        DrawVariation color;
        PottedPlantAge plant_age;
        PottedPlantNeed plant_need;
        // int num_of_time_fertilized;
        int num_of_time_watered;
        int feeding_pre_grow;
        PottedPlantTime time;
        // int future_attribute;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PottedPlantInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["orientation"] = magic_enum::enum_name(nlohmann_json_t.orientation);
        nlohmann_json_j["color"] = magic_enum::enum_name(nlohmann_json_t.color);
        nlohmann_json_j["plant_age"] = magic_enum::enum_name(nlohmann_json_t.plant_age);
        nlohmann_json_j["plant_need"] = magic_enum::enum_name(nlohmann_json_t.plant_need);
        // nlohmann_json_j["num_of_time_fertilized"] = nlohmann_json_t.num_of_time_fertilized;
        nlohmann_json_j["num_of_time_watered"] = nlohmann_json_t.num_of_time_watered;
        nlohmann_json_j["feeding_pre_grow"] = nlohmann_json_t.feeding_pre_grow;
        nlohmann_json_j["time"] = nlohmann_json_t.time;
        // nlohmann_json_j["future_attribute"] = nlohmann_json_t.future_attribute;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PottedPlantInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_t.orientation = magic_enum_cast<FacingDirection>(nlohmann_json_j.at("orientation").get<std::string>());
        nlohmann_json_t.color = magic_enum_cast<DrawVariation>(nlohmann_json_j.at("color").get<std::string>());
        nlohmann_json_t.plant_age = magic_enum_cast<PottedPlantAge>(nlohmann_json_j.at("plant_age").get<std::string>());
        nlohmann_json_t.plant_need = magic_enum_cast<PottedPlantNeed>(nlohmann_json_j.at("plant_need").get<std::string>());
        nlohmann_json_j.at("num_of_time_watered").get_to(nlohmann_json_t.num_of_time_watered);
        nlohmann_json_j.at("feeding_pre_grow").get_to(nlohmann_json_t.feeding_pre_grow);
        nlohmann_json_j.at("time").get_to(nlohmann_json_t.time);
        return;
    }

    struct PottedPlant
    {
        SeedType seed_type;
        GerdenType location;
        Position position;
        PottedPlantInfo plant_status;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PottedPlant &nlohmann_json_t) -> void
    {
        nlohmann_json_j["seed_type"] = magic_enum::enum_name(nlohmann_json_t.seed_type);
        nlohmann_json_j["location"] = magic_enum::enum_name(nlohmann_json_t.location);
        nlohmann_json_j["position"] = nlohmann_json_t.position;
        nlohmann_json_j["plant_status"] = nlohmann_json_t.plant_status;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PottedPlant &nlohmann_json_t) -> void
    {
        nlohmann_json_t.seed_type = magic_enum_cast<SeedType>(nlohmann_json_j.at("seed_type").get<std::string>());
        nlohmann_json_t.location = magic_enum_cast<GerdenType>(nlohmann_json_j.at("location").get<std::string>());
        nlohmann_json_j.at("position").get_to(nlohmann_json_t.position);
        nlohmann_json_j.at("plant_status").get_to(nlohmann_json_t.plant_status);
        return;
    }

    struct ZenGardenPurchased
    {
        //  bool potted_marigold_1;
        //  bool potted_marigold_2;
        // bool potted_marigold_3;
        int last_purchsed_basic_giftbox_time;
        int last_purchsed_bronze_giftboxtime;
        int last_purchsed_silver_giftbox_time;
        int last_purchsed_gold_giftbox_time;
        int last_purchesed_marigold_1_time;
        int last_purchesed_marigold_2_time;
        int last_purchesed_marigold_3_time;
        int fertilizer;
        int bug_spray;
        int chocolate;
        bool gold_watering_can;
        bool phonograph;
        bool gardening_glove;
        bool mushroom_garden;
        bool wheel_barrow;
        bool aquarium_garden;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ZenGardenPurchased &nlohmann_json_t) -> void
    {
        if (k_version >= 30)
        {
            nlohmann_json_j["last_purchsed_basic_giftbox_time"] = nlohmann_json_t.last_purchsed_basic_giftbox_time;
            nlohmann_json_j["last_purchsed_bronze_giftboxtime"] = nlohmann_json_t.last_purchsed_bronze_giftboxtime;
            nlohmann_json_j["last_purchsed_silver_giftbox_time"] = nlohmann_json_t.last_purchsed_silver_giftbox_time;
            nlohmann_json_j["last_purchsed_gold_giftbox_time"] = nlohmann_json_t.last_purchsed_gold_giftbox_time;
        }
        //  nlohmann_json_j["potted_marigold_1"] = nlohmann_json_t.potted_marigold_1;
        nlohmann_json_j["last_purchesed_marigold_1_time"] = nlohmann_json_t.last_purchesed_marigold_1_time;
        //   nlohmann_json_j["potted_marigold_2"] = nlohmann_json_t.potted_marigold_2;
        nlohmann_json_j["last_purchesed_marigold_2_time"] = nlohmann_json_t.last_purchesed_marigold_2_time;
        //  nlohmann_json_j["potted_marigold_3"] = nlohmann_json_t.potted_marigold_3;
        nlohmann_json_j["last_purchesed_marigold_3_time"] = nlohmann_json_t.last_purchesed_marigold_3_time;
        nlohmann_json_j["fertilizer"] = nlohmann_json_t.fertilizer;
        nlohmann_json_j["bug_spray"] = nlohmann_json_t.bug_spray;
        nlohmann_json_j["chocolate"] = nlohmann_json_t.chocolate;
        nlohmann_json_j["gold_watering_can"] = nlohmann_json_t.gold_watering_can;
        nlohmann_json_j["phonograph"] = nlohmann_json_t.phonograph;
        nlohmann_json_j["gardening_glove"] = nlohmann_json_t.gardening_glove;
        nlohmann_json_j["mushroom_garden"] = nlohmann_json_t.mushroom_garden;
        nlohmann_json_j["wheel_barrow"] = nlohmann_json_t.wheel_barrow;
        nlohmann_json_j["aquarium_garden"] = nlohmann_json_t.aquarium_garden;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ZenGardenPurchased &nlohmann_json_t) -> void
    {
        if (k_version >= 30)
        {
            nlohmann_json_j.at("last_purchsed_basic_giftbox_time").get_to(nlohmann_json_t.last_purchsed_basic_giftbox_time);
            nlohmann_json_j.at("last_purchsed_bronze_giftboxtime").get_to(nlohmann_json_t.last_purchsed_bronze_giftboxtime);
            nlohmann_json_j.at("last_purchsed_silver_giftbox_time").get_to(nlohmann_json_t.last_purchsed_silver_giftbox_time);
            nlohmann_json_j.at("last_purchsed_gold_giftbox_time").get_to(nlohmann_json_t.last_purchsed_gold_giftbox_time);
        }
        nlohmann_json_j.at("last_purchesed_marigold_1_time").get_to(nlohmann_json_t.last_purchesed_marigold_1_time);
        nlohmann_json_j.at("last_purchesed_marigold_2_time").get_to(nlohmann_json_t.last_purchesed_marigold_2_time);
        nlohmann_json_j.at("last_purchesed_marigold_3_time").get_to(nlohmann_json_t.last_purchesed_marigold_3_time);
        nlohmann_json_j.at("fertilizer").get_to(nlohmann_json_t.fertilizer);
        nlohmann_json_j.at("bug_spray").get_to(nlohmann_json_t.bug_spray);
        nlohmann_json_j.at("chocolate").get_to(nlohmann_json_t.chocolate);
        nlohmann_json_j.at("gold_watering_can").get_to(nlohmann_json_t.gold_watering_can);
        nlohmann_json_j.at("phonograph").get_to(nlohmann_json_t.phonograph);
        nlohmann_json_j.at("gardening_glove").get_to(nlohmann_json_t.gardening_glove);
        nlohmann_json_j.at("mushroom_garden").get_to(nlohmann_json_t.mushroom_garden);
        nlohmann_json_j.at("wheel_barrow").get_to(nlohmann_json_t.wheel_barrow);
        nlohmann_json_j.at("aquarium_garden").get_to(nlohmann_json_t.aquarium_garden);
        return;
    }

    struct TreeOfWisdom
    {
        bool purchased;
        int height;
        int tree_food;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TreeOfWisdom &nlohmann_json_t) -> void
    {
        nlohmann_json_j["purchased"] = nlohmann_json_t.purchased;
        nlohmann_json_j["height"] = nlohmann_json_t.height;
        nlohmann_json_j["tree_food"] = nlohmann_json_t.tree_food;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TreeOfWisdom &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("purchased").get_to(nlohmann_json_t.purchased);
        nlohmann_json_j.at("height").get_to(nlohmann_json_t.height);
        nlohmann_json_j.at("tree_food").get_to(nlohmann_json_t.tree_food);
        return;
    }

    struct ZenGarden
    {
        StinkyTheSnail stinky;
        TreeOfWisdom tree_of_wisdom;
        ZenGardenPurchased purchased;
        std::vector<PottedPlant> potted_plants;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ZenGarden &nlohmann_json_t) -> void
    {
        nlohmann_json_j["stinky"] = nlohmann_json_t.stinky;
        if (k_version >= 11)
        {
            nlohmann_json_j["tree_of_wisdom"] = nlohmann_json_t.tree_of_wisdom;
        }
        else
        {
            nlohmann_json_j["tree_of_wisdom"] = nullptr;
        }
        nlohmann_json_j["purchased"] = nlohmann_json_t.purchased;
        nlohmann_json_j["potted_plants"] = nlohmann_json_t.potted_plants;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ZenGarden &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("stinky").get_to(nlohmann_json_t.stinky);
        if (k_version >= 11)
        {
            nlohmann_json_j.at("tree_of_wisdom").get_to(nlohmann_json_t.tree_of_wisdom);
        }
        nlohmann_json_j.at("purchased").get_to(nlohmann_json_t.purchased);
        nlohmann_json_j.at("potted_plants").get_to(nlohmann_json_t.potted_plants);
        return;
    }

    struct StorePurchased
    {
        bool pool_cleaner;
        bool roof_cleaner;
        bool first_aid;
        bool plant_gatling_pea;
        bool plant_twin_sunflower;
        bool plant_gloomshroom;
        bool plant_cattail;
        bool plant_winter_melon;
        bool plant_gold_magnet;
        bool plant_sprike_rock;
        bool plant_cob_cannon;
        bool plant_imitater;
        bool bonus_lawn_mower;
        bool zombotany;
        bool minigame_pack_1;
        bool minigame_pack_2;
        bool minigame_pack_3;
        bool minigame_pack_4;
        bool minigame_pack_5;
        bool i_zombie_pack;
        bool scary_potter_pack;
        bool last_stand_pack;
        bool bacon;
        bool bacon_2;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StorePurchased &nlohmann_json_t) -> void
    {
        nlohmann_json_j["pool_cleaner"] = nlohmann_json_t.pool_cleaner;
        nlohmann_json_j["roof_cleaner"] = nlohmann_json_t.roof_cleaner;
        if (k_version >= 11)
        {
            nlohmann_json_j["first_aid"] = nlohmann_json_t.first_aid;
        }
        nlohmann_json_j["plant_gatling_pea"] = nlohmann_json_t.plant_gatling_pea;
        nlohmann_json_j["plant_twin_sunflower"] = nlohmann_json_t.plant_twin_sunflower;
        nlohmann_json_j["plant_gloomshroom"] = nlohmann_json_t.plant_gloomshroom;
        nlohmann_json_j["plant_cattail"] = nlohmann_json_t.plant_cattail;
        nlohmann_json_j["plant_winter_melon"] = nlohmann_json_t.plant_winter_melon;
        nlohmann_json_j["plant_gold_magnet"] = nlohmann_json_t.plant_gold_magnet;
        nlohmann_json_j["plant_sprike_rock"] = nlohmann_json_t.plant_sprike_rock;
        nlohmann_json_j["plant_cob_cannon"] = nlohmann_json_t.plant_cob_cannon;
        nlohmann_json_j["plant_imitater"] = nlohmann_json_t.plant_imitater;
        if (k_version >= 30)
        {
            nlohmann_json_j["zombotany"] = nlohmann_json_t.zombotany;
            nlohmann_json_j["minigame_pack_1"] = nlohmann_json_t.minigame_pack_1;
            nlohmann_json_j["minigame_pack_2"] = nlohmann_json_t.minigame_pack_2;
            nlohmann_json_j["minigame_pack_3"] = nlohmann_json_t.minigame_pack_3;
            nlohmann_json_j["minigame_pack_4"] = nlohmann_json_t.minigame_pack_4;
            nlohmann_json_j["minigame_pack_5"] = nlohmann_json_t.minigame_pack_5;
            nlohmann_json_j["i_zombie_pack"] = nlohmann_json_t.i_zombie_pack;
            nlohmann_json_j["scary_potter_pack"] = nlohmann_json_t.scary_potter_pack;
            nlohmann_json_j["last_stand_pack"] = nlohmann_json_t.last_stand_pack;
            nlohmann_json_j["bacon"] = nlohmann_json_t.bacon;
            nlohmann_json_j["bacon_2"] = nlohmann_json_t.bacon_2;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StorePurchased &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("pool_cleaner").get_to(nlohmann_json_t.pool_cleaner);
        nlohmann_json_j.at("roof_cleaner").get_to(nlohmann_json_t.roof_cleaner);
        if (k_version >= 11)
        {
            nlohmann_json_j.at("first_aid").get_to(nlohmann_json_t.first_aid);
        }
        nlohmann_json_j.at("plant_gatling_pea").get_to(nlohmann_json_t.plant_gatling_pea);
        nlohmann_json_j.at("plant_twin_sunflower").get_to(nlohmann_json_t.plant_twin_sunflower);
        nlohmann_json_j.at("plant_gloomshroom").get_to(nlohmann_json_t.plant_gloomshroom);
        nlohmann_json_j.at("plant_cattail").get_to(nlohmann_json_t.plant_cattail);
        nlohmann_json_j.at("plant_winter_melon").get_to(nlohmann_json_t.plant_winter_melon);
        nlohmann_json_j.at("plant_gold_magnet").get_to(nlohmann_json_t.plant_gold_magnet);
        nlohmann_json_j.at("plant_sprike_rock").get_to(nlohmann_json_t.plant_sprike_rock);
        nlohmann_json_j.at("plant_cob_cannon").get_to(nlohmann_json_t.plant_cob_cannon);
        nlohmann_json_j.at("plant_imitater").get_to(nlohmann_json_t.plant_imitater);
        if (k_version >= 30)
        {
            nlohmann_json_j.at("zombotany").get_to(nlohmann_json_t.zombotany);
            nlohmann_json_j.at("minigame_pack_1").get_to(nlohmann_json_t.minigame_pack_1);
            nlohmann_json_j.at("minigame_pack_2").get_to(nlohmann_json_t.minigame_pack_2);
            nlohmann_json_j.at("minigame_pack_3").get_to(nlohmann_json_t.minigame_pack_3);
            nlohmann_json_j.at("minigame_pack_4").get_to(nlohmann_json_t.minigame_pack_4);
            nlohmann_json_j.at("minigame_pack_5").get_to(nlohmann_json_t.minigame_pack_5);
            nlohmann_json_j.at("i_zombie_pack").get_to(nlohmann_json_t.i_zombie_pack);
            nlohmann_json_j.at("scary_potter_pack").get_to(nlohmann_json_t.scary_potter_pack);
            nlohmann_json_j.at("last_stand_pack").get_to(nlohmann_json_t.last_stand_pack);
            nlohmann_json_j.at("bacon").get_to(nlohmann_json_t.bacon);
            nlohmann_json_j.at("bacon_2").get_to(nlohmann_json_t.bacon_2);
        }
        return;
    }

    struct Store
    {
        int extra_packet_slots;
        int uses_of_rake;
        int hammer;
        StorePurchased purchased;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Store &nlohmann_json_t) -> void
    {
        nlohmann_json_j["extra_packet_slots"] = nlohmann_json_t.extra_packet_slots;
        nlohmann_json_j["uses_of_rake"] = nlohmann_json_t.uses_of_rake;
        if (k_version >= 30)
        {
            nlohmann_json_j["hammer"] = nlohmann_json_t.hammer;
        }
        nlohmann_json_j["purchased"] = nlohmann_json_t.purchased;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Store &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("extra_packet_slots").get_to(nlohmann_json_t.extra_packet_slots);
        nlohmann_json_j.at("uses_of_rake").get_to(nlohmann_json_t.uses_of_rake);
        if (k_version >= 30)
        {
            nlohmann_json_j.at("hammer").get_to(nlohmann_json_t.hammer);
        }
        nlohmann_json_j.at("purchased").get_to(nlohmann_json_t.purchased);
        return;
    }

    struct AchievementInfo
    {
        bool earned;
        bool shown;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AchievementInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["earned"] = nlohmann_json_t.earned;
        nlohmann_json_j["shown"] = nlohmann_json_t.shown;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AchievementInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("earned").get_to(nlohmann_json_t.earned);
        nlohmann_json_j.at("shown").get_to(nlohmann_json_t.shown);
        return;
    }

    struct Achievement
    {
        int num_of_achievement; // mics
        AchievementInfo home_lawn_security;
        AchievementInfo nobel_peas_prize;
        AchievementInfo better_off_dead;
        AchievementInfo china_shop;
        AchievementInfo spudow;
        AchievementInfo explodonator;
        AchievementInfo morticulturalist;
        AchievementInfo dont_pea_in_the_pool;
        AchievementInfo roll_some_heads;
        AchievementInfo grounded;
        AchievementInfo zombologist;
        AchievementInfo penny_pincher;
        AchievementInfo sunny_days;
        AchievementInfo popcorn_party;
        AchievementInfo good_morning;
        AchievementInfo no_fungus_among_us;
        AchievementInfo beyond_the_grave;
        AchievementInfo immortal;
        AchievementInfo towering_wisdom;
        AchievementInfo mustache_mode;
        AchievementInfo last_mown_stading;
        AchievementInfo below_zero;
        AchievementInfo flower_power;
        AchievementInfo pyromaniac;
        AchievementInfo lawn_mowner_man;
        AchievementInfo chill_out;
        AchievementInfo defcorn_five;
        AchievementInfo monster_mash;
        AchievementInfo blind_faith;
        AchievementInfo pool_closed;
        AchievementInfo melony_lane;
        AchievementInfo second_life;
        AchievementInfo lucky_spin;
        AchievementInfo chilli_free;
        AchievementInfo enlightened;
        AchievementInfo diamond_beghouler;
        AchievementInfo greenfingers;
        AchievementInfo wallnot_attack;
        AchievementInfo peking_express;
        AchievementInfo shooting_star;
        AchievementInfo sol_invictus;
        AchievementInfo sproing_sproing;
        AchievementInfo i_win;
        AchievementInfo where_the_sun_dont_shine;
        AchievementInfo undead_space;
        AchievementInfo disco_is_undead;
        AchievementInfo smashing;
        AchievementInfo gotcha;
        AchievementInfo still_standing;
        AchievementInfo the_stuff_of_legends;
        AchievementInfo hammer_time;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Achievement &nlohmann_json_t) -> void
    {
        if (k_version >= 12)
        {
            if (k_version < 13)
            {
                nlohmann_json_j["home_lawn_security"] = nlohmann_json_t.home_lawn_security;
                nlohmann_json_j["nobel_peas_prize"] = nlohmann_json_t.nobel_peas_prize;
                nlohmann_json_j["better_off_dead"] = nlohmann_json_t.better_off_dead;
                nlohmann_json_j["china_shop"] = nlohmann_json_t.china_shop;
                nlohmann_json_j["spudow"] = nlohmann_json_t.spudow;
                nlohmann_json_j["explodonator"] = nlohmann_json_t.explodonator;
                nlohmann_json_j["morticulturalist"] = nlohmann_json_t.morticulturalist;
                nlohmann_json_j["dont_pea_in_the_pool"] = nlohmann_json_t.dont_pea_in_the_pool;
                nlohmann_json_j["roll_some_heads"] = nlohmann_json_t.roll_some_heads;
                nlohmann_json_j["grounded"] = nlohmann_json_t.grounded;
                nlohmann_json_j["zombologist"] = nlohmann_json_t.zombologist;
                nlohmann_json_j["penny_pincher"] = nlohmann_json_t.penny_pincher;
                nlohmann_json_j["sunny_days"] = nlohmann_json_t.sunny_days;
                nlohmann_json_j["popcorn_party"] = nlohmann_json_t.popcorn_party;
                nlohmann_json_j["good_morning"] = nlohmann_json_t.good_morning;
                nlohmann_json_j["no_fungus_among_us"] = nlohmann_json_t.no_fungus_among_us;
                nlohmann_json_j["beyond_the_grave"] = nlohmann_json_t.beyond_the_grave;
                nlohmann_json_j["immortal"] = nlohmann_json_t.immortal;
                nlohmann_json_j["towering_wisdom"] = nlohmann_json_t.towering_wisdom;
                nlohmann_json_j["mustache_mode"] = nlohmann_json_t.mustache_mode;
            }
            else if (k_version < 30)
            {
                nlohmann_json_j["home_lawn_security"] = nlohmann_json_t.home_lawn_security;
                nlohmann_json_j["spudow"] = nlohmann_json_t.spudow;
                nlohmann_json_j["explodonator"] = nlohmann_json_t.explodonator;
                nlohmann_json_j["morticulturalist"] = nlohmann_json_t.morticulturalist;
                nlohmann_json_j["dont_pea_in_the_pool"] = nlohmann_json_t.dont_pea_in_the_pool;
                nlohmann_json_j["roll_some_heads"] = nlohmann_json_t.roll_some_heads;
                nlohmann_json_j["grounded"] = nlohmann_json_t.grounded;
                nlohmann_json_j["zombologist"] = nlohmann_json_t.zombologist;
                nlohmann_json_j["penny_pincher"] = nlohmann_json_t.penny_pincher;
                nlohmann_json_j["sunny_days"] = nlohmann_json_t.sunny_days;
                nlohmann_json_j["popcorn_party"] = nlohmann_json_t.popcorn_party;
                nlohmann_json_j["good_morning"] = nlohmann_json_t.good_morning;
                nlohmann_json_j["no_fungus_among_us"] = nlohmann_json_t.no_fungus_among_us;
                if (nlohmann_json_t.num_of_achievement > 13)
                {
                    try_assert(nlohmann_json_t.num_of_achievement == 25, "invalid_num_of_achievement");
                    nlohmann_json_j["last_mown_stading"] = nlohmann_json_t.last_mown_stading;
                    nlohmann_json_j["below_zero"] = nlohmann_json_t.below_zero;
                    nlohmann_json_j["flower_power"] = nlohmann_json_t.flower_power;
                    nlohmann_json_j["pyromaniac"] = nlohmann_json_t.pyromaniac;
                    nlohmann_json_j["lawn_mowner_man"] = nlohmann_json_t.lawn_mowner_man;
                    nlohmann_json_j["chill_out"] = nlohmann_json_t.chill_out;
                    nlohmann_json_j["defcorn_five"] = nlohmann_json_t.defcorn_five;
                    nlohmann_json_j["monster_mash"] = nlohmann_json_t.monster_mash;
                    nlohmann_json_j["blind_faith"] = nlohmann_json_t.blind_faith;
                    nlohmann_json_j["pool_closed"] = nlohmann_json_t.pool_closed;
                    nlohmann_json_j["melony_lane"] = nlohmann_json_t.melony_lane;
                    nlohmann_json_j["second_life"] = nlohmann_json_t.second_life;
                }
            }
            else
            {
                nlohmann_json_j["home_lawn_security"] = nlohmann_json_t.home_lawn_security;
                nlohmann_json_j["spudow"] = nlohmann_json_t.spudow;
                nlohmann_json_j["explodonator"] = nlohmann_json_t.explodonator;
                nlohmann_json_j["morticulturalist"] = nlohmann_json_t.morticulturalist;
                nlohmann_json_j["dont_pea_in_the_pool"] = nlohmann_json_t.dont_pea_in_the_pool;
                nlohmann_json_j["roll_some_heads"] = nlohmann_json_t.roll_some_heads;
                nlohmann_json_j["grounded"] = nlohmann_json_t.grounded;
                nlohmann_json_j["zombologist"] = nlohmann_json_t.zombologist;
                nlohmann_json_j["penny_pincher"] = nlohmann_json_t.penny_pincher;
                nlohmann_json_j["sunny_days"] = nlohmann_json_t.sunny_days;
                nlohmann_json_j["good_morning"] = nlohmann_json_t.good_morning;
                nlohmann_json_j["no_fungus_among_us"] = nlohmann_json_t.no_fungus_among_us;
                nlohmann_json_j["last_mown_stading"] = nlohmann_json_t.last_mown_stading;
                nlohmann_json_j["below_zero"] = nlohmann_json_t.below_zero;
                nlohmann_json_j["flower_power"] = nlohmann_json_t.flower_power;
                nlohmann_json_j["pyromaniac"] = nlohmann_json_t.pyromaniac;
                nlohmann_json_j["lawn_mowner_man"] = nlohmann_json_t.lawn_mowner_man;
                nlohmann_json_j["chill_out"] = nlohmann_json_t.chill_out;
                nlohmann_json_j["defcorn_five"] = nlohmann_json_t.defcorn_five;
                nlohmann_json_j["monster_mash"] = nlohmann_json_t.monster_mash;
                nlohmann_json_j["blind_faith"] = nlohmann_json_t.blind_faith;
                nlohmann_json_j["pool_closed"] = nlohmann_json_t.pool_closed;
                nlohmann_json_j["melony_lane"] = nlohmann_json_t.melony_lane;
                nlohmann_json_j["second_life"] = nlohmann_json_t.second_life;
                nlohmann_json_j["lucky_spin"] = nlohmann_json_t.lucky_spin;
                nlohmann_json_j["chilli_free"] = nlohmann_json_t.chilli_free;
                nlohmann_json_j["enlightened"] = nlohmann_json_t.enlightened;
                nlohmann_json_j["diamond_beghouler"] = nlohmann_json_t.diamond_beghouler;
                nlohmann_json_j["greenfingers"] = nlohmann_json_t.greenfingers;
                nlohmann_json_j["wallnot_attack"] = nlohmann_json_t.wallnot_attack;
                nlohmann_json_j["beyond_the_grave"] = nlohmann_json_t.beyond_the_grave;
                nlohmann_json_j["peking_express"] = nlohmann_json_t.peking_express;
                nlohmann_json_j["shooting_star"] = nlohmann_json_t.shooting_star;
                nlohmann_json_j["sol_invictus"] = nlohmann_json_t.sol_invictus;
                nlohmann_json_j["sproing_sproing"] = nlohmann_json_t.sproing_sproing;
                nlohmann_json_j["better_off_dead"] = nlohmann_json_t.better_off_dead;
                nlohmann_json_j["i_win"] = nlohmann_json_t.i_win;
                nlohmann_json_j["where_the_sun_dont_shine"] = nlohmann_json_t.where_the_sun_dont_shine;
                nlohmann_json_j["undead_space"] = nlohmann_json_t.undead_space;
                nlohmann_json_j["disco_is_undead"] = nlohmann_json_t.disco_is_undead;
                nlohmann_json_j["china_shop"] = nlohmann_json_t.china_shop;
                nlohmann_json_j["smashing"] = nlohmann_json_t.smashing;
                nlohmann_json_j["gotcha"] = nlohmann_json_t.gotcha;
                nlohmann_json_j["still_standing"] = nlohmann_json_t.still_standing;
                nlohmann_json_j["the_stuff_of_legends"] = nlohmann_json_t.the_stuff_of_legends;
                nlohmann_json_j["hammer_time"] = nlohmann_json_t.hammer_time;
            }
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Achievement &nlohmann_json_t) -> void
    {
        if (k_version >= 12)
        {
            if (k_version < 13)
            {
                nlohmann_json_j.at("home_lawn_security").get_to(nlohmann_json_t.home_lawn_security);
                nlohmann_json_j.at("nobel_peas_prize").get_to(nlohmann_json_t.nobel_peas_prize);
                nlohmann_json_j.at("better_off_dead").get_to(nlohmann_json_t.better_off_dead);
                nlohmann_json_j.at("china_shop").get_to(nlohmann_json_t.china_shop);
                nlohmann_json_j.at("spudow").get_to(nlohmann_json_t.spudow);
                nlohmann_json_j.at("explodonator").get_to(nlohmann_json_t.explodonator);
                nlohmann_json_j.at("morticulturalist").get_to(nlohmann_json_t.morticulturalist);
                nlohmann_json_j.at("dont_pea_in_the_pool").get_to(nlohmann_json_t.dont_pea_in_the_pool);
                nlohmann_json_j.at("roll_some_heads").get_to(nlohmann_json_t.roll_some_heads);
                nlohmann_json_j.at("grounded").get_to(nlohmann_json_t.grounded);
                nlohmann_json_j.at("zombologist").get_to(nlohmann_json_t.zombologist);
                nlohmann_json_j.at("penny_pincher").get_to(nlohmann_json_t.penny_pincher);
                nlohmann_json_j.at("sunny_days").get_to(nlohmann_json_t.sunny_days);
                nlohmann_json_j.at("popcorn_party").get_to(nlohmann_json_t.popcorn_party);
                nlohmann_json_j.at("good_morning").get_to(nlohmann_json_t.good_morning);
                nlohmann_json_j.at("no_fungus_among_us").get_to(nlohmann_json_t.no_fungus_among_us);
                nlohmann_json_j.at("beyond_the_grave").get_to(nlohmann_json_t.beyond_the_grave);
                nlohmann_json_j.at("immortal").get_to(nlohmann_json_t.immortal);
                nlohmann_json_j.at("towering_wisdom").get_to(nlohmann_json_t.towering_wisdom);
                nlohmann_json_j.at("mustache_mode").get_to(nlohmann_json_t.mustache_mode);
            }
            else if (k_version < 30)
            {
                nlohmann_json_j.at("home_lawn_security").get_to(nlohmann_json_t.home_lawn_security);
                nlohmann_json_j.at("spudow").get_to(nlohmann_json_t.spudow);
                nlohmann_json_j.at("explodonator").get_to(nlohmann_json_t.explodonator);
                nlohmann_json_j.at("morticulturalist").get_to(nlohmann_json_t.morticulturalist);
                nlohmann_json_j.at("dont_pea_in_the_pool").get_to(nlohmann_json_t.dont_pea_in_the_pool);
                nlohmann_json_j.at("roll_some_heads").get_to(nlohmann_json_t.roll_some_heads);
                nlohmann_json_j.at("grounded").get_to(nlohmann_json_t.grounded);
                nlohmann_json_j.at("zombologist").get_to(nlohmann_json_t.zombologist);
                nlohmann_json_j.at("penny_pincher").get_to(nlohmann_json_t.penny_pincher);
                nlohmann_json_j.at("sunny_days").get_to(nlohmann_json_t.sunny_days);
                nlohmann_json_j.at("popcorn_party").get_to(nlohmann_json_t.popcorn_party);
                nlohmann_json_j.at("good_morning").get_to(nlohmann_json_t.good_morning);
                nlohmann_json_j.at("no_fungus_among_us").get_to(nlohmann_json_t.no_fungus_among_us);
                nlohmann_json_t.num_of_achievement = 13;
                try
                {
                    nlohmann_json_j.at("last_mown_stading").get_to(nlohmann_json_t.last_mown_stading);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("below_zero").get_to(nlohmann_json_t.below_zero);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("flower_power").get_to(nlohmann_json_t.flower_power);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("pyromaniac").get_to(nlohmann_json_t.pyromaniac);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("lawn_mowner_man").get_to(nlohmann_json_t.lawn_mowner_man);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("chill_out").get_to(nlohmann_json_t.chill_out);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("defcorn_five").get_to(nlohmann_json_t.defcorn_five);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("monster_mash").get_to(nlohmann_json_t.monster_mash);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("blind_faith").get_to(nlohmann_json_t.blind_faith);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("pool_closed").get_to(nlohmann_json_t.pool_closed);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("melony_lane").get_to(nlohmann_json_t.melony_lane);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                try
                {
                    nlohmann_json_j.at("second_life").get_to(nlohmann_json_t.second_life);
                    ++nlohmann_json_t.num_of_achievement;
                }
                catch (nlohmann::ordered_json::exception &e)
                {
                }
                if (nlohmann_json_t.num_of_achievement > 13)
                {
                    try_assert(nlohmann_json_t.num_of_achievement == 25, "invalid_num_of_achievement");
                }
            }
            else
            {
                nlohmann_json_j.at("home_lawn_security").get_to(nlohmann_json_t.home_lawn_security);
                nlohmann_json_j.at("spudow").get_to(nlohmann_json_t.spudow);
                nlohmann_json_j.at("explodonator").get_to(nlohmann_json_t.explodonator);
                nlohmann_json_j.at("morticulturalist").get_to(nlohmann_json_t.morticulturalist);
                nlohmann_json_j.at("dont_pea_in_the_pool").get_to(nlohmann_json_t.dont_pea_in_the_pool);
                nlohmann_json_j.at("grounded").get_to(nlohmann_json_t.grounded);
                nlohmann_json_j.at("roll_some_heads").get_to(nlohmann_json_t.roll_some_heads);
                nlohmann_json_j.at("zombologist").get_to(nlohmann_json_t.zombologist);
                nlohmann_json_j.at("penny_pincher").get_to(nlohmann_json_t.penny_pincher);
                nlohmann_json_j.at("sunny_days").get_to(nlohmann_json_t.sunny_days);
                nlohmann_json_j.at("good_morning").get_to(nlohmann_json_t.good_morning);
                nlohmann_json_j.at("no_fungus_among_us").get_to(nlohmann_json_t.no_fungus_among_us);
                nlohmann_json_j.at("last_mown_stading").get_to(nlohmann_json_t.last_mown_stading);
                nlohmann_json_j.at("below_zero").get_to(nlohmann_json_t.below_zero);
                nlohmann_json_j.at("flower_power").get_to(nlohmann_json_t.flower_power);
                nlohmann_json_j.at("pyromaniac").get_to(nlohmann_json_t.pyromaniac);
                nlohmann_json_j.at("lawn_mowner_man").get_to(nlohmann_json_t.lawn_mowner_man);
                nlohmann_json_j.at("chill_out").get_to(nlohmann_json_t.chill_out);
                nlohmann_json_j.at("defcorn_five").get_to(nlohmann_json_t.defcorn_five);
                nlohmann_json_j.at("monster_mash").get_to(nlohmann_json_t.monster_mash);
                nlohmann_json_j.at("blind_faith").get_to(nlohmann_json_t.blind_faith);
                nlohmann_json_j.at("pool_closed").get_to(nlohmann_json_t.pool_closed);
                nlohmann_json_j.at("melony_lane").get_to(nlohmann_json_t.melony_lane);
                nlohmann_json_j.at("second_life").get_to(nlohmann_json_t.second_life);
                nlohmann_json_j.at("lucky_spin").get_to(nlohmann_json_t.lucky_spin);
                nlohmann_json_j.at("chilli_free").get_to(nlohmann_json_t.chilli_free);
                nlohmann_json_j.at("enlightened").get_to(nlohmann_json_t.enlightened);
                nlohmann_json_j.at("diamond_beghouler").get_to(nlohmann_json_t.diamond_beghouler);
                nlohmann_json_j.at("greenfingers").get_to(nlohmann_json_t.greenfingers);
                nlohmann_json_j.at("wallnot_attack").get_to(nlohmann_json_t.wallnot_attack);
                nlohmann_json_j.at("beyond_the_grave").get_to(nlohmann_json_t.beyond_the_grave);
                nlohmann_json_j.at("peking_express").get_to(nlohmann_json_t.peking_express);
                nlohmann_json_j.at("shooting_star").get_to(nlohmann_json_t.shooting_star);
                nlohmann_json_j.at("sol_invictus").get_to(nlohmann_json_t.sol_invictus);
                nlohmann_json_j.at("sproing_sproing").get_to(nlohmann_json_t.sproing_sproing);
                nlohmann_json_j.at("better_off_dead").get_to(nlohmann_json_t.better_off_dead);
                nlohmann_json_j.at("i_win").get_to(nlohmann_json_t.i_win);
                nlohmann_json_j.at("where_the_sun_dont_shine").get_to(nlohmann_json_t.where_the_sun_dont_shine);
                nlohmann_json_j.at("undead_space").get_to(nlohmann_json_t.undead_space);
                nlohmann_json_j.at("disco_is_undead").get_to(nlohmann_json_t.disco_is_undead);
                nlohmann_json_j.at("china_shop").get_to(nlohmann_json_t.china_shop);
                nlohmann_json_j.at("smashing").get_to(nlohmann_json_t.smashing);
                nlohmann_json_j.at("gotcha").get_to(nlohmann_json_t.gotcha);
                nlohmann_json_j.at("still_standing").get_to(nlohmann_json_t.still_standing);
                nlohmann_json_j.at("the_stuff_of_legends").get_to(nlohmann_json_t.the_stuff_of_legends);
                nlohmann_json_j.at("hammer_time").get_to(nlohmann_json_t.hammer_time);
            }
        }
        return;
    }

    struct ZombatarArgument
    {
        int type;
        int color;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ZombatarArgument &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["color"] = nlohmann_json_t.color;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ZombatarArgument &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        nlohmann_json_j.at("color").get_to(nlohmann_json_t.color);
        return;
    }

    struct ZombatarValue
    {
        int skin_color;
        ZombatarArgument clothes;
        ZombatarArgument tidbits;
        ZombatarArgument accessories;
        ZombatarArgument facial_hair;
        ZombatarArgument hair;
        ZombatarArgument eyewear;
        ZombatarArgument hat;
        ZombatarArgument backdrop;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ZombatarValue &nlohmann_json_t) -> void
    {
        nlohmann_json_j["skin_color"] = nlohmann_json_t.skin_color;
        nlohmann_json_j["clothes"] = nlohmann_json_t.clothes;
        nlohmann_json_j["accessories"] = nlohmann_json_t.accessories;
        nlohmann_json_j["facial_hair"] = nlohmann_json_t.facial_hair;
        nlohmann_json_j["hair"] = nlohmann_json_t.hair;
        nlohmann_json_j["eyewear"] = nlohmann_json_t.eyewear;
        nlohmann_json_j["backdrop"] = nlohmann_json_t.backdrop;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ZombatarValue &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("skin_color").get_to(nlohmann_json_t.skin_color);
        nlohmann_json_j.at("clothes").get_to(nlohmann_json_t.clothes);
        nlohmann_json_j.at("accessories").get_to(nlohmann_json_t.accessories);
        nlohmann_json_j.at("facial_hair").get_to(nlohmann_json_t.facial_hair);
        nlohmann_json_j.at("hair").get_to(nlohmann_json_t.hair);
        nlohmann_json_j.at("eyewear").get_to(nlohmann_json_t.eyewear);
        nlohmann_json_j.at("backdrop").get_to(nlohmann_json_t.backdrop);
        return;
    }

    struct Zombatar
    {
        bool accepted_zombatar_license;
        bool display_save_image_messenge;
        std::vector<ZombatarValue> value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Zombatar &nlohmann_json_t) -> void
    {
        nlohmann_json_j["accepted_zombatar_license"] = nlohmann_json_t.accepted_zombatar_license;
        nlohmann_json_j["display_save_image_messenge"] = nlohmann_json_t.display_save_image_messenge;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Zombatar &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("accepted_zombatar_license").get_to(nlohmann_json_t.accepted_zombatar_license);
        nlohmann_json_j.at("display_save_image_messenge").get_to(nlohmann_json_t.display_save_image_messenge);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct DetailInfo
    {
        int version;
        General general;
        bool has_unlocked;
        Unlocked unlocked;
        bool has_trophies;
        ChallengeRecord trophies;
        Store store;
        bool has_zen_garden;
        ZenGarden zen_garden;
        bool has_achievement;
        Achievement achievement;
        bool has_zombatar;
        Zombatar zombatar;
        std::string unknown_expand;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const DetailInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["general"] = nlohmann_json_t.general;
        if (nlohmann_json_t.has_unlocked)
        {
            nlohmann_json_j["unlocked"] = nlohmann_json_t.unlocked;
        }
        else
        {
            nlohmann_json_j["unlocked"] = nullptr;
        }
        if (nlohmann_json_t.has_trophies)
        {
            nlohmann_json_j["trophies"] = nlohmann_json_t.trophies;
        }
        else
        {
            nlohmann_json_j["trophies"] = nullptr;
        }
        nlohmann_json_j["store"] = nlohmann_json_t.store;
        if (nlohmann_json_t.has_zen_garden)
        {
            nlohmann_json_j["zen_garden"] = nlohmann_json_t.zen_garden;
        }
        else
        {
            nlohmann_json_j["zen_garden"] = nullptr;
        }
        if (nlohmann_json_t.has_achievement)
        {
            nlohmann_json_j["achievement"] = nlohmann_json_t.achievement;
        }
        else
        {
            nlohmann_json_j["achievement"] = nullptr;
        }
        if (nlohmann_json_t.has_zombatar)
        {
            nlohmann_json_j["zombatar"] = nlohmann_json_t.zombatar;
        }
        else
        {
            nlohmann_json_j["zombatar"] = nullptr;
        }
        if (!nlohmann_json_t.unknown_expand.empty())
        {
            nlohmann_json_j["unknown_expand"] = nlohmann_json_t.unknown_expand;
        }
        else
        {
            nlohmann_json_j["unknown_expand"] = nullptr;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        DetailInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("general").get_to(nlohmann_json_t.general);
        if (nlohmann_json_j["unlocked"] != nullptr)
        {
            nlohmann_json_j.at("unlocked").get_to(nlohmann_json_t.unlocked);
            nlohmann_json_t.has_unlocked = true;
        }
        if (nlohmann_json_j["trophies"] != nullptr)
        {
            nlohmann_json_j.at("trophies").get_to(nlohmann_json_t.trophies);
            nlohmann_json_t.has_trophies = true;
        }
        nlohmann_json_j.at("store").get_to(nlohmann_json_t.store);
        if (nlohmann_json_j["zen_garden"] != nullptr)
        {
            nlohmann_json_j.at("zen_garden").get_to(nlohmann_json_t.zen_garden);
            nlohmann_json_t.has_zen_garden = true;
        }
        if (nlohmann_json_j["achievement"] != nullptr)
        {
            nlohmann_json_j.at("achievement").get_to(nlohmann_json_t.achievement);
            nlohmann_json_t.has_achievement = true;
        }
        if (nlohmann_json_j["zombatar"] != nullptr)
        {
            nlohmann_json_j.at("zombatar").get_to(nlohmann_json_t.zombatar);
            nlohmann_json_t.has_zombatar = true;
        }
        if (nlohmann_json_j["unknown_expand"] != nullptr)
        {
            nlohmann_json_j.at("unknown_expand").get_to(nlohmann_json_t.unknown_expand);
        }
        return;
    }

    struct UserDetail
    {
        std::string name;
        int use_seq;
        int id;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const UserDetail &nlohmann_json_t) -> void
    {
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        nlohmann_json_j["use_seq"] = nlohmann_json_t.use_seq;
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        UserDetail &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        nlohmann_json_j.at("use_seq").get_to(nlohmann_json_t.use_seq);
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        return;
    }

    struct SummaryInfo
    {
        int version;
        std::vector<UserDetail> users;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SummaryInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["users"] = nlohmann_json_t.users;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SummaryInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        nlohmann_json_j.at("users").get_to(nlohmann_json_t.users);
        return;
    }

    struct VersionInfo {
        int version;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const VersionInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        k_version = nlohmann_json_t.version;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        VersionInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        k_version = nlohmann_json_t.version;
        return;
    }
}