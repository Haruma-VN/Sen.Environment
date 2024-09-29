#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::PlayerInfo
{
    using namespace Definition;

    struct Common
    {
        inline constexpr static auto k_user_version_list = std::array<int, 4>{10, 11, 12, 13};

        inline constexpr static auto k_profile_version = 14;

        inline constexpr static auto k_amount_ratio = 1000;

        inline constexpr static auto k_coins_ratio = 10;

        struct ZombatarStructure
        {
            int skin_color;
            int clothes_type;
            int clothes_color;
            int tidbits_type;
            int tidbits_color;
            int accessories_type;
            int accessories_color;
            int facial_hair_type;
            int facial_hair_color;
            int hair_type;
            int hair_color;
            int eyewear_type;
            int eyewear_color;
            int hat_type;
            int hat_color;
            int backdrop_type;
            int backdrop_color;
        };

        inline static auto exchange_to_zombatar_struct(
            DataStreamView &data,
            ZombatarStructure &value) -> void
        {
            try_assert(data.readUint32() == 0xFFFFFFFF, "invalid_section");
            value.skin_color = data.readInt32();
            value.clothes_type = data.readInt32();
            value.clothes_color = data.readInt32();
            value.tidbits_type = data.readInt32();
            value.tidbits_color = data.readInt32();
            value.accessories_type = data.readInt32();
            value.accessories_color = data.readInt32();
            value.facial_hair_type = data.readInt32();
            value.facial_hair_color = data.readInt32();
            value.hair_type = data.readInt32();
            value.hair_color = data.readInt32();
            value.eyewear_type = data.readInt32();
            value.eyewear_color = data.readInt32();
            value.hat_type = data.readInt32();
            value.hat_color = data.readInt32();
            value.backdrop_type = data.readInt32();
            value.backdrop_color = data.readInt32();
            return;
        }

        inline static auto exchange_from_zombatar_struct(
            DataStreamView &data,
            ZombatarStructure const &value) -> void
        {
            data.writeUint32(0xFFFFFFFF);
            data.writeInt32(value.skin_color);
            data.writeInt32(value.clothes_type);
            data.writeInt32(value.clothes_color);
            data.writeInt32(value.tidbits_type);
            data.writeInt32(value.tidbits_color);
            data.writeInt32(value.accessories_type);
            data.writeInt32(value.accessories_color);
            data.writeInt32(value.facial_hair_type);
            data.writeInt32(value.facial_hair_color);
            data.writeInt32(value.hair_type);
            data.writeInt32(value.hair_color);
            data.writeInt32(value.eyewear_type);
            data.writeInt32(value.eyewear_color);
            data.writeInt32(value.hat_type);
            data.writeInt32(value.hat_color);
            data.writeInt32(value.backdrop_type);
            data.writeInt32(value.backdrop_color);
            return;
        }

        struct PottedPlantStructure
        {
            int seed_type;
            int which_zen_garden;
            int m_x;
            int m_y;
            int facing;
            int64_t last_watered_time;
            int draw_variation;
            int plant_age;
            int times_fed;
            int feedings_pergrow;
            int plant_need;
            int64_t last_need_fulfilled_time;
            int64_t last_fertilized_time;
            int64_t last_chocolate_time;
            int64_t future_attribute;
        };

        inline static auto exchange_to_potted_plants(
            DataStreamView &data,
            PottedPlantStructure &value) -> void
        {
            if (k_version < 30)
            {
                value.seed_type = data.readInt32();
                value.which_zen_garden = data.readInt32();
                value.m_x = data.readInt32();
                value.m_y = data.readInt32();
                value.facing = data.readInt32();
                data.read_pos += 4_size;
                value.last_watered_time = data.readInt64();
                value.draw_variation = data.readInt32();
                value.plant_age = data.readUint32();
                value.times_fed = data.readInt32();
                value.feedings_pergrow = data.readInt32();
                value.plant_need = data.readInt32();
                data.read_pos += 4_size;
                value.last_need_fulfilled_time = data.readInt64();
                value.last_fertilized_time = data.readInt64();
                value.last_chocolate_time = data.readInt64();
            }
            else
            {
                value.seed_type = data.readInt32();
                value.which_zen_garden = data.readInt32();
                value.m_x = data.readInt32();
                value.m_y = data.readInt32();
                value.facing = data.readInt32();
                value.last_watered_time = data.readInt32();
                value.draw_variation = data.readInt32();
                value.plant_age = data.readUint32();
                value.times_fed = data.readInt32();
                value.feedings_pergrow = data.readInt32();
                value.plant_need = data.readInt32();
                data.read_pos += 4_size;
                value.last_need_fulfilled_time = data.readInt32();
                value.last_fertilized_time = data.readInt32();
                value.last_chocolate_time = data.readInt32();
                value.future_attribute = data.readInt32();
                data.read_pos += 4_size;
            }
            return;
        }

        inline static auto exchange_from_potted_plants(
            DataStreamView &data,
            PottedPlantStructure const &value) -> void
        {
            if (k_version < 30)
            {
                data.writeInt32(value.seed_type);
                data.writeInt32(value.which_zen_garden);
                data.writeInt32(value.m_x);
                data.writeInt32(value.m_y);
                data.writeInt32(value.facing);
                data.writeNull(4_size);
                data.writeInt64(value.last_watered_time);
                data.writeInt32(value.draw_variation);
                data.writeInt32(value.plant_age);
                data.writeInt32(value.times_fed);
                data.writeInt32(value.feedings_pergrow);
                data.writeInt32(value.plant_need);
                data.writeNull(4_size);
                data.writeInt64(value.last_need_fulfilled_time);
                data.writeInt64(value.last_fertilized_time);
                data.writeInt64(value.last_chocolate_time);
                data.writeInt64(value.future_attribute);
            }
            else
            {
                data.writeInt32(value.seed_type);
                data.writeInt32(value.which_zen_garden);
                data.writeInt32(value.m_x);
                data.writeInt32(value.m_y);
                data.writeInt32(value.facing);
                data.writeInt32(value.last_watered_time);
                data.writeInt32(value.draw_variation);
                data.writeInt32(value.plant_age);
                data.writeInt32(value.times_fed);
                data.writeInt32(value.feedings_pergrow);
                data.writeInt32(value.plant_need);
                data.writeNull(4_size);
                data.writeInt32(value.last_need_fulfilled_time);
                data.writeInt32(value.last_fertilized_time);
                data.writeInt32(value.last_chocolate_time);
                data.writeInt32(value.future_attribute);
                data.writeNull(4_size);
            }
            return;
        }

        struct DetailStructure
        {
            int level;
            int coins;
            int finished_adventure;
            std::vector<int> challenge_records;
            std::vector<int> purchases;
            int play_time_active_player;
            int play_time_inactive_player;
            int has_used_cheat_keys;
            int has_woken_stiky;
            int didnt_purchase_packet_upgrade;
            int last_stinky_chocolate_time;
            int stinky_pos_x;
            int stinky_pos_y;
            int has_unlocked_minigames;
            int has_unlocked_puzzle_mode;
            int has_new_minigame;
            int has_new_scary_potter;
            int has_new_i_zombie;
            int has_new_survival;
            int has_new_limbo_page;
            int need_message_on_game_seletor;
            int need_magic_taco_reward;
            int has_seen_stinky;
            int has_seen_upsell;
            int place_holder_player_stats;
            int num_potted_plants;
            std::vector<PottedPlantStructure> potted_plants;
            std::vector<bool> earned_achievements;
            std::vector<bool> shown_achievements;
            bool accepted_zombatar_license;
            std::vector<ZombatarStructure> zombatars;
            bool display_save_image_messenge;
            // aditional
            bool has_zen_garden;
            bool has_achievement;
            bool has_zombatar;
            std::vector<uint8_t> expand;
        };

        inline static auto exchange_to_version(
            DataStreamView &data,
            int &value) -> void
        {
            value = data.readInt32();
            return;
        }

        inline static auto exchange_from_version(
            DataStreamView &data,
            const int &value) -> void
        {
            data.writeInt32(value);
            return;
        }

        template <auto min, auto max, typename T>
            requires std::is_arithmetic<T>::value
        inline static auto exchange_range(
            T const &value) -> T
        {
            static_assert(sizeof(min) == sizeof(T) && sizeof(max) == sizeof(T), "invalid_size_range");
            return (value > min) ? (value > max) ? max : value : min;
        }

        inline static auto exchange_challenge_records_count(

            ) -> size_t
        {
            if (k_version < 11)
            {
                return 101_size;
            }
            else if (k_version < 13)
            {
                return 100_size;
            }
            else if (k_version < 30)
            {
                return 200_size;
            }
            else
            {
                return 200_size;
            }
        }

        inline static auto exchange_purchases_count(

            ) -> size_t
        {
            if (k_version < 11)
            {
                return 80_size;
            }
            else if (k_version < 13)
            {
                return 80_size;
            }
            else if (k_version < 30)
            {
                return 102_size;
            }
            else
            {
                return 80_size;
            }
        }

        // Type is uint8_t || enum
        template <typename InputType, typename ReturnType>
            requires((std::is_enum<InputType>::value || std::is_same<InputType, uint8_t>::value)) && (std::is_enum<ReturnType>::value || std::is_same<ReturnType, uint8_t>::value || std::is_same<ReturnType, size_t>::value) && true
        inline static auto exchange_enumeration(
            InputType const &data) -> ReturnType
        {
            // static_assert(sizeof...(InputType) == 1 || sizeof...(InputType) == 0, "Expected 0 or 1 argument only");
            static_assert(std::is_enum<InputType>::value || std::is_enum<ReturnType>::value, "Must be one enumeration type");
            if constexpr (std::is_same<InputType, GameMode>::value || std::is_same<ReturnType, GameMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version < 11)
                    {
                        return 70_size;
                    }
                    else if (k_version < 13)
                    {
                        return 72_size;
                    }
                    else if (k_version < 30)
                    {
                        return 72_size;
                    }
                    else
                    {
                        return 200_size;
                    }
                }
                else
                {
                    if (k_version < 11)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, GameMode>, 70>{
                            std::pair(0, GameMode::adventure),
                            std::pair(1, GameMode::survival_normal_stage_1),
                            std::pair(2, GameMode::survival_normal_stage_2),
                            std::pair(3, GameMode::survival_normal_stage_3),
                            std::pair(4, GameMode::survival_normal_stage_4),
                            std::pair(5, GameMode::survival_normal_stage_5),
                            std::pair(6, GameMode::survival_hard_stage_1),
                            std::pair(7, GameMode::survival_hard_stage_2),
                            std::pair(8, GameMode::survival_hard_stage_3),
                            std::pair(9, GameMode::survival_hard_stage_4),
                            std::pair(10, GameMode::survival_hard_stage_5),
                            std::pair(11, GameMode::survival_endless_stage_1),
                            std::pair(12, GameMode::survival_endless_stage_2),
                            std::pair(13, GameMode::survival_endless_stage_3),
                            std::pair(14, GameMode::survival_endless_stage_4),
                            std::pair(15, GameMode::survival_endless_stage_5),
                            std::pair(16, GameMode::challenge_war_and_peas),
                            std::pair(17, GameMode::challenge_beghouled),
                            std::pair(18, GameMode::challenge_slot_machine),
                            std::pair(19, GameMode::challenge_raining_seeds),
                            std::pair(20, GameMode::challenge_wallnut_bowling),
                            std::pair(21, GameMode::challenge_invisighoul),
                            std::pair(22, GameMode::challenge_seeing_stars),
                            std::pair(23, GameMode::challenge_zombiquarium),
                            std::pair(24, GameMode::challenge_beghouled_twist),
                            std::pair(25, GameMode::challenge_little_trouble),
                            std::pair(26, GameMode::challenge_portal_combat),
                            std::pair(27, GameMode::challenge_column),
                            std::pair(28, GameMode::challenge_bobsled_bonanza),
                            std::pair(29, GameMode::challenge_speed),
                            std::pair(30, GameMode::challenge_whack_a_zombie),
                            std::pair(31, GameMode::challenge_pogo_party),
                            std::pair(32, GameMode::challenge_war_and_peas_2),
                            std::pair(33, GameMode::challenge_wallnut_bowling_2),
                            std::pair(34, GameMode::challenge_last_stand),
                            std::pair(35, GameMode::challenge_final_boss),
                            std::pair(36, GameMode::challenge_art_challenge_wallnut),
                            std::pair(37, GameMode::challenge_sunny_day),
                            std::pair(38, GameMode::challenge_resodded),
                            std::pair(39, GameMode::challenge_big_time),
                            std::pair(40, GameMode::challenge_art_challenge_sunflower),
                            std::pair(41, GameMode::challenge_air_raid),
                            std::pair(42, GameMode::challenge_ice),
                            std::pair(43, GameMode::challenge_zen_garden),
                            std::pair(44, GameMode::challenge_high_gravity),
                            std::pair(45, GameMode::challenge_grave_danger),
                            std::pair(46, GameMode::challenge_shovel),
                            std::pair(47, GameMode::challenge_stormy_night),
                            std::pair(48, GameMode::challenge_bungee_blitz),
                            std::pair(49, GameMode::challenge_squirrel),
                            std::pair(50, GameMode::scary_potter_1),
                            std::pair(51, GameMode::scary_potter_2),
                            std::pair(52, GameMode::scary_potter_3),
                            std::pair(53, GameMode::scary_potter_4),
                            std::pair(54, GameMode::scary_potter_5),
                            std::pair(55, GameMode::scary_potter_6),
                            std::pair(56, GameMode::scary_potter_7),
                            std::pair(57, GameMode::scary_potter_8),
                            std::pair(58, GameMode::scary_potter_9),
                            std::pair(59, GameMode::scary_potter_endless),
                            std::pair(60, GameMode::puzzle_i_zombie_1),
                            std::pair(61, GameMode::puzzle_i_zombie_2),
                            std::pair(62, GameMode::puzzle_i_zombie_3),
                            std::pair(63, GameMode::puzzle_i_zombie_4),
                            std::pair(64, GameMode::puzzle_i_zombie_5),
                            std::pair(65, GameMode::puzzle_i_zombie_6),
                            std::pair(66, GameMode::puzzle_i_zombie_7),
                            std::pair(67, GameMode::puzzle_i_zombie_8),
                            std::pair(68, GameMode::puzzle_i_zombie_9),
                            std::pair(69, GameMode::puzzle_i_zombie_endless)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 13)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, GameMode>, 72>{
                            std::pair(0, GameMode::survival_normal_stage_1),
                            std::pair(1, GameMode::survival_normal_stage_2),
                            std::pair(2, GameMode::survival_normal_stage_3),
                            std::pair(3, GameMode::survival_normal_stage_4),
                            std::pair(4, GameMode::survival_normal_stage_5),
                            std::pair(5, GameMode::survival_hard_stage_1),
                            std::pair(6, GameMode::survival_hard_stage_2),
                            std::pair(7, GameMode::survival_hard_stage_3),
                            std::pair(8, GameMode::survival_hard_stage_4),
                            std::pair(9, GameMode::survival_hard_stage_5),
                            std::pair(10, GameMode::survival_endless_stage_1),
                            std::pair(11, GameMode::survival_endless_stage_2),
                            std::pair(12, GameMode::survival_endless_stage_3),
                            std::pair(13, GameMode::survival_endless_stage_4),
                            std::pair(14, GameMode::survival_endless_stage_5),
                            std::pair(15, GameMode::challenge_war_and_peas),
                            std::pair(16, GameMode::challenge_wallnut_bowling),
                            std::pair(17, GameMode::challenge_slot_machine),
                            std::pair(18, GameMode::challenge_raining_seeds),
                            std::pair(19, GameMode::challenge_beghouled),
                            std::pair(20, GameMode::challenge_invisighoul),
                            std::pair(21, GameMode::challenge_seeing_stars),
                            std::pair(22, GameMode::challenge_zombiquarium),
                            std::pair(23, GameMode::challenge_beghouled_twist),
                            std::pair(24, GameMode::challenge_little_trouble),
                            std::pair(25, GameMode::challenge_portal_combat),
                            std::pair(26, GameMode::challenge_column),
                            std::pair(27, GameMode::challenge_bobsled_bonanza),
                            std::pair(28, GameMode::challenge_speed),
                            std::pair(29, GameMode::challenge_whack_a_zombie),
                            std::pair(30, GameMode::challenge_last_stand),
                            std::pair(31, GameMode::challenge_war_and_peas_2),
                            std::pair(32, GameMode::challenge_wallnut_bowling_2),
                            std::pair(33, GameMode::challenge_pogo_party),
                            std::pair(34, GameMode::challenge_final_boss),
                            std::pair(35, GameMode::challenge_art_challenge_wallnut),
                            std::pair(36, GameMode::challenge_sunny_day),
                            std::pair(37, GameMode::challenge_resodded),
                            std::pair(38, GameMode::challenge_big_time),
                            std::pair(39, GameMode::challenge_art_challenge_sunflower),
                            std::pair(40, GameMode::challenge_air_raid),
                            std::pair(41, GameMode::challenge_ice),
                            std::pair(42, GameMode::challenge_zen_garden),
                            std::pair(43, GameMode::challenge_high_gravity),
                            std::pair(44, GameMode::challenge_grave_danger),
                            std::pair(45, GameMode::challenge_shovel),
                            std::pair(46, GameMode::challenge_stormy_night),
                            std::pair(47, GameMode::challenge_bungee_blitz),
                            std::pair(48, GameMode::challenge_squirrel),
                            std::pair(49, GameMode::tree_of_wisdom),
                            std::pair(50, GameMode::scary_potter_1),
                            std::pair(51, GameMode::scary_potter_2),
                            std::pair(52, GameMode::scary_potter_3),
                            std::pair(53, GameMode::scary_potter_4),
                            std::pair(54, GameMode::scary_potter_5),
                            std::pair(55, GameMode::scary_potter_6),
                            std::pair(56, GameMode::scary_potter_7),
                            std::pair(57, GameMode::scary_potter_8),
                            std::pair(58, GameMode::scary_potter_9),
                            std::pair(59, GameMode::scary_potter_endless),
                            std::pair(60, GameMode::puzzle_i_zombie_1),
                            std::pair(61, GameMode::puzzle_i_zombie_2),
                            std::pair(62, GameMode::puzzle_i_zombie_3),
                            std::pair(63, GameMode::puzzle_i_zombie_4),
                            std::pair(64, GameMode::puzzle_i_zombie_5),
                            std::pair(65, GameMode::puzzle_i_zombie_6),
                            std::pair(66, GameMode::puzzle_i_zombie_7),
                            std::pair(67, GameMode::puzzle_i_zombie_8),
                            std::pair(68, GameMode::puzzle_i_zombie_9),
                            std::pair(69, GameMode::puzzle_i_zombie_endless),
                            std::pair(70, GameMode::upsell),
                            std::pair(71, GameMode::intro)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 30)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, GameMode>, 72>{
                            std::pair(0, GameMode::survival_normal_stage_1),
                            std::pair(1, GameMode::survival_normal_stage_2),
                            std::pair(2, GameMode::survival_normal_stage_3),
                            std::pair(3, GameMode::survival_normal_stage_4),
                            std::pair(4, GameMode::survival_normal_stage_5),
                            std::pair(5, GameMode::survival_hard_stage_1),
                            std::pair(6, GameMode::survival_hard_stage_2),
                            std::pair(7, GameMode::survival_hard_stage_3),
                            std::pair(8, GameMode::survival_hard_stage_4),
                            std::pair(9, GameMode::survival_hard_stage_5),
                            std::pair(10, GameMode::survival_endless_stage_1),
                            std::pair(11, GameMode::survival_endless_stage_2),
                            std::pair(12, GameMode::survival_endless_stage_3),
                            std::pair(13, GameMode::survival_endless_stage_4),
                            std::pair(14, GameMode::survival_endless_stage_5),
                            std::pair(15, GameMode::challenge_war_and_peas),
                            std::pair(16, GameMode::challenge_wallnut_bowling),
                            std::pair(17, GameMode::challenge_slot_machine),
                            std::pair(18, GameMode::challenge_raining_seeds),
                            std::pair(19, GameMode::challenge_beghouled),
                            std::pair(20, GameMode::challenge_invisighoul),
                            std::pair(21, GameMode::challenge_seeing_stars),
                            std::pair(22, GameMode::challenge_zombiquarium),
                            std::pair(23, GameMode::challenge_beghouled_twist),
                            std::pair(24, GameMode::challenge_little_trouble),
                            std::pair(25, GameMode::challenge_portal_combat),
                            std::pair(26, GameMode::challenge_column),
                            std::pair(27, GameMode::challenge_bobsled_bonanza),
                            std::pair(28, GameMode::challenge_speed),
                            std::pair(29, GameMode::challenge_whack_a_zombie),
                            std::pair(30, GameMode::challenge_last_stand),
                            std::pair(31, GameMode::challenge_war_and_peas_2),
                            std::pair(32, GameMode::challenge_wallnut_bowling_2),
                            std::pair(33, GameMode::challenge_pogo_party),
                            std::pair(34, GameMode::challenge_final_boss),
                            std::pair(35, GameMode::challenge_art_challenge_wallnut),
                            std::pair(36, GameMode::challenge_sunny_day),
                            std::pair(37, GameMode::challenge_resodded),
                            std::pair(38, GameMode::challenge_big_time),
                            std::pair(39, GameMode::challenge_art_challenge_sunflower),
                            std::pair(40, GameMode::challenge_air_raid),
                            std::pair(41, GameMode::challenge_ice),
                            std::pair(42, GameMode::challenge_zen_garden),
                            std::pair(43, GameMode::challenge_high_gravity),
                            std::pair(44, GameMode::challenge_grave_danger),
                            std::pair(45, GameMode::challenge_shovel),
                            std::pair(46, GameMode::challenge_stormy_night),
                            std::pair(47, GameMode::challenge_bungee_blitz),
                            std::pair(48, GameMode::challenge_squirrel),
                            std::pair(49, GameMode::tree_of_wisdom),
                            std::pair(50, GameMode::scary_potter_1),
                            std::pair(51, GameMode::scary_potter_2),
                            std::pair(52, GameMode::scary_potter_3),
                            std::pair(53, GameMode::scary_potter_4),
                            std::pair(54, GameMode::scary_potter_5),
                            std::pair(55, GameMode::scary_potter_6),
                            std::pair(56, GameMode::scary_potter_7),
                            std::pair(57, GameMode::scary_potter_8),
                            std::pair(58, GameMode::scary_potter_9),
                            std::pair(59, GameMode::scary_potter_endless),
                            std::pair(60, GameMode::puzzle_i_zombie_1),
                            std::pair(61, GameMode::puzzle_i_zombie_2),
                            std::pair(62, GameMode::puzzle_i_zombie_3),
                            std::pair(63, GameMode::puzzle_i_zombie_4),
                            std::pair(64, GameMode::puzzle_i_zombie_5),
                            std::pair(65, GameMode::puzzle_i_zombie_6),
                            std::pair(66, GameMode::puzzle_i_zombie_7),
                            std::pair(67, GameMode::puzzle_i_zombie_8),
                            std::pair(68, GameMode::puzzle_i_zombie_9),
                            std::pair(69, GameMode::puzzle_i_zombie_endless),
                            std::pair(70, GameMode::upsell),
                            std::pair(71, GameMode::intro)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, GameMode>, 81>{
                            std::pair(0, GameMode::survival_normal_stage_1),
                            std::pair(1, GameMode::survival_normal_stage_2),
                            std::pair(2, GameMode::survival_normal_stage_3),
                            std::pair(3, GameMode::survival_normal_stage_4),
                            std::pair(4, GameMode::survival_normal_stage_5),
                            std::pair(5, GameMode::survival_hard_stage_1),
                            std::pair(6, GameMode::survival_hard_stage_2),
                            std::pair(7, GameMode::survival_hard_stage_3),
                            std::pair(8, GameMode::survival_hard_stage_4),
                            std::pair(9, GameMode::survival_hard_stage_5),
                            std::pair(10, GameMode::survival_endless_stage_1),
                            std::pair(11, GameMode::survival_endless_stage_2),
                            std::pair(12, GameMode::survival_endless_stage_3),
                            std::pair(13, GameMode::survival_endless_stage_4),
                            std::pair(14, GameMode::survival_endless_stage_5),
                            std::pair(15, GameMode::challenge_war_and_peas),
                            std::pair(16, GameMode::challenge_wallnut_bowling),
                            std::pair(17, GameMode::challenge_slot_machine),
                            std::pair(18, GameMode::challenge_raining_seeds),
                            std::pair(19, GameMode::challenge_beghouled),
                            std::pair(20, GameMode::challenge_invisighoul),
                            std::pair(21, GameMode::challenge_seeing_stars),
                            std::pair(22, GameMode::challenge_zombiquarium),
                            std::pair(23, GameMode::challenge_beghouled_twist),
                            std::pair(24, GameMode::challenge_little_trouble),
                            std::pair(25, GameMode::challenge_portal_combat),
                            std::pair(26, GameMode::challenge_column),
                            std::pair(27, GameMode::challenge_bobsled_bonanza),
                            std::pair(28, GameMode::challenge_speed),
                            std::pair(29, GameMode::challenge_whack_a_zombie),
                            std::pair(30, GameMode::challenge_last_stand),
                            std::pair(31, GameMode::challenge_war_and_peas_2),
                            std::pair(32, GameMode::challenge_wallnut_bowling_2),
                            std::pair(33, GameMode::challenge_pogo_party),
                            std::pair(34, GameMode::challenge_final_boss),
                            std::pair(35, GameMode::challenge_art_challenge_wallnut),
                            std::pair(36, GameMode::challenge_sunny_day),
                            std::pair(37, GameMode::challenge_resodded),
                            std::pair(38, GameMode::challenge_big_time),
                            std::pair(39, GameMode::challenge_art_challenge_sunflower),
                            std::pair(40, GameMode::challenge_air_raid),
                            std::pair(41, GameMode::challenge_ice),
                            std::pair(42, GameMode::challenge_zen_garden),
                            std::pair(43, GameMode::challenge_high_gravity),
                            std::pair(44, GameMode::challenge_grave_danger),
                            std::pair(45, GameMode::challenge_shovel),
                            std::pair(46, GameMode::challenge_stormy_night),
                            std::pair(47, GameMode::challenge_bungee_blitz),
                            std::pair(48, GameMode::challenge_squirrel),
                            std::pair(49, GameMode::scary_potter_1),
                            std::pair(50, GameMode::scary_potter_2),
                            std::pair(51, GameMode::scary_potter_3),
                            std::pair(52, GameMode::scary_potter_4),
                            std::pair(53, GameMode::scary_potter_5),
                            std::pair(54, GameMode::scary_potter_6),
                            std::pair(55, GameMode::scary_potter_7),
                            std::pair(56, GameMode::scary_potter_8),
                            std::pair(57, GameMode::scary_potter_9),
                            std::pair(58, GameMode::scary_potter_endless),
                            std::pair(59, GameMode::puzzle_i_zombie_1),
                            std::pair(60, GameMode::puzzle_i_zombie_2),
                            std::pair(61, GameMode::puzzle_i_zombie_3),
                            std::pair(62, GameMode::puzzle_i_zombie_4),
                            std::pair(63, GameMode::puzzle_i_zombie_5),
                            std::pair(64, GameMode::puzzle_i_zombie_6),
                            std::pair(65, GameMode::puzzle_i_zombie_7),
                            std::pair(66, GameMode::puzzle_i_zombie_8),
                            std::pair(67, GameMode::puzzle_i_zombie_9),
                            std::pair(68, GameMode::puzzle_i_zombie_endless),
                            std::pair(121, GameMode::last_stand_stage_1),
                            std::pair(122, GameMode::last_stand_stage_2),
                            std::pair(123, GameMode::last_stand_stage_3),
                            std::pair(124, GameMode::last_stand_stage_4),
                            std::pair(125, GameMode::last_stand_stage_5),
                            std::pair(126, GameMode::last_stand_endless),
                            std::pair(139, GameMode::unknown),
                            std::pair(195, GameMode::unknown),
                            std::pair(196, GameMode::unknown),
                            std::pair(197, GameMode::unknown),
                            std::pair(198, GameMode::unknown),
                            std::pair(199, GameMode::unknown)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, StoreItem>::value || std::is_same<ReturnType, StoreItem>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version < 11)
                    {
                        return 27_size;
                    }
                    else if (k_version < 13)
                    {
                        return 31_size;
                    }
                    else if (k_version < 30)
                    { // Maybe
                        return 19_size;
                    }
                    else
                    {
                        return 44_size;
                    }
                }
                else
                {
                    if (k_version < 11)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, StoreItem>, 27>{
                            std::pair(0, StoreItem::plant_gatlingpea),
                            std::pair(1, StoreItem::plant_twinsunflower),
                            std::pair(2, StoreItem::plant_gloomshroom),
                            std::pair(3, StoreItem::plant_cattail),
                            std::pair(4, StoreItem::plant_wintermelon),
                            std::pair(5, StoreItem::plant_gold_magnet),
                            std::pair(6, StoreItem::plant_spikerock),
                            std::pair(7, StoreItem::plant_cobcannon),
                            std::pair(8, StoreItem::plant_imitater),
                            std::pair(9, StoreItem::bonus_lawn_mower),
                            std::pair(10, StoreItem::potted_marigold_1),
                            std::pair(11, StoreItem::potted_marigold_2),
                            std::pair(12, StoreItem::potted_marigold_3),
                            std::pair(13, StoreItem::gold_wateringcan),
                            std::pair(14, StoreItem::fertilizer),
                            std::pair(15, StoreItem::bug_spray),
                            std::pair(16, StoreItem::phonograph),
                            std::pair(17, StoreItem::gardening_glove),
                            std::pair(18, StoreItem::mushroom_garden),
                            std::pair(19, StoreItem::wheel_barrow),
                            std::pair(20, StoreItem::stinky_the_snail),
                            std::pair(21, StoreItem::packet_upgrade),
                            std::pair(22, StoreItem::pool_cleaner),
                            std::pair(23, StoreItem::roof_cleaner),
                            std::pair(24, StoreItem::rake),
                            std::pair(25, StoreItem::aquarium_garden),
                            std::pair(26, StoreItem::chocolate)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 13)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, StoreItem>, 31>{
                            std::pair(0, StoreItem::plant_gatlingpea),
                            std::pair(1, StoreItem::plant_twinsunflower),
                            std::pair(2, StoreItem::plant_gloomshroom),
                            std::pair(3, StoreItem::plant_cattail),
                            std::pair(4, StoreItem::plant_wintermelon),
                            std::pair(5, StoreItem::plant_gold_magnet),
                            std::pair(6, StoreItem::plant_spikerock),
                            std::pair(7, StoreItem::plant_cobcannon),
                            std::pair(8, StoreItem::plant_imitater),
                            std::pair(9, StoreItem::bonus_lawn_mower),
                            std::pair(10, StoreItem::potted_marigold_1),
                            std::pair(11, StoreItem::potted_marigold_2),
                            std::pair(12, StoreItem::potted_marigold_3),
                            std::pair(13, StoreItem::gold_wateringcan),
                            std::pair(14, StoreItem::fertilizer),
                            std::pair(15, StoreItem::bug_spray),
                            std::pair(16, StoreItem::phonograph),
                            std::pair(17, StoreItem::gardening_glove),
                            std::pair(18, StoreItem::mushroom_garden),
                            std::pair(19, StoreItem::wheel_barrow),
                            std::pair(20, StoreItem::stinky_the_snail),
                            std::pair(21, StoreItem::packet_upgrade),
                            std::pair(22, StoreItem::pool_cleaner),
                            std::pair(23, StoreItem::roof_cleaner),
                            std::pair(24, StoreItem::rake),
                            std::pair(25, StoreItem::aquarium_garden),
                            std::pair(26, StoreItem::chocolate),
                            std::pair(27, StoreItem::tree_of_wisdom),
                            std::pair(28, StoreItem::tree_food),
                            std::pair(29, StoreItem::firstaid),
                            std::pair(30, StoreItem::pvz)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 30)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, StoreItem>, 15>{
                            std::pair(0, StoreItem::plant_gatlingpea),
                            std::pair(1, StoreItem::plant_twinsunflower),
                            std::pair(2, StoreItem::plant_gloomshroom),
                            std::pair(3, StoreItem::plant_cattail),
                            std::pair(4, StoreItem::plant_wintermelon),
                            std::pair(5, StoreItem::plant_gold_magnet),
                            std::pair(6, StoreItem::plant_spikerock),
                            std::pair(7, StoreItem::plant_cobcannon),
                            std::pair(8, StoreItem::plant_imitater),
                            std::pair(13, StoreItem::packet_upgrade),
                            std::pair(14, StoreItem::pool_cleaner),
                            std::pair(15, StoreItem::roof_cleaner),
                            std::pair(16, StoreItem::rake),
                            std::pair(17, StoreItem::firstaid),
                            std::pair(18, StoreItem::pvz)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, StoreItem>, 44>{
                            std::pair(0, StoreItem::plant_gatlingpea),
                            std::pair(1, StoreItem::plant_twinsunflower),
                            std::pair(2, StoreItem::plant_gloomshroom),
                            std::pair(3, StoreItem::plant_cattail),
                            std::pair(4, StoreItem::plant_wintermelon),
                            std::pair(5, StoreItem::plant_gold_magnet),
                            std::pair(6, StoreItem::plant_spikerock),
                            std::pair(7, StoreItem::plant_cobcannon),
                            std::pair(8, StoreItem::plant_imitater),
                            std::pair(9, StoreItem::bonus_lawn_mower),
                            std::pair(10, StoreItem::potted_marigold_1),
                            std::pair(11, StoreItem::potted_marigold_2),
                            std::pair(12, StoreItem::potted_marigold_3),
                            std::pair(13, StoreItem::packet_upgrade),
                            std::pair(14, StoreItem::pool_cleaner),
                            std::pair(15, StoreItem::roof_cleaner),
                            std::pair(16, StoreItem::rake),
                            std::pair(17, StoreItem::firstaid),
                            std::pair(18, StoreItem::gold_wateringcan),
                            std::pair(19, StoreItem::fertilizer),
                            std::pair(20, StoreItem::bug_spray),
                            std::pair(21, StoreItem::phonograph),
                            std::pair(22, StoreItem::gardening_glove),
                            std::pair(23, StoreItem::mushroom_garden),
                            std::pair(24, StoreItem::wheel_barrow),
                            std::pair(25, StoreItem::stinky_the_snail),
                            std::pair(26, StoreItem::aquarium_garden),
                            std::pair(27, StoreItem::chocolate),
                            std::pair(28, StoreItem::basic_giftbox),
                            std::pair(29, StoreItem::zombotany),
                            std::pair(30, StoreItem::minigame_pack_1),
                            std::pair(31, StoreItem::minigame_pack_2),
                            std::pair(32, StoreItem::minigame_pack_3),
                            std::pair(33, StoreItem::minigame_pack_4),
                            std::pair(34, StoreItem::minigame_pack_5),
                            std::pair(35, StoreItem::i_zombie_pack),
                            std::pair(36, StoreItem::scary_potter_pack),
                            std::pair(37, StoreItem::bronze_giftbox),
                            std::pair(38, StoreItem::silver_giftbox),
                            std::pair(39, StoreItem::gold_giftbox),
                            std::pair(40, StoreItem::bacon),
                            std::pair(41, StoreItem::gold_giftbox),
                            std::pair(42, StoreItem::last_stand_pack),
                            std::pair(43, StoreItem::hammer)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }

            if constexpr (std::is_same<InputType, AchievementId>::value || std::is_same<ReturnType, AchievementId>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 12)
                    {
                        if (k_version < 13)
                        {
                            return 20_size;
                        }
                        else if (k_version < 30)
                        {
                            return 25_size;
                        }
                        else
                        {
                            return 46_size;
                        }
                    }
                }
                else
                {
                    if (k_version >= 12)
                    {
                        if (k_version < 13)
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, AchievementId>, 20>{
                                std::pair(0, AchievementId::home_security),
                                std::pair(1, AchievementId::nobel_peas_prize),
                                std::pair(2, AchievementId::better_off_dead),
                                std::pair(3, AchievementId::china_shop),
                                std::pair(4, AchievementId::spudow),
                                std::pair(5, AchievementId::explodonator),
                                std::pair(6, AchievementId::morticulturalist),
                                std::pair(7, AchievementId::dont_pea),
                                std::pair(8, AchievementId::roll_some_heads),
                                std::pair(9, AchievementId::grounded),
                                std::pair(10, AchievementId::zombologist),
                                std::pair(11, AchievementId::penny_pincher),
                                std::pair(12, AchievementId::sunny_days),
                                std::pair(13, AchievementId::popcorn_party),
                                std::pair(14, AchievementId::good_morning),
                                std::pair(15, AchievementId::no_fungus_amongus),
                                std::pair(16, AchievementId::beyond_the_grave),
                                std::pair(17, AchievementId::immortal),
                                std::pair(18, AchievementId::towering_wisdom),
                                std::pair(19, AchievementId::mustache_mode)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                        else if (k_version < 30)
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, AchievementId>, 25>{
                                std::pair(0, AchievementId::home_security),
                                std::pair(1, AchievementId::spudow),
                                std::pair(2, AchievementId::explodonator),
                                std::pair(3, AchievementId::morticulturalist),
                                std::pair(4, AchievementId::dont_pea),
                                std::pair(5, AchievementId::roll_some_heads),
                                std::pair(6, AchievementId::grounded),
                                std::pair(7, AchievementId::zombologist),
                                std::pair(8, AchievementId::penny_pincher),
                                std::pair(9, AchievementId::sunny_days),
                                std::pair(10, AchievementId::popcorn_party),
                                std::pair(11, AchievementId::good_morning),
                                std::pair(12, AchievementId::no_fungus_amongus),
                                std::pair(13, AchievementId::last_mown_stading),
                                std::pair(14, AchievementId::below_zero),
                                std::pair(15, AchievementId::flower_power),
                                std::pair(16, AchievementId::pyromaniac),
                                std::pair(17, AchievementId::lawn_mowner_man),
                                std::pair(18, AchievementId::chill_out),
                                std::pair(19, AchievementId::defcorn_five),
                                std::pair(20, AchievementId::monster_mash),
                                std::pair(21, AchievementId::blind_faith),
                                std::pair(22, AchievementId::pool_closed),
                                std::pair(23, AchievementId::melony_lane),
                                std::pair(24, AchievementId::second_life)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                        else
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, AchievementId>, 46>{
                                std::pair(0, AchievementId::home_security),
                                std::pair(1, AchievementId::spudow),
                                std::pair(2, AchievementId::explodonator),
                                std::pair(3, AchievementId::morticulturalist),
                                std::pair(4, AchievementId::dont_pea),
                                std::pair(5, AchievementId::roll_some_heads),
                                std::pair(6, AchievementId::grounded),
                                std::pair(7, AchievementId::zombologist),
                                std::pair(8, AchievementId::penny_pincher),
                                std::pair(9, AchievementId::sunny_days),
                                std::pair(10, AchievementId::good_morning),
                                std::pair(11, AchievementId::no_fungus_amongus),
                                std::pair(12, AchievementId::last_mown_stading),
                                std::pair(13, AchievementId::below_zero),
                                std::pair(14, AchievementId::flower_power),
                                std::pair(15, AchievementId::pyromaniac),
                                std::pair(16, AchievementId::lawn_mowner_man),
                                std::pair(17, AchievementId::chill_out),
                                std::pair(18, AchievementId::defcorn_five),
                                std::pair(19, AchievementId::monster_mash),
                                std::pair(20, AchievementId::blind_faith),
                                std::pair(21, AchievementId::pool_closed),
                                std::pair(22, AchievementId::melony_lane),
                                std::pair(23, AchievementId::second_life),
                                std::pair(24, AchievementId::lucky_spin),
                                std::pair(25, AchievementId::chilli_free),
                                std::pair(26, AchievementId::enlightened),
                                std::pair(27, AchievementId::diamond_beghouler),
                                std::pair(28, AchievementId::greenfingers),
                                std::pair(29, AchievementId::wallnot_attack),
                                std::pair(30, AchievementId::beyond_the_grave),
                                std::pair(31, AchievementId::peking_express),
                                std::pair(32, AchievementId::shooting_star),
                                std::pair(33, AchievementId::sol_invictus),
                                std::pair(34, AchievementId::sproing_sproing),
                                std::pair(35, AchievementId::better_off_dead),
                                std::pair(36, AchievementId::i_win),
                                std::pair(37, AchievementId::where_the_sun_dont_shine),
                                std::pair(38, AchievementId::undead_space),
                                std::pair(39, AchievementId::disco_is_undead),
                                std::pair(40, AchievementId::china_shop),
                                std::pair(41, AchievementId::smashing),
                                std::pair(42, AchievementId::gotcha),
                                std::pair(43, AchievementId::still_standing),
                                std::pair(44, AchievementId::the_stuff_of_legends),
                                std::pair(45, AchievementId::hammer_time)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                    }
                }
            }

            //------------------------------
            if constexpr (std::is_same<ReturnType, size_t>::value)
            {
                return 0_size;
            }
            if constexpr (std::is_same<InputType, uint8_t>::value)
            {
                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                auto type = std::string_view{std::to_string(data)};
                assert_conditional(false,  String::format(fmt::format("{}", Language::get("miscellaneous.shared.failed_cast_enum_index")), type, magic_enum::enum_type_name<ReturnType>()), "exchange_enumeration");
            }
        }
    };
}