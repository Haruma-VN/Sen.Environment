#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/popcap/player_info/definition.hpp"
#include "kernel/support/popcap/player_info/common.hpp"

namespace Sen::Kernel::Support::PopCap::PlayerInfo
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Decode : Common
    {
    private:
        inline static auto exchange_data(
            std::vector<uint8_t> const &buffer) -> std::string
        {
            auto oss = std::ostringstream{};
            for (const auto &byte : buffer)
            {
                oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << ' ';
            }
            auto hexString = oss.str();
            std::transform(hexString.cbegin(), hexString.cend(), hexString.begin(), [&](auto c)
                           { return std::toupper(c); });
            return hexString.substr(0, hexString.length() - 1);
        }

        inline static auto exchange_detail_by_version(
            DataStreamView &data,
            DetailStructure &value) -> void
        {
            if (k_version < 13)
            {
                exchange_list(data, value.challenge_records, [](auto &data, auto &value)
                              { value = data.readInt32(); }, exchange_challenge_records_count());
                exchange_list(data, value.purchases, [](auto &data, auto &value)
                              { value = data.readInt32(); }, exchange_purchases_count());
                value.play_time_active_player = data.readInt32();
                value.play_time_inactive_player = data.readInt32();
                value.has_used_cheat_keys = data.readInt32();
                value.has_woken_stiky = data.readInt32();
                value.didnt_purchase_packet_upgrade = data.readInt32();
                value.last_stinky_chocolate_time = data.readInt32();
                value.stinky_pos_x = data.readInt32();
                value.stinky_pos_y = data.readInt32();
                value.has_unlocked_minigames = data.readInt32();
                value.has_unlocked_puzzle_mode = data.readInt32();
                value.has_new_minigame = data.readInt32();
                value.has_new_scary_potter = data.readInt32();
                value.has_new_i_zombie = data.readInt32();
                value.has_new_survival = data.readInt32();
                value.has_new_limbo_page = data.readInt32();
                value.need_message_on_game_seletor = data.readInt32();
                value.need_magic_taco_reward = data.readInt32();
                value.has_seen_stinky = data.readInt32();
                value.has_seen_upsell = data.readInt32();
                value.place_holder_player_stats = data.readInt32();
                auto num_potted_plants = data.readInt32();
                exchange_list(data, value.potted_plants, &exchange_to_potted_plants, num_potted_plants);
                value.has_zen_garden = true;
                if (k_version < 11 || data.read_pos >= data.size())
                {
                    return;
                }
                for (auto i : Range(exchange_enumeration<AchievementId, size_t>(AchievementId::home_security)))
                {
                    value.earned_achievements.emplace_back(data.readBoolean());
                    value.shown_achievements.emplace_back(data.readBoolean());
                }
                value.has_achievement = true;
                if (k_version == 12)
                {
                    value.accepted_zombatar_license = data.readBoolean();
                    exchange_list(data, value.zombatars, &exchange_to_zombatar_struct, data.readInt32());
                    data.read_pos += 0x14_size;
                    value.display_save_image_messenge = data.readBoolean();
                    value.has_zombatar = true;
                }
            }
            else if (k_version < 30)
            {
                data.read_pos += (exchange_challenge_records_count() * k_block_size);
                exchange_list(data, value.purchases, [](auto &data, auto &value)
                              { value = data.readInt32(); }, exchange_purchases_count());
                auto num_of_achievement = exchange_enumeration<AchievementId, size_t>(AchievementId::home_security);
                if (k_version == 13 && data.read_pos + static_cast<size_t>(num_of_achievement * 2) >= data.size()) {
                    num_of_achievement = 13;
                }
                for (auto i : Range(num_of_achievement))
                {
                    value.earned_achievements.emplace_back(data.readBoolean());
                }
                for (auto i : Range(num_of_achievement))
                {
                    value.shown_achievements.emplace_back(data.readBoolean());
                }
                value.has_achievement = true;
                if (data.read_pos < data.size())
                {
                    value.expand = data.readBytes((data.size() - data.read_pos));
                }
            }
            else
            {
                exchange_list(data, value.challenge_records, [](auto &data, auto &value)
                              { value = data.readInt32(); }, exchange_challenge_records_count());
                exchange_list(data, value.purchases, [](auto &data, auto &value)
                              { value = data.readInt32(); }, exchange_purchases_count());
                value.play_time_active_player = data.readInt32();
                value.play_time_inactive_player = data.readInt32();
                value.has_used_cheat_keys = data.readInt32();
                value.has_woken_stiky = data.readInt32();
                value.didnt_purchase_packet_upgrade = data.readInt32();
                value.last_stinky_chocolate_time = data.readInt32();
                value.stinky_pos_x = data.readInt32();
                value.stinky_pos_y = data.readInt32();
                value.has_unlocked_minigames = data.readInt32();
                value.has_unlocked_puzzle_mode = data.readInt32();
                value.has_new_minigame = data.readInt32();
                value.has_new_scary_potter = data.readInt32();
                value.has_new_i_zombie = data.readInt32();
                value.has_new_survival = data.readInt32();
                value.has_new_limbo_page = data.readInt32();
                value.need_message_on_game_seletor = data.readInt32();
                value.need_magic_taco_reward = data.readInt32();
                value.has_seen_stinky = data.readInt32();
                value.has_seen_upsell = data.readInt32();
                value.place_holder_player_stats = data.readInt32();
                data.readInt32();
                auto num_potted_plants = data.readInt32();
                exchange_list(data, value.potted_plants, &exchange_to_potted_plants, num_potted_plants);
                value.has_zen_garden = true;
                auto num_of_achievement = exchange_enumeration<AchievementId, size_t>(AchievementId::home_security);
                for (auto i : Range(num_of_achievement))
                {
                    value.earned_achievements.emplace_back(data.readBoolean());
                }
                for (auto i : Range(num_of_achievement))
                {
                    value.shown_achievements.emplace_back(data.readBoolean());
                }
                value.has_achievement = true;
                if (data.read_pos < data.size())
                {
                    value.expand = data.readBytes((data.size() - data.read_pos));
                }
            }
        }

        inline static auto exchange_detail_structure(
            DataStreamView &data,
            DetailStructure &value) -> void
        {
            value.level = data.readInt32();
            value.coins = data.readInt32();
            value.finished_adventure = data.readInt32();
            exchange_detail_by_version(data, value);
            return;
        }

        inline static auto exchange_detail_general(
            DetailStructure const &data,
            General &value) -> void
        {
            value.adventure_level = data.level;
            value.coins = data.coins * k_coins_ratio;
            value.adventure_compeled = static_cast<bool>(data.need_message_on_game_seletor);
            value.completed_adventure_time = data.finished_adventure;
            value.player_time_active_player = data.play_time_active_player;
            value.player_time_inactive_player = data.play_time_inactive_player;
            value.place_holder_player_stats = data.place_holder_player_stats;
            value.has_magic_taco_reward = static_cast<bool>(data.need_magic_taco_reward);
            return;
        }

        inline static auto exchange_detail_unlocked(
            DetailStructure const &data,
            Unlocked &value) -> void
        {
            value.minigame = static_cast<bool>(data.has_unlocked_minigames);
            value.puzzle_mode = static_cast<bool>(data.has_unlocked_puzzle_mode);
            //  value.survival_mode = static_cast<bool>(data.has_unlocked_survival_mode);
            value.first_time_play_minigame = static_cast<bool>(data.has_new_minigame);
            value.first_time_play_vasebreaker = static_cast<bool>(data.has_new_scary_potter);
            value.first_time_play_i_zombie = static_cast<bool>(data.has_new_i_zombie);
            value.first_time_play_survival = static_cast<bool>(data.has_new_survival);
            return;
        }

        inline static auto exchange_purchased(
            std::vector<int> const &purchases,
            Store &store_value,
            ZenGarden &zen_garden) -> void
        {
            for (auto i : Range(exchange_enumeration<StoreItem, size_t>(StoreItem::pvz)))
            {
                if (static_cast<bool>(purchases[i]))
                {
                    switch (exchange_enumeration<uint8_t, StoreItem>(static_cast<uint8_t>(i)))
                    {
                    case StoreItem::plant_gatlingpea:
                    {
                        store_value.purchased.plant_gatling_pea = true;
                        break;
                    }
                    case StoreItem::plant_twinsunflower:
                    {
                        store_value.purchased.plant_twin_sunflower = true;
                        break;
                    }
                    case StoreItem::plant_gloomshroom:
                    {
                        store_value.purchased.plant_gloomshroom = true;
                        break;
                    }
                    case StoreItem::plant_cattail:
                    {
                        store_value.purchased.plant_cattail = true;
                        break;
                    }
                    case StoreItem::plant_wintermelon:
                    {
                        store_value.purchased.plant_winter_melon = true;
                        break;
                    }
                    case StoreItem::plant_gold_magnet:
                    {
                        store_value.purchased.plant_gold_magnet = true;
                        break;
                    }
                    case StoreItem::plant_spikerock:
                    {
                        store_value.purchased.plant_sprike_rock = true;
                        break;
                    }
                    case StoreItem::plant_cobcannon:
                    {
                        store_value.purchased.plant_cob_cannon = true;
                        break;
                    }
                    case StoreItem::plant_imitater:
                    {
                        store_value.purchased.plant_imitater = true;
                        break;
                    }
                    case StoreItem::bonus_lawn_mower:
                    {
                        //Unknown
                        break;
                    }
                    case StoreItem::potted_marigold_1:
                    {
                        zen_garden.purchased.last_purchesed_marigold_1_time = purchases[i];
                        break;
                    }
                    case StoreItem::potted_marigold_2:
                    {
                        zen_garden.purchased.last_purchesed_marigold_2_time = purchases[i];
                        break;
                    }
                    case StoreItem::potted_marigold_3:
                    {
                        zen_garden.purchased.last_purchesed_marigold_3_time = purchases[i];
                        break;
                    }
                    case StoreItem::gold_wateringcan:
                    {
                        zen_garden.purchased.gold_watering_can = true;
                        break;
                    }
                    case StoreItem::fertilizer:
                    {
                        zen_garden.purchased.fertilizer = purchases[i] - k_amount_ratio;
                        break;
                    }
                    case StoreItem::bug_spray:
                    {
                        zen_garden.purchased.bug_spray = purchases[i] - k_amount_ratio;
                        break;
                    }
                    case StoreItem::phonograph:
                    {
                        zen_garden.purchased.phonograph = true;
                        break;
                    }
                    case StoreItem::gardening_glove:
                    {
                        zen_garden.purchased.gardening_glove = true;
                        break;
                    }
                    case StoreItem::mushroom_garden:
                    {
                        zen_garden.purchased.mushroom_garden = true;
                        break;
                    }
                    case StoreItem::wheel_barrow:
                    {
                        zen_garden.purchased.wheel_barrow = true;
                        break;
                    }
                    case StoreItem::stinky_the_snail:
                    {
                        zen_garden.stinky.purchased = true;
                        zen_garden.stinky.status.last_awoken = purchases[i];
                        break;
                    }
                    case StoreItem::packet_upgrade:
                    {
                        store_value.extra_packet_slots = exchange_range<0, 4>(purchases[i]);
                        break;
                    }
                    case StoreItem::pool_cleaner:
                    {
                        store_value.purchased.pool_cleaner = true;
                        break;
                    }
                    case StoreItem::roof_cleaner:
                    {
                        store_value.purchased.roof_cleaner = true;
                        break;
                    }
                    case StoreItem::rake:
                    {
                        store_value.uses_of_rake = purchases[i];
                        break;
                    }
                    case StoreItem::aquarium_garden:
                    {
                        zen_garden.purchased.aquarium_garden = true;
                        break;
                    }
                    case StoreItem::chocolate:
                    {
                        zen_garden.purchased.chocolate = purchases[i] - k_amount_ratio;
                        break;
                    }
                    case StoreItem::tree_of_wisdom:
                    {
                        zen_garden.tree_of_wisdom.purchased = true;
                        break;
                    }
                    case StoreItem::tree_food:
                    {
                        zen_garden.tree_of_wisdom.tree_food = purchases[i] - k_amount_ratio;
                        break;
                    }
                    case StoreItem::firstaid:
                    {
                        store_value.purchased.first_aid = true;
                        break;
                    }
                    case StoreItem::basic_giftbox:
                    {
                        zen_garden.purchased.last_purchsed_basic_giftbox_time = purchases[i];
                        break;
                    }
                    case StoreItem::bronze_giftbox:
                    {
                        zen_garden.purchased.last_purchsed_bronze_giftboxtime = purchases[i];
                        break;
                    }
                    case StoreItem::silver_giftbox:
                    {
                        zen_garden.purchased.last_purchsed_silver_giftbox_time = purchases[i];
                        break;
                    }
                    case StoreItem::gold_giftbox:
                    {
                        zen_garden.purchased.last_purchsed_gold_giftbox_time = purchases[i];
                        break;
                    }
                    case StoreItem::zombotany:
                    {
                        store_value.purchased.zombotany = true;
                        break;
                    }
                    case StoreItem::minigame_pack_1:
                    {
                        store_value.purchased.minigame_pack_1 = true;
                        break;
                    }
                    case StoreItem::minigame_pack_2:
                    {
                        store_value.purchased.minigame_pack_2 = true;
                        break;
                    }
                    case StoreItem::minigame_pack_3:
                    {
                        store_value.purchased.minigame_pack_3 = true;
                        break;
                    }
                    case StoreItem::minigame_pack_4:
                    {
                        store_value.purchased.minigame_pack_4 = true;
                        break;
                    }
                    case StoreItem::minigame_pack_5:
                    {
                        store_value.purchased.minigame_pack_5 = true;
                        break;
                    }
                    case StoreItem::i_zombie_pack:
                    {
                        store_value.purchased.i_zombie_pack = true;
                        break;
                    }
                    case StoreItem::scary_potter_pack:
                    {
                        store_value.purchased.scary_potter_pack = true;
                        break;
                    }
                    case StoreItem::bacon:
                    {
                        store_value.purchased.bacon = true;
                        break;
                    }
                    case StoreItem::bacon_2:
                    {
                        store_value.purchased.bacon_2 = true;
                        break;
                    }
                    case StoreItem::last_stand_pack:
                    {
                        store_value.purchased.last_stand_pack = true;
                        break;
                    }
                    case StoreItem::hammer:
                    {
                        store_value.hammer = purchases[i];
                        break;
                    }
                    case StoreItem::pvz:
                    {
                        //Unknown
                        break;
                    }
                    default:
                    {
                        assert_conditional(false, fmt::format("invalid_store_item: {}", i), "exchange_purchased");
                    }
                    }
                }
            }
            return;
        }

        inline static auto exchange_zen_garden(
            DetailStructure const &data,
            ZenGarden &value) -> void
        {
            value.stinky.status.sleeping = static_cast<bool>(data.has_seen_stinky);
            value.stinky.status.last_chocolate = data.last_stinky_chocolate_time;
            value.stinky.position.x = data.stinky_pos_x;
            value.stinky.position.y = data.stinky_pos_y;
            value.potted_plants.resize(data.potted_plants.size());
            for (auto i : Range(value.potted_plants.size()))
            {
                auto &potted_plant = value.potted_plants[i];
                auto &potted_plant_data = data.potted_plants[i];
                potted_plant.seed_type = static_cast<SeedType>(potted_plant_data.seed_type);
                potted_plant.location = static_cast<GerdenType>(potted_plant_data.which_zen_garden);
                potted_plant.position.x = potted_plant_data.m_x;
                potted_plant.position.y = potted_plant_data.m_y;
                potted_plant.plant_status.orientation = static_cast<FacingDirection>(potted_plant_data.facing);
                potted_plant.plant_status.color = static_cast<DrawVariation>(potted_plant_data.draw_variation);
                potted_plant.plant_status.plant_age = static_cast<PottedPlantAge>(potted_plant_data.plant_age);
                potted_plant.plant_status.plant_need = static_cast<PottedPlantNeed>(potted_plant_data.plant_need);
                potted_plant.plant_status.num_of_time_watered = potted_plant_data.times_fed;
                potted_plant.plant_status.feeding_pre_grow = potted_plant_data.feedings_pergrow;
                potted_plant.plant_status.time.last_watered_time = potted_plant_data.last_watered_time;
                potted_plant.plant_status.time.last_need_ful_filled_time = potted_plant_data.last_need_fulfilled_time;
                potted_plant.plant_status.time.last_fertilized_time = potted_plant_data.last_fertilized_time;
                potted_plant.plant_status.time.last_chocolate_time = potted_plant_data.last_chocolate_time;
            }
            return;
        }

        inline static auto exchange_height_of_tree(
            std::vector<int> const &challenge_records,
            TreeOfWisdom &value) -> void
        {
            auto index = exchange_enumeration<GameMode, uint8_t>(GameMode::tree_of_wisdom);
            if (static_cast<size_t>(index) < challenge_records.size())
            {
                value.height = challenge_records[index];
            }
            return;
        }

        inline static auto exchange_challenge(
            std::vector<int> const &challenge_records,
            ChallengeRecord &value) -> void
        {
            for (auto i : Range(exchange_enumeration<GameMode, size_t>(GameMode::adventure)))
            {
                if (static_cast<bool>(challenge_records[i]))
                {
                    switch (exchange_enumeration<uint8_t, GameMode>(static_cast<uint8_t>(i)))
                    {
                    case GameMode::adventure:
                    {
                        break;
                    }
                    case GameMode::survival_normal_stage_1:
                    {
                        value.survival.day_normal_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_normal_stage_2:
                    {
                        value.survival.night_normal_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_normal_stage_3:
                    {
                        value.survival.pool_normal_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_normal_stage_4:
                    {
                        value.survival.fog_normal_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_normal_stage_5:
                    {
                        value.survival.roof_normal_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_hard_stage_1:
                    {
                        value.survival.day_hard_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_hard_stage_2:
                    {
                        value.survival.night_hard_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_hard_stage_3:
                    {
                        value.survival.pool_hard_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_hard_stage_4:
                    {
                        value.survival.fog_hard_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_hard_stage_5:
                    {
                        value.survival.roof_hard_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_endless_stage_1:
                    {
                        value.canceled_minigame.survival_day_endless_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_endless_stage_2:
                    {
                        value.canceled_minigame.survival_night_endless_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_endless_stage_3:
                    {
                        value.survival.survival_endless_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_endless_stage_4:
                    {
                        value.canceled_minigame.survival_fog_endless_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::survival_endless_stage_5:
                    {
                        value.canceled_minigame.survival_roof_endless_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::challenge_war_and_peas:
                    {
                        value.minigame.zombotany = true;
                        break;
                    }
                    case GameMode::challenge_wallnut_bowling:
                    {
                        value.minigame.wallnull_bowling = true;
                        break;
                    }
                    case GameMode::challenge_slot_machine:
                    {
                        value.minigame.slot_machine = true;
                        break;
                    }
                    case GameMode::challenge_raining_seeds:
                    {
                        value.minigame.raining_seeds = true;
                        break;
                    }
                    case GameMode::challenge_beghouled:
                    {
                        value.minigame.beghouled = true;
                        break;
                    }
                    case GameMode::challenge_invisighoul:
                    {
                        value.minigame.invisi_ghoul = true;
                        break;
                    }
                    case GameMode::challenge_seeing_stars:
                    {
                        value.minigame.seeing_stars = true;
                        break;
                    }
                    case GameMode::challenge_zombiquarium:
                    {
                        value.minigame.zombiquarium = true;
                        break;
                    }
                    case GameMode::challenge_beghouled_twist:
                    {
                        value.minigame.beghouled_twist = true;
                        break;
                    }
                    case GameMode::challenge_little_trouble:
                    {
                        value.minigame.little_trouble = true;
                        break;
                    }
                    case GameMode::challenge_portal_combat:
                    {
                        value.minigame.portal_combat = true;
                        break;
                    }
                    case GameMode::challenge_column:
                    {
                        value.minigame.column_like_see_you_em = true;
                        break;
                    }
                    case GameMode::challenge_bobsled_bonanza:
                    {
                        value.minigame.bobsled_bonanza = true;
                        break;
                    }
                    case GameMode::challenge_speed:
                    {
                        value.minigame.zombie_nimble_zombie_quick = true;
                        break;
                    }
                    case GameMode::challenge_whack_a_zombie:
                    {
                        value.minigame.whack_a_zombie = true;
                        break;
                    }
                    case GameMode::challenge_last_stand:
                    {
                        value.minigame.last_stand = challenge_records[i];
                        break;
                    }
                    case GameMode::challenge_war_and_peas_2:
                    {
                        value.minigame.zombotany_2 = true;
                        break;
                    }
                    case GameMode::challenge_wallnut_bowling_2:
                    {
                        value.minigame.wallnut_bowling_2 = true;
                        break;
                    }
                    case GameMode::challenge_pogo_party:
                    {
                        value.minigame.pogo_party = true;
                        break;
                    }
                    case GameMode::challenge_final_boss:
                    {
                        value.minigame.dr_zomboss_revenge = true;
                        break;
                    }
                    case GameMode::challenge_art_challenge_wallnut:
                    {
                        value.canceled_minigame.art_challenge_wallnut = true;
                        break;
                    }
                    case GameMode::challenge_sunny_day:
                    {
                        value.canceled_minigame.sunny_day = true;
                        break;
                    }
                    case GameMode::challenge_resodded:
                    {
                        value.canceled_minigame.unsodded = true;
                        break;
                    }
                    case GameMode::challenge_big_time:
                    {
                        value.canceled_minigame.big_time = true;
                        break;
                    }
                    case GameMode::challenge_art_challenge_sunflower:
                    {
                        value.canceled_minigame.art_challenge_sunflower = true;
                        break;
                    }
                    case GameMode::challenge_air_raid:
                    {
                        value.canceled_minigame.air_raid = true;
                        break;
                    }
                    case GameMode::challenge_ice:
                    {
                        value.canceled_minigame.ice_level = true;
                        break;
                    }
                    case GameMode::challenge_zen_garden:
                    {
                        value.canceled_minigame.zen_garden = true;
                        break;
                    }
                    case GameMode::challenge_high_gravity:
                    {
                        value.canceled_minigame.high_gravity = true;
                        break;
                    }
                    case GameMode::challenge_grave_danger:
                    {
                        value.canceled_minigame.grave_danger = true;
                        break;
                    }
                    case GameMode::challenge_shovel:
                    {
                        value.canceled_minigame.can_you_dig_it = true;
                        break;
                    }
                    case GameMode::challenge_stormy_night:
                    {
                        value.canceled_minigame.dark_stormy_night = true;
                        break;
                    }
                    case GameMode::challenge_bungee_blitz:
                    {
                        value.canceled_minigame.bungee_blitz = true;
                        break;
                    }
                    case GameMode::challenge_squirrel:
                    {
                        value.canceled_minigame.squirrel = true;
                        break;
                    }
                    case GameMode::tree_of_wisdom:
                    {
                        value.canceled_minigame.tree_of_wisdom = true;
                        break;
                    }
                    case GameMode::scary_potter_1:
                    {
                        value.puzzle.vasebreaker_1 = true;
                        break;
                    }
                    case GameMode::scary_potter_2:
                    {
                        value.puzzle.vasebreaker_2 = true;
                        break;
                    }
                    case GameMode::scary_potter_3:
                    {
                        value.puzzle.vasebreaker_3 = true;
                        break;
                    }
                    case GameMode::scary_potter_4:
                    {
                        value.puzzle.vasebreaker_4 = true;
                        break;
                    }
                    case GameMode::scary_potter_5:
                    {
                        value.puzzle.vasebreaker_5 = true;
                        break;
                    }
                    case GameMode::scary_potter_6:
                    {
                        value.puzzle.vasebreaker_6 = true;
                        break;
                    }
                    case GameMode::scary_potter_7:
                    {
                        value.puzzle.vasebreaker_7 = true;
                        break;
                    }
                    case GameMode::scary_potter_8:
                    {
                        value.puzzle.vasebreaker_8 = true;
                        break;
                    }
                    case GameMode::scary_potter_9:
                    {
                        value.puzzle.vasebreaker_9 = true;
                        break;
                    }
                    case GameMode::scary_potter_endless:
                    {
                        value.puzzle.vasebreaker_endless_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::puzzle_i_zombie_1:
                    {
                        value.puzzle.i_zombie_1 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_2:
                    {
                        value.puzzle.i_zombie_2 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_3:
                    {
                        value.puzzle.i_zombie_3 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_4:
                    {
                        value.puzzle.i_zombie_4 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_5:
                    {
                        value.puzzle.i_zombie_5 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_6:
                    {
                        value.puzzle.i_zombie_6 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_7:
                    {
                        value.puzzle.i_zombie_7 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_8:
                    {
                        value.puzzle.i_zombie_8 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_9:
                    {
                        value.puzzle.i_zombie_9 = true;
                        break;
                    }
                    case GameMode::puzzle_i_zombie_endless:
                    {
                        value.puzzle.i_zombie_endless_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::upsell:
                    {
                        value.canceled_minigame.upsell = true;
                        break;
                    }
                    case GameMode::intro:
                    {
                        value.canceled_minigame.intro = true;
                        break;
                    }
                    case GameMode::last_stand_stage_1:
                    {
                        value.last_stand.day_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::last_stand_stage_2:
                    {
                        value.last_stand.night_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::last_stand_stage_3:
                    {
                        value.last_stand.pool_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::last_stand_stage_4:
                    {
                        value.last_stand.fog_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::last_stand_stage_5:
                    {
                        value.last_stand.roof_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::last_stand_endless:
                    {
                        value.last_stand.endless_flag = challenge_records[i];
                        break;
                    }
                    case GameMode::unknown:
                    {
                        break;
                    }
                    default:
                    {
                        assert_conditional(false, fmt::format("invalid_gamemode_type: {}", i), "exchange_challenge");
                    }
                    }
                }
            }
            return;
        }

        inline static auto exchange_achievement(
            DetailStructure const &data,
            Achievement &value) -> void
        {
            value.num_of_achievement = data.earned_achievements.size();
            for (auto i : Range(data.earned_achievements.size()))
            {
                switch (exchange_enumeration<uint8_t, AchievementId>(static_cast<uint8_t>(i)))
                {
                case AchievementId::home_security:
                {
                    value.home_lawn_security.earned = data.earned_achievements[i];
                    value.home_lawn_security.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::nobel_peas_prize:
                {
                    value.nobel_peas_prize.earned = data.earned_achievements[i];
                    value.nobel_peas_prize.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::better_off_dead:
                {
                    value.better_off_dead.earned = data.earned_achievements[i];
                    value.better_off_dead.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::china_shop:
                {
                    value.china_shop.earned = data.earned_achievements[i];
                    value.china_shop.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::spudow:
                {
                    value.spudow.earned = data.earned_achievements[i];
                    value.spudow.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::explodonator:
                {
                    value.explodonator.earned = data.earned_achievements[i];
                    value.explodonator.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::morticulturalist:
                {
                    value.morticulturalist.earned = data.earned_achievements[i];
                    value.morticulturalist.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::dont_pea:
                {
                    value.dont_pea_in_the_pool.earned = data.earned_achievements[i];
                    value.dont_pea_in_the_pool.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::roll_some_heads:
                {
                    value.roll_some_heads.earned = data.earned_achievements[i];
                    value.roll_some_heads.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::grounded:
                {
                    value.grounded.earned = data.earned_achievements[i];
                    value.grounded.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::zombologist:
                {
                    value.zombologist.earned = data.earned_achievements[i];
                    value.zombologist.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::penny_pincher:
                {
                    value.penny_pincher.earned = data.earned_achievements[i];
                    value.penny_pincher.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::sunny_days:
                {
                    value.sunny_days.earned = data.earned_achievements[i];
                    value.sunny_days.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::popcorn_party:
                {
                    value.popcorn_party.earned = data.earned_achievements[i];
                    value.popcorn_party.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::good_morning:
                {
                    value.good_morning.earned = data.earned_achievements[i];
                    value.good_morning.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::no_fungus_amongus:
                {
                    value.no_fungus_among_us.earned = data.earned_achievements[i];
                    value.no_fungus_among_us.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::beyond_the_grave:
                {
                    value.beyond_the_grave.earned = data.earned_achievements[i];
                    value.beyond_the_grave.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::immortal:
                {
                    value.immortal.earned = data.earned_achievements[i];
                    value.immortal.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::towering_wisdom:
                {
                    value.towering_wisdom.earned = data.earned_achievements[i];
                    value.towering_wisdom.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::mustache_mode:
                {
                    value.mustache_mode.earned = data.earned_achievements[i];
                    value.mustache_mode.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::last_mown_stading:
                {
                    value.last_mown_stading.earned = data.earned_achievements[i];
                    value.last_mown_stading.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::below_zero:
                {
                    value.below_zero.earned = data.earned_achievements[i];
                    value.below_zero.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::flower_power:
                {
                    value.flower_power.earned = data.earned_achievements[i];
                    value.flower_power.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::pyromaniac:
                {
                    value.pyromaniac.earned = data.earned_achievements[i];
                    value.pyromaniac.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::lawn_mowner_man:
                {
                    value.lawn_mowner_man.earned = data.earned_achievements[i];
                    value.lawn_mowner_man.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::chill_out:
                {
                    value.chill_out.earned = data.earned_achievements[i];
                    value.chill_out.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::defcorn_five:
                {
                    value.defcorn_five.earned = data.earned_achievements[i];
                    value.defcorn_five.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::monster_mash:
                {
                    value.monster_mash.earned = data.earned_achievements[i];
                    value.monster_mash.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::blind_faith:
                {
                    value.blind_faith.earned = data.earned_achievements[i];
                    value.blind_faith.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::pool_closed:
                {
                    value.pool_closed.earned = data.earned_achievements[i];
                    value.pool_closed.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::melony_lane:
                {
                    value.melony_lane.earned = data.earned_achievements[i];
                    value.melony_lane.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::second_life:
                {
                    value.second_life.earned = data.earned_achievements[i];
                    value.second_life.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::lucky_spin:
                {
                    value.lucky_spin.earned = data.earned_achievements[i];
                    value.lucky_spin.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::chilli_free:
                {
                    value.chilli_free.earned = data.earned_achievements[i];
                    value.chilli_free.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::enlightened:
                {
                    value.enlightened.earned = data.earned_achievements[i];
                    value.enlightened.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::diamond_beghouler:
                {
                    value.diamond_beghouler.earned = data.earned_achievements[i];
                    value.diamond_beghouler.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::greenfingers:
                {
                    value.greenfingers.earned = data.earned_achievements[i];
                    value.greenfingers.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::wallnot_attack:
                {
                    value.wallnot_attack.earned = data.earned_achievements[i];
                    value.wallnot_attack.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::peking_express:
                {
                    value.peking_express.earned = data.earned_achievements[i];
                    value.peking_express.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::shooting_star:
                {
                    value.shooting_star.earned = data.earned_achievements[i];
                    value.shooting_star.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::sol_invictus:
                {
                    value.sol_invictus.earned = data.earned_achievements[i];
                    value.sol_invictus.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::sproing_sproing:
                {
                    value.sproing_sproing.earned = data.earned_achievements[i];
                    value.sproing_sproing.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::i_win:
                {
                    value.i_win.earned = data.earned_achievements[i];
                    value.i_win.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::where_the_sun_dont_shine:
                {
                    value.where_the_sun_dont_shine.earned = data.earned_achievements[i];
                    value.where_the_sun_dont_shine.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::undead_space:
                {
                    value.undead_space.earned = data.earned_achievements[i];
                    value.undead_space.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::disco_is_undead:
                {
                    value.disco_is_undead.earned = data.earned_achievements[i];
                    value.disco_is_undead.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::smashing:
                {
                    value.smashing.earned = data.earned_achievements[i];
                    value.smashing.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::gotcha:
                {
                    value.gotcha.earned = data.earned_achievements[i];
                    value.gotcha.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::still_standing:
                {
                    value.still_standing.earned = data.earned_achievements[i];
                    value.still_standing.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::the_stuff_of_legends:
                {
                    value.the_stuff_of_legends.earned = data.earned_achievements[i];
                    value.the_stuff_of_legends.shown = data.shown_achievements[i];
                    break;
                }
                case AchievementId::hammer_time:
                {
                    value.hammer_time.earned = data.earned_achievements[i];
                    value.hammer_time.shown = data.shown_achievements[i];
                    break;
                }
                default:
                {
                    assert_conditional(false, "invalid_achievement_id", "exchange_challenge");
                }
                }
            }
            return;
        }

        inline static auto exchange_zombatar(
            DetailStructure const &data,
            Zombatar &value) -> void
        {
            value.accepted_zombatar_license = data.accepted_zombatar_license;
            value.value.resize(data.zombatars.size());
            for (auto i : Range(data.zombatars.size()))
            {
                auto &zombatar_data = data.zombatars[i];
                auto &zombatar = value.value[i];
                zombatar.skin_color = zombatar_data.skin_color;
                zombatar.clothes.type = zombatar_data.clothes_type;
                zombatar.clothes.color = zombatar_data.clothes_color;
                zombatar.tidbits.type = zombatar_data.tidbits_type;
                zombatar.tidbits.color = zombatar_data.tidbits_color;
                zombatar.accessories.type = zombatar_data.accessories_type;
                zombatar.accessories.color = zombatar_data.accessories_color;
                zombatar.facial_hair.type = zombatar_data.facial_hair_type;
                zombatar.facial_hair.color = zombatar_data.facial_hair_color;
                zombatar.hair.type = zombatar_data.hair_type;
                zombatar.hair.color = zombatar_data.hair_color;
                zombatar.eyewear.type = zombatar_data.eyewear_type;
                zombatar.eyewear.color = zombatar_data.eyewear_color;
                zombatar.hat.type = zombatar_data.hat_type;
                zombatar.hat.color = zombatar_data.hat_color;
                zombatar.backdrop.type = zombatar_data.backdrop_type;
                zombatar.backdrop.color = zombatar_data.backdrop_color;
            }
            value.display_save_image_messenge = data.display_save_image_messenge;
            return;
        }

        inline static auto exchange_detail_info(
            DetailStructure const &data,
            DetailInfo &value) -> void
        {
            exchange_detail_general(data, value.general);
            exchange_purchased(data.purchases, value.store, value.zen_garden);
            if (data.has_zen_garden)
            {
                exchange_zen_garden(data, value.zen_garden);
                if (k_version < 30) {
                    exchange_height_of_tree(data.challenge_records, value.zen_garden.tree_of_wisdom);
                }
                value.has_zen_garden = true;
            }
            if (k_version < 13 || k_version >= 30)
            {
                exchange_detail_unlocked(data, value.unlocked);
                value.has_unlocked = true;
                exchange_challenge(data.challenge_records, value.trophies);
                value.has_trophies = true;
            }
            if (data.has_achievement)
            {
                exchange_achievement(data, value.achievement);
                value.has_achievement = true;
            }
            if (data.has_zombatar)
            {
                exchange_zombatar(data, value.zombatar);
                value.has_zombatar = true;
            }
            if (!data.expand.empty())
            {
                value.unknown_expand = exchange_data(data.expand);
            }
            return;
        }

        inline static auto exchange_player_info(
            DataStreamView &stream,
            DetailInfo &definition) -> void
        {
            auto detail_structure = DetailStructure{};
            exchange_detail_structure(stream, detail_structure);
            exchange_detail_info(detail_structure, definition);
            return;
        }
        
        inline static auto exchange_summary_info(
            DataStreamView &stream,
            SummaryInfo &value
        ) -> void
        {
            exchange_list(stream, value.users, [] (auto &data, auto &value) {
                value.name = data.readStringByUint16();
                value.use_seq = data.readInt32();
                value.id = data.readInt32();
            }, stream.readUint16());
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            std::string_view destination) -> void
        {
            exchange_to_version(stream, k_version);
            if (k_version == 14)
            {
                auto definition = SummaryInfo{
                    .version = k_version
                };
                exchange_summary_info(stream, definition);
                FileSystem::write_json(destination, definition);
            }
            else
            {
                auto definition = DetailInfo{
                    .version = k_version
                };
                exchange_player_info(stream, definition);
                FileSystem::write_json(destination, definition);
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{source};
            process_whole(stream, destination);
            return;
        }
    };
}