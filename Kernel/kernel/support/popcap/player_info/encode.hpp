#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/popcap/player_info/definition.hpp"
#include "kernel/support/popcap/player_info/common.hpp"

namespace Sen::Kernel::Support::PopCap::PlayerInfo
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Encode : Common
    {
    private:
        inline static auto exchange_data(
            std::string const &value) -> std::vector<uint8_t>
        {
            auto hexString = value;
            hexString.erase(std::remove(hexString.begin(), hexString.end(), ' '), hexString.end());
            auto bytes = std::vector<uint8_t>{};
            for (auto i = static_cast<unsigned int>(0); i < hexString.length(); i += 2)
            {
                auto byteString = hexString.substr(i, 2);
                auto byte = static_cast<std::uint8_t>(std::strtol(byteString.data(), NULL, 16));
                bytes.push_back(byte);
            }
            return bytes;
        }

        inline static auto exchange_detail_by_version(
            DataStreamView &data,
            DetailStructure const &value) -> void
        {
            if (k_version < 13)
            {
                exchange_list<false, uint32_t>(data, value.challenge_records, [](auto &data, auto &value)
                                               { data.writeInt32(value); });
                data.writeNull((exchange_challenge_records_count() - value.challenge_records.size()) * k_block_size);
                exchange_list<false, uint32_t>(data, value.purchases, [](auto &data, auto &value)
                                               { data.writeInt32(value); });
                data.writeNull((exchange_purchases_count() - value.purchases.size()) * k_block_size);
                data.writeInt32(value.play_time_active_player);
                data.writeInt32(value.play_time_inactive_player);
                data.writeInt32(value.has_used_cheat_keys);
                data.writeInt32(value.has_woken_stiky);
                data.writeInt32(value.didnt_purchase_packet_upgrade);
                data.writeInt32(value.last_stinky_chocolate_time);
                data.writeInt32(value.stinky_pos_x);
                data.writeInt32(value.stinky_pos_y);
                data.writeInt32(value.has_unlocked_minigames);
                data.writeInt32(value.has_unlocked_puzzle_mode);
                data.writeInt32(value.has_new_minigame);
                data.writeInt32(value.has_new_scary_potter);
                data.writeInt32(value.has_new_survival);
                data.writeInt32(value.has_new_i_zombie);
                data.writeInt32(value.has_new_limbo_page);
                data.writeInt32(value.need_message_on_game_seletor);
                data.writeInt32(value.need_magic_taco_reward);
                data.writeInt32(value.has_seen_stinky);
                data.writeInt32(value.has_seen_upsell);
                data.writeInt32(value.place_holder_player_stats);
                if (value.has_zen_garden)
                {
                    exchange_list<true, uint32_t>(data, value.potted_plants, &exchange_from_potted_plants);
                }
                if (value.has_achievement)
                {
                    for (auto i : Range(exchange_enumeration<AchievementId, size_t>(AchievementId::home_security)))
                    {
                        data.writeBoolean(value.earned_achievements[i]);
                        data.writeBoolean(value.shown_achievements[i]);
                    }
                }
                if (value.has_zombatar)
                {
                    data.writeBoolean(value.accepted_zombatar_license);
                    exchange_list<true, uint32_t>(data, value.zombatars, &exchange_from_zombatar_struct);
                    data.read_pos += 0x14_size;
                    data.writeNull(0x14_size);
                    data.writeBoolean(value.display_save_image_messenge);
                }
            }
            else if (k_version < 30)
            {
                data.writeNull(exchange_challenge_records_count() * k_block_size);
                exchange_list<false, uint32_t>(data, value.purchases, [](auto &data, auto &value)
                                               { data.writeInt32(value); });
                data.writeNull((exchange_purchases_count() - value.purchases.size()) * k_block_size);
                if (value.has_achievement)
                {
                    for (auto i : Range(value.earned_achievements.size())) 
                    {
                        data.writeBoolean(value.earned_achievements[i]);
                    }
                    for (auto i : Range(value.shown_achievements.size()))
                    {
                        data.writeBoolean(value.shown_achievements[i]);
                    }
                }
                data.writeBytes(value.expand);
            }
            else
            {
                exchange_list<false, uint32_t>(data, value.challenge_records, [](auto &data, auto &value)
                                               { data.writeInt32(value); });
                data.writeNull((exchange_challenge_records_count() - value.challenge_records.size()) * k_block_size);
                exchange_list<false, uint32_t>(data, value.purchases, [](auto &data, auto &value)
                                               { data.writeInt32(value); });
                data.writeNull((exchange_purchases_count() - value.purchases.size()) * k_block_size);
                data.writeInt32(value.play_time_active_player);
                data.writeInt32(value.play_time_inactive_player);
                data.writeInt32(value.has_used_cheat_keys);
                data.writeInt32(value.has_woken_stiky);
                data.writeInt32(value.didnt_purchase_packet_upgrade);
                data.writeInt32(value.last_stinky_chocolate_time);
                data.writeInt32(value.stinky_pos_x);
                data.writeInt32(value.stinky_pos_y);
                data.writeInt32(value.has_unlocked_minigames);
                data.writeInt32(value.has_unlocked_puzzle_mode);
                data.writeInt32(value.has_new_minigame);
                data.writeInt32(value.has_new_scary_potter);
                data.writeInt32(value.has_new_survival);
                data.writeInt32(value.has_new_i_zombie);
                data.writeInt32(value.has_new_limbo_page);
                data.writeInt32(value.need_message_on_game_seletor);
                data.writeInt32(value.need_magic_taco_reward);
                data.writeInt32(value.has_seen_stinky);
                data.writeInt32(value.has_seen_upsell);
                data.writeInt32(value.place_holder_player_stats);
                data.writeInt32(0);
                if (value.has_zen_garden)
                {
                    exchange_list<true, uint32_t>(data, value.potted_plants, &exchange_from_potted_plants);
                }
                if (value.has_achievement)
                {
                    for (auto i : Range(value.earned_achievements.size())) 
                    {
                        data.writeBoolean(value.earned_achievements[i]);
                    }
                    for (auto i : Range(value.shown_achievements.size())) 
                    {
                        data.writeBoolean(value.shown_achievements[i]);
                    }
                }
                data.writeBytes(value.expand);
            }
        }

        inline static auto exchange_detail_structure(
            DataStreamView &data,
            DetailStructure const &value) -> void
        {
            data.writeInt32(value.level);
            data.writeInt32(value.coins);
            data.writeInt32(value.finished_adventure);
            exchange_detail_by_version(data, value);
            return;
        }

        inline static auto exchange_detail_general(
            General const &data,
            DetailStructure &value) -> void
        {
            value.level = data.adventure_level;
            value.coins = data.coins / k_coins_ratio;
            value.need_message_on_game_seletor = static_cast<int>(data.adventure_compeled);
            value.finished_adventure = data.completed_adventure_time;
            value.play_time_active_player = data.player_time_active_player;
            value.play_time_inactive_player = data.player_time_inactive_player;
            value.place_holder_player_stats = data.place_holder_player_stats;
            value.need_magic_taco_reward = static_cast<int>(data.has_magic_taco_reward);
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
            Store const &store_value,
            ZenGarden const &zen_garden,
            std::vector<int> &purchases
        ) -> void
        {
            purchases.resize(exchange_enumeration<StoreItem, size_t>(StoreItem::pvz));
            if (store_value.purchased.plant_gatling_pea) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_gatlingpea)] = 1;
            }
            if (store_value.purchased.plant_twin_sunflower) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_twinsunflower)] = 1;
            }
            if (store_value.purchased.plant_gloomshroom) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_gloomshroom)] = 1;
            }
            if (store_value.purchased.plant_cattail) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_cattail)] = 1;
            }
            if (store_value.purchased.plant_winter_melon) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_wintermelon)] = 1;
            }
            if (store_value.purchased.plant_gold_magnet) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_gold_magnet)] = 1;
            }
            if (store_value.purchased.plant_sprike_rock) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_spikerock)] = 1;
            }
            if (store_value.purchased.plant_cob_cannon) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_cobcannon)] = 1;
            }
            if (store_value.purchased.plant_imitater) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::plant_imitater)] = 1;
            }
            if (store_value.purchased.bonus_lawn_mower) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::bonus_lawn_mower)] = 1;
            }
            if (zen_garden.purchased.last_purchesed_marigold_1_time) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::potted_marigold_1)] = zen_garden.purchased.last_purchesed_marigold_1_time;
            }
            if (zen_garden.purchased.last_purchesed_marigold_2_time) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::potted_marigold_2)] = zen_garden.purchased.last_purchesed_marigold_2_time;
            }
            if (zen_garden.purchased.last_purchesed_marigold_3_time) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::potted_marigold_3)] = zen_garden.purchased.last_purchesed_marigold_3_time;
            }
            if (zen_garden.purchased.gold_watering_can) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::gold_wateringcan)] = 1;
            }
            if (zen_garden.purchased.fertilizer) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::fertilizer)] = zen_garden.purchased.fertilizer + k_amount_ratio;
            }
            if (zen_garden.purchased.bug_spray) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::bug_spray)] = zen_garden.purchased.bug_spray + k_amount_ratio;
            }
            if (zen_garden.purchased.phonograph) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::phonograph)] = 1;
            }
            if (zen_garden.purchased.gardening_glove) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::gardening_glove)] = 1;
            }
            if (zen_garden.purchased.mushroom_garden) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::mushroom_garden)] = 1;
            }
            if (zen_garden.purchased.wheel_barrow) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::wheel_barrow)] = 1;
            }
            if (zen_garden.stinky.purchased) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::stinky_the_snail)] = zen_garden.stinky.status.last_awoken;
            }
            if (store_value.extra_packet_slots) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::packet_upgrade)] = exchange_range<0, 4>(store_value.extra_packet_slots);
            }
            if (store_value.uses_of_rake) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::rake)] = exchange_range<0, 3>(store_value.uses_of_rake);
            }
            if (store_value.purchased.pool_cleaner) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::pool_cleaner)] = 1;
            }
            if (store_value.purchased.roof_cleaner) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::roof_cleaner)] = 1;
            }
            if (zen_garden.purchased.aquarium_garden) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::aquarium_garden)] = 1;
            }
            if (zen_garden.purchased.chocolate) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::chocolate)] = zen_garden.purchased.chocolate + k_amount_ratio;
            }
            if (zen_garden.tree_of_wisdom.purchased) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::tree_of_wisdom)] = 1;
            }
            if (zen_garden.tree_of_wisdom.tree_food) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::tree_food)] = zen_garden.tree_of_wisdom.tree_food + k_amount_ratio;
            }
            if (store_value.purchased.first_aid) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::firstaid)] = 1;
            }
            if (zen_garden.purchased.last_purchsed_basic_giftbox_time) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::basic_giftbox)] = zen_garden.purchased.last_purchsed_basic_giftbox_time;
            }
            if (zen_garden.purchased.last_purchsed_bronze_giftboxtime) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::bronze_giftbox)] = zen_garden.purchased.last_purchsed_bronze_giftboxtime;
            }
            if (zen_garden.purchased.last_purchsed_silver_giftbox_time) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::silver_giftbox)] = zen_garden.purchased.last_purchsed_silver_giftbox_time;
            }
            if (zen_garden.purchased.last_purchsed_gold_giftbox_time) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::gold_giftbox)] = zen_garden.purchased.last_purchsed_gold_giftbox_time;
            }
            if (store_value.purchased.zombotany) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::zombotany)] = 1;
            }
            if (store_value.purchased.minigame_pack_1) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::minigame_pack_1)] = 1;
            }
            if (store_value.purchased.minigame_pack_2) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::minigame_pack_2)] = 1;
            }
            if (store_value.purchased.minigame_pack_3) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::minigame_pack_3)] = 1;
            }
            if (store_value.purchased.minigame_pack_4) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::minigame_pack_4)] = 1;
            }
            if (store_value.purchased.minigame_pack_5) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::minigame_pack_5)] = 1;
            }
            if (store_value.purchased.i_zombie_pack) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::i_zombie_pack)] = 1;
            }
            if (store_value.purchased.scary_potter_pack) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::scary_potter_pack)] = 1;
            }
            if (store_value.purchased.bacon) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::bacon)] = 1;
            }
            if (store_value.purchased.bacon_2) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::bacon_2)] = 1;
            }
            if (store_value.purchased.last_stand_pack) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::last_stand_pack)] = 1;
            }
            if (store_value.hammer) {
                purchases[exchange_enumeration<StoreItem, uint8_t>(StoreItem::hammer)] = store_value.hammer;
            }
            return;
        }

        inline static auto exchange_zen_garden(
            ZenGarden const &data,
            DetailStructure &value) -> void
        {
            value.has_seen_stinky = static_cast<int>(data.stinky.status.sleeping);
            value.last_stinky_chocolate_time = data.stinky.status.last_chocolate;
            value.stinky_pos_x = data.stinky.position.x;
            value.stinky_pos_y = data.stinky.position.y;
            value.potted_plants.resize(data.potted_plants.size());
            for (auto i : Range(value.potted_plants.size()))
            {
                auto &potted_plant = value.potted_plants[i];
                auto &potted_plant_data = data.potted_plants[i];
                potted_plant.seed_type = static_cast<int>(potted_plant_data.seed_type);
                potted_plant.which_zen_garden = static_cast<int>(potted_plant_data.location);
                potted_plant.m_x = potted_plant_data.position.x;
                potted_plant.m_y = potted_plant_data.position.y;
                potted_plant.facing = static_cast<int>(potted_plant_data.plant_status.orientation);
                potted_plant.draw_variation = static_cast<int>(potted_plant_data.plant_status.color);
                potted_plant.plant_age = static_cast<int>(potted_plant_data.plant_status.plant_age);
                potted_plant.plant_need = static_cast<int>(potted_plant_data.plant_status.plant_need);
                potted_plant.times_fed = potted_plant_data.plant_status.num_of_time_watered;
                potted_plant.feedings_pergrow = potted_plant_data.plant_status.feeding_pre_grow;
                potted_plant.last_watered_time = potted_plant_data.plant_status.time.last_watered_time;
                potted_plant.last_need_fulfilled_time = potted_plant_data.plant_status.time.last_need_ful_filled_time;
                potted_plant.last_fertilized_time = potted_plant_data.plant_status.time.last_fertilized_time;
                potted_plant.last_chocolate_time = potted_plant_data.plant_status.time.last_chocolate_time;
            }
            return;
        }

        inline static auto exchange_height_of_tree(
            TreeOfWisdom const &data,
            std::vector<int> &challenge_records
            ) -> void
        {
            auto index = exchange_enumeration<GameMode, uint8_t>(GameMode::tree_of_wisdom);
            if (static_cast<size_t>(index) < challenge_records.size())
            {
                challenge_records[index] = data.height;
            }
            return;
        }

        inline static auto exchange_detail_unlocked(
            Unlocked const &data,
            DetailStructure &value
        ) -> void
        {
            value.has_unlocked_minigames = static_cast<int>(data.minigame);
            value.has_unlocked_puzzle_mode = static_cast<int>(data.puzzle_mode);
            value.has_new_minigame = static_cast<int>(data.first_time_play_minigame);
            value.has_new_scary_potter = static_cast<int>(data.first_time_play_vasebreaker);
            value.has_new_i_zombie = static_cast<int>(data.first_time_play_i_zombie);
            value.has_new_survival = static_cast<int>(data.first_time_play_survival);
            return;
        }

        inline static auto exchange_challenge(
            ChallengeRecord const &data,
            std::vector<int> &value) -> void
        {
            value.resize(exchange_enumeration<GameMode, size_t>(GameMode::adventure));
            if (data.survival.day_normal_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_normal_stage_1)] = exchange_range<0, 5>(data.survival.day_normal_flag);
            }
            if (data.survival.night_normal_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_normal_stage_2)] = exchange_range<0, 5>(data.survival.night_normal_flag);
            }
            if (data.survival.pool_normal_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_normal_stage_3)] = exchange_range<0, 5>(data.survival.pool_normal_flag);
            }
            if (data.survival.fog_normal_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_normal_stage_4)] = exchange_range<0, 5>(data.survival.fog_normal_flag);
            }
            if (data.survival.roof_normal_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_normal_stage_5)] = exchange_range<0, 5>(data.survival.roof_normal_flag);
            }
            if (data.survival.day_hard_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_hard_stage_1)] = exchange_range<0, 10>(data.survival.day_hard_flag);
            }
            if (data.survival.night_hard_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_hard_stage_2)] = exchange_range<0, 10>(data.survival.night_hard_flag);
            }
            if (data.survival.pool_hard_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_hard_stage_3)] = exchange_range<0, 10>(data.survival.pool_hard_flag);
            }
            if (data.survival.fog_hard_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_hard_stage_4)] = exchange_range<0, 10>(data.survival.fog_hard_flag);
            }
            if (data.survival.roof_hard_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_hard_stage_5)] = exchange_range<0, 10>(data.survival.roof_hard_flag);
            }
            if (data.canceled_minigame.survival_day_endless_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_endless_stage_1)] = data.canceled_minigame.survival_day_endless_flag;
            }
            if (data.canceled_minigame.survival_night_endless_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_endless_stage_2)] = data.canceled_minigame.survival_night_endless_flag;
            }
            if (data.survival.survival_endless_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_endless_stage_3)] = data.survival.survival_endless_flag;
            }
            if (data.canceled_minigame.survival_fog_endless_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_endless_stage_4)] = data.canceled_minigame.survival_fog_endless_flag;
            }
            if (data.canceled_minigame.survival_roof_endless_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::survival_endless_stage_5)] = data.canceled_minigame.survival_roof_endless_flag;
            }
            if (data.minigame.zombotany) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_war_and_peas)] = 1;
            }
            if (data.minigame.wallnull_bowling) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_wallnut_bowling)] = 1;
            }
            if (data.minigame.slot_machine) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_slot_machine)] = 1;
            }
            if (data.minigame.raining_seeds) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_raining_seeds)] = 1;
            }
            if (data.minigame.beghouled) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_beghouled)] = 1;
            }
            if (data.minigame.invisi_ghoul) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_invisighoul)] = 1;
            }
            if (data.minigame.seeing_stars) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_seeing_stars)] = 1;
            }
            if (data.minigame.zombiquarium) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_zombiquarium)] = 1;
            }
            if (data.minigame.beghouled_twist) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_beghouled_twist)] = 1;
            }
            if (data.minigame.little_trouble) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_little_trouble)] = 1;
            }
            if (data.minigame.portal_combat) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_portal_combat)] = 1;
            }
            if (data.minigame.column_like_see_you_em) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_column)] = 1;
            }
            if (data.minigame.bobsled_bonanza) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_bobsled_bonanza)] = 1;
            }
            if (data.minigame.zombie_nimble_zombie_quick) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_speed)] = 1;
            }
            if (data.minigame.whack_a_zombie) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_whack_a_zombie)] = 1;
            }
            if (data.minigame.last_stand) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_last_stand)] = data.minigame.last_stand;
            }
            if (data.minigame.zombotany_2) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_war_and_peas_2)] = 1;
            }
            if (data.minigame.wallnut_bowling_2) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_wallnut_bowling_2)] = 1;
            }
            if (data.minigame.pogo_party) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_pogo_party)] = 1;
            }
            if (data.minigame.dr_zomboss_revenge) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_final_boss)] = 1;
            }
            if (data.canceled_minigame.art_challenge_wallnut) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_art_challenge_wallnut)] = 1;
            }
            if (data.canceled_minigame.sunny_day) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_sunny_day)] = 1;
            }
            if (data.canceled_minigame.unsodded) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_resodded)] = 1;
            }
            if (data.canceled_minigame.big_time) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_big_time)] = 1;
            }
            if (data.canceled_minigame.art_challenge_sunflower) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_art_challenge_sunflower)] = 1;
            }
            if (data.canceled_minigame.air_raid) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_air_raid)] = 1;
            }
            if (data.canceled_minigame.ice_level) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_ice)] = 1;
            }
            if (data.canceled_minigame.zen_garden) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_zen_garden)] = 1;
            }
            if (data.canceled_minigame.high_gravity) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_high_gravity)] = 1;
            }
            if (data.canceled_minigame.grave_danger) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_grave_danger)] = 1;
            }
            if (data.canceled_minigame.can_you_dig_it) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_shovel)] = 1;
            }
            if (data.canceled_minigame.dark_stormy_night) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_stormy_night)] = 1;
            }
            if (data.canceled_minigame.bungee_blitz) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_bungee_blitz)] = 1;
            }
            if (data.canceled_minigame.squirrel) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::challenge_squirrel)] = 1;
            }
            if (data.canceled_minigame.upsell) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::upsell)] = 1;
            }
            if (data.canceled_minigame.intro) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::intro)] = 1;
            }
            if (data.canceled_minigame.tree_of_wisdom) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::tree_of_wisdom)] = 1;
            }
            if (data.puzzle.vasebreaker_1) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_1)] = 1;
            }
            if (data.puzzle.vasebreaker_2) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_2)] = 1;
            }
            if (data.puzzle.vasebreaker_3) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_3)] = 1;
            }
            if (data.puzzle.vasebreaker_4) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_4)] = 1;
            }
            if (data.puzzle.vasebreaker_5) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_5)] = 1;
            }
            if (data.puzzle.vasebreaker_6) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_6)] = 1;
            }
            if (data.puzzle.vasebreaker_7) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_7)] = 1;
            }
            if (data.puzzle.vasebreaker_8) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_8)] = 1;
            }
            if (data.puzzle.vasebreaker_9) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_9)] = 1;
            }
            if (data.puzzle.vasebreaker_endless_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::scary_potter_endless)] = data.puzzle.vasebreaker_endless_flag;
            }
            if (data.puzzle.i_zombie_1) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_1)] = 1;
            }
            if (data.puzzle.i_zombie_2) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_2)] = 1;
            }
            if (data.puzzle.i_zombie_3) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_3)] = 1;
            }
            if (data.puzzle.i_zombie_4) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_4)] = 1;
            }
            if (data.puzzle.i_zombie_5) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_5)] = 1;
            }
            if (data.puzzle.i_zombie_6) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_6)] = 1;
            }
            if (data.puzzle.i_zombie_7) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_7)] = 1;
            }
            if (data.puzzle.i_zombie_8) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_8)] = 1;
            }
            if (data.puzzle.i_zombie_9) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_9)] = 1;
            }
            if (data.puzzle.i_zombie_endless_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::puzzle_i_zombie_endless)] = data.puzzle.i_zombie_endless_flag;
            }
            if (data.last_stand.day_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::last_stand_stage_1)] = data.last_stand.day_flag;
            }
            if (data.last_stand.night_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::last_stand_stage_2)] = data.last_stand.night_flag;
            }
            if (data.last_stand.pool_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::last_stand_stage_3)] = data.last_stand.pool_flag;
            }
            if (data.last_stand.fog_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::last_stand_stage_4)] = data.last_stand.fog_flag;
            }
            if (data.last_stand.roof_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::last_stand_stage_5)] = data.last_stand.roof_flag;
            }
            if (data.last_stand.endless_flag) {
                value[exchange_enumeration<GameMode, uint8_t>(GameMode::last_stand_endless)] = data.last_stand.endless_flag;
            }
            return;
        }

        inline static auto exchange_achievement(
            Achievement const &data,
            DetailStructure &value) -> void
        {
            auto num_of_achievement = exchange_enumeration<AchievementId, size_t>(AchievementId::home_security);
            if (k_version == 13) {
                num_of_achievement = data.num_of_achievement;
            }
            value.earned_achievements.resize(num_of_achievement);
            value.shown_achievements.resize(num_of_achievement);
            //earned
            if (data.home_lawn_security.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::home_security)] = true;
            }
            if (data.nobel_peas_prize.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::nobel_peas_prize)] = true;
            }
            if (data.better_off_dead.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::better_off_dead)] = true;
            }
            if (data.china_shop.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::china_shop)] = true;
            }
            if (data.spudow.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::spudow)] = true;
            }
            if (data.explodonator.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::explodonator)] = true;
            }
            if (data.morticulturalist.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::morticulturalist)] = true;
            }
            if (data.dont_pea_in_the_pool.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::dont_pea)] = true;
            }
            if (data.roll_some_heads.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::roll_some_heads)] = true;
            }
            if (data.grounded.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::grounded)] = true;
            }
            if (data.zombologist.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::zombologist)] = true;
            }
            if (data.penny_pincher.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::penny_pincher)] = true;
            }
            if (data.sunny_days.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::sunny_days)] = true;
            }
            if (data.popcorn_party.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::popcorn_party)] = true;
            }
            if (data.good_morning.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::good_morning)] = true;
            }
            if (data.no_fungus_among_us.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::no_fungus_amongus)] = true;
            }
            if (data.beyond_the_grave.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::beyond_the_grave)] = true;
            }
            if (data.immortal.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::immortal)] = true;
            }
            if (data.towering_wisdom.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::towering_wisdom)] = true;
            }
            if (data.mustache_mode.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::mustache_mode)] = true;
            }
            if (data.last_mown_stading.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::last_mown_stading)] = true;
            }
            if (data.below_zero.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::below_zero)] = true;
            }
            if (data.flower_power.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::flower_power)] = true;
            }
            if (data.pyromaniac.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::pyromaniac)] = true;
            }
            if (data.lawn_mowner_man.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::lawn_mowner_man)] = true;
            }
            if (data.chill_out.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::chill_out)] = true;
            }
            if (data.defcorn_five.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::defcorn_five)] = true;
            }
            if (data.monster_mash.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::monster_mash)] = true;
            }
            if (data.blind_faith.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::blind_faith)] = true;
            }
            if (data.pool_closed.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::pool_closed)] = true;
            }
            if (data.melony_lane.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::melony_lane)] = true;
            }
            if (data.second_life.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::second_life)] = true;
            }
            if (data.lucky_spin.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::lucky_spin)] = true;
            }
            if (data.chilli_free.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::chilli_free)] = true;
            }
            if (data.enlightened.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::enlightened)] = true;
            }
            if (data.diamond_beghouler.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::diamond_beghouler)] = true;
            }
            if (data.greenfingers.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::greenfingers)] = true;
            }
            if (data.wallnot_attack.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::wallnot_attack)] = true;
            }
            if (data.peking_express.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::peking_express)] = true;
            }
            if (data.shooting_star.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::shooting_star)] = true;
            }
            if (data.sol_invictus.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::sol_invictus)] = true;
            }
            if (data.sproing_sproing.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::sproing_sproing)] = true;
            }
            if (data.i_win.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::i_win)] = true;
            }
            if (data.where_the_sun_dont_shine.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::where_the_sun_dont_shine)] = true;
            }
            if (data.undead_space.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::undead_space)] = true;
            }
            if (data.disco_is_undead.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::disco_is_undead)] = true;
            }
            if (data.smashing.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::smashing)] = true;
            }
            if (data.gotcha.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::gotcha)] = true;
            }
            if (data.still_standing.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::still_standing)] = true;
            }
            if (data.the_stuff_of_legends.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::the_stuff_of_legends)] = true;
            }
            if (data.hammer_time.earned) {
                value.earned_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::hammer_time)] = true;
            }
            //shown
            if (data.home_lawn_security.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::home_security)] = true;
            }
            if (data.nobel_peas_prize.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::nobel_peas_prize)] = true;
            }
            if (data.better_off_dead.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::better_off_dead)] = true;
            }
            if (data.china_shop.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::china_shop)] = true;
            }
            if (data.spudow.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::spudow)] = true;
            }
            if (data.explodonator.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::explodonator)] = true;
            }
            if (data.morticulturalist.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::morticulturalist)] = true;
            }
            if (data.dont_pea_in_the_pool.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::dont_pea)] = true;
            }
            if (data.roll_some_heads.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::roll_some_heads)] = true;
            }
            if (data.grounded.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::grounded)] = true;
            }
            if (data.zombologist.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::zombologist)] = true;
            }
            if (data.penny_pincher.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::penny_pincher)] = true;
            }
            if (data.sunny_days.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::sunny_days)] = true;
            }
            if (data.popcorn_party.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::popcorn_party)] = true;
            }
            if (data.good_morning.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::good_morning)] = true;
            }
            if (data.no_fungus_among_us.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::no_fungus_amongus)] = true;
            }
            if (data.beyond_the_grave.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::beyond_the_grave)] = true;
            }
            if (data.immortal.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::immortal)] = true;
            }
            if (data.towering_wisdom.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::towering_wisdom)] = true;
            }
            if (data.mustache_mode.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::mustache_mode)] = true;
            }
            if (data.last_mown_stading.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::last_mown_stading)] = true;
            }
            if (data.below_zero.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::below_zero)] = true;
            }
            if (data.flower_power.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::flower_power)] = true;
            }
            if (data.pyromaniac.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::pyromaniac)] = true;
            }
            if (data.lawn_mowner_man.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::lawn_mowner_man)] = true;
            }
            if (data.chill_out.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::chill_out)] = true;
            }
            if (data.defcorn_five.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::defcorn_five)] = true;
            }
            if (data.monster_mash.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::monster_mash)] = true;
            }
            if (data.blind_faith.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::blind_faith)] = true;
            }
            if (data.pool_closed.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::pool_closed)] = true;
            }
            if (data.melony_lane.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::melony_lane)] = true;
            }
            if (data.second_life.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::second_life)] = true;
            }
            if (data.lucky_spin.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::lucky_spin)] = true;
            }
            if (data.chilli_free.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::chilli_free)] = true;
            }
            if (data.enlightened.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::enlightened)] = true;
            }
            if (data.diamond_beghouler.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::diamond_beghouler)] = true;
            }
            if (data.greenfingers.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::greenfingers)] = true;
            }
            if (data.wallnot_attack.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::wallnot_attack)] = true;
            }
            if (data.peking_express.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::peking_express)] = true;
            }
            if (data.shooting_star.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::shooting_star)] = true;
            }
            if (data.sol_invictus.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::sol_invictus)] = true;
            }
            if (data.sproing_sproing.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::sproing_sproing)] = true;
            }
            if (data.i_win.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::i_win)] = true;
            }
            if (data.where_the_sun_dont_shine.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::where_the_sun_dont_shine)] = true;
            }
            if (data.undead_space.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::undead_space)] = true;
            }
            if (data.disco_is_undead.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::disco_is_undead)] = true;
            }
            if (data.smashing.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::smashing)] = true;
            }
            if (data.gotcha.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::gotcha)] = true;
            }
            if (data.still_standing.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::still_standing)] = true;
            }
            if (data.the_stuff_of_legends.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::the_stuff_of_legends)] = true;
            }
            if (data.hammer_time.shown) {
                value.shown_achievements[exchange_enumeration<AchievementId, uint8_t>(AchievementId::hammer_time)] = true;
            }
            return;
        }

        inline static auto exchange_zombatar(
            Zombatar const &data,
            DetailStructure &value) -> void
        {
            value.accepted_zombatar_license = data.accepted_zombatar_license;
            value.zombatars.resize(data.value.size());
            for (auto i : Range(data.value.size()))
            {
                auto &zombatar_data = data.value[i];
                auto &zombatar = value.zombatars[i];
                zombatar.skin_color = zombatar_data.skin_color;
                zombatar.clothes_type = zombatar_data.clothes.type;
                zombatar.clothes_color = zombatar_data.clothes.color;
                zombatar.tidbits_type = zombatar_data.tidbits.type;
                zombatar.tidbits_color = zombatar_data.tidbits.color;
                zombatar.accessories_type = zombatar_data.accessories.type;
                zombatar.accessories_color = zombatar_data.accessories.color;
                zombatar.facial_hair_type = zombatar_data.facial_hair.type;
                zombatar.facial_hair_color = zombatar_data.facial_hair.color;
                zombatar.hair_type = zombatar_data.hair.type;
                zombatar.hair_color = zombatar_data.hair.color;
                zombatar.eyewear_type = zombatar_data.eyewear.type;
                zombatar.eyewear_color = zombatar_data.eyewear.color;
                zombatar.hat_type = zombatar_data.hat.type;
                zombatar.hat_color = zombatar_data.hat.color;
                zombatar.backdrop_type = zombatar_data.backdrop.type;
                zombatar.backdrop_color = zombatar_data.backdrop.color;
            }
            value.display_save_image_messenge = data.display_save_image_messenge;
            return;
        }

        inline static auto exchange_detail_info(
            DetailInfo const &data,
            DetailStructure &value) -> void
        {
            exchange_detail_general(data.general, value);
            exchange_purchased(data.store, data.zen_garden, value.purchases);
            if (data.has_trophies || data.has_unlocked)
            {
                exchange_detail_unlocked(data.unlocked, value);
                exchange_challenge(data.trophies, value.challenge_records);
            }
            if (data.has_zen_garden)
            {
                exchange_zen_garden(data.zen_garden, value);
                if (k_version < 30) {
                    exchange_height_of_tree(data.zen_garden.tree_of_wisdom, value.challenge_records);
                }
                value.has_zen_garden = true;
            }
            if (data.has_achievement)
            {
                exchange_achievement(data.achievement, value);
                value.has_achievement = true;
            }
            if (data.has_zombatar)
            {
                exchange_zombatar(data.zombatar, value);
                value.has_zombatar = true;
            }
            if (!data.unknown_expand.empty())
            {
                value.expand = exchange_data(data.unknown_expand);
            }
            return;
        }

        inline static auto exchange_player_info(
            DataStreamView &stream,
            DetailInfo const &definition) -> void
        {
            auto detail_structure = DetailStructure{};
            exchange_detail_info(definition, detail_structure);
            exchange_from_version(stream, k_version);
            exchange_detail_structure(stream, detail_structure);
            return;
        }

        inline static auto exchange_summary_info(
            DataStreamView &stream,
            SummaryInfo const &value
        ) -> void
        {
            stream.writeUint32(value.version);
            exchange_list<true, uint16_t>(stream, value.users, [] (auto &data, auto &value) {
                data.writeStringByUint16(value.name);
                data.writeInt32(value.use_seq);
                data.writeInt32(value.id);
            });
            return;
        }

        inline static auto read_version(
            std::string_view source
        ) -> void
        {
            VersionInfo definition = *FileSystem::read_json(source);
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            std::string_view source) -> void
        {
            read_version(source);
            if (k_version == 14)
            {
                SummaryInfo definition = *FileSystem::read_json(source);
                exchange_summary_info(stream, definition);
            }
            else
            {
                DetailInfo definition = *FileSystem::read_json(source);
                exchange_player_info(stream, definition);
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            process_whole(stream, source);
            stream.out_file(destination);
            return;
        }
    };
}