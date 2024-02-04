#pragma once

#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank {
	
	struct Decode {

        private:

            inline auto create_hex_string(
                const std::vector<uint8_t>& buffer
            ) -> std::string
            {
                auto oss = std::ostringstream{};
                for (const auto& byte : buffer){
                    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << ' ';
                }
                auto hexString = oss.str();
                std::transform(hexString.cbegin(), hexString.cend(), hexString.begin(), [&](auto c) {
                    return std::toupper(c);
                });
                return hexString.substr(0, hexString.length() - 1);
            }

        protected:

            DataStreamView sen;

            inline auto decode_init(
                SoundBankInformation &info
            ) -> void
            {
                sen.readUint32();
                auto length = sen.readUint32();
                auto init_list = std::vector<INIT>{};
                init_list.reserve(length);
                for (auto i : Range<std::uint32_t>(length))
                {
                    init_list.emplace_back(INIT
                    {
                        .id = sen.readUint32(),
                        .name = sen.readStringByEmpty(),
                    });
                }
                info.initialization = std::move(init_list);
                info.has_initialization = true;
                return;
            }

            inline auto decode_environment(
                SoundBankInformation &info
            ) -> void
            {
                sen.readUint32();
                info.environments = ENVS
                {
                    .obstruction = decode_environment_item(info),
                    .occlusion = decode_environment_item(info),
                };
                info.has_environments = true;
                return;
            }

            inline auto decode_environment_item(
                SoundBankInformation &info
            ) -> ENVSItem
            {
                auto volumeValue = create_hex_string(sen.readBytes(2));
                auto volumeNumber = sen.readUint16();
                auto volumePoint = std::vector<std::string>{};
                volumePoint.reserve(volumeNumber);
                for (auto i : Range<uint16_t>(volumeNumber))
                {
                    volumePoint.emplace_back(create_hex_string(sen.readBytes(12)));
                }
                auto lowPassFilterValue = create_hex_string(sen.readBytes(2));
                auto lowPassFilterNumber = sen.readUint16();
                auto lowPassFilterPoint = std::vector<std::string>{};
                lowPassFilterPoint.reserve(lowPassFilterNumber);
                for (auto i : Range<uint16_t>(lowPassFilterNumber))
                {
                    lowPassFilterPoint.emplace_back(create_hex_string(sen.readBytes(12)));
                }
                if (info.bank_header.version >= 112)
                {
                    auto highPassFilterValue = create_hex_string(sen.readBytes(2));
                    auto highPassFilterNumber = sen.readUint16();
                    auto highPassFilterPoint = std::vector<std::string>{};
                    highPassFilterPoint.reserve(highPassFilterNumber);
                    for (auto i : Range<uint16_t>(highPassFilterNumber))
                    {
                        highPassFilterPoint.emplace_back(create_hex_string(sen.readBytes(12)));
                    }
                    return ENVSItem
                    {
                        .volume = ENVSVolume
                        {
                            .volume_value = volumeValue,
                            .volume_point = std::move(volumePoint),
                        },
                        .low_pass_filter = ENVSLowPassFilter
                        {
                            .low_pass_filter_value = lowPassFilterValue,
                            .low_pass_filter_point = std::move(lowPassFilterPoint),
                        },
                        .high_pass_filter = ENVSHighPassFilter
                        {
                            .high_pass_filter_value = highPassFilterValue,
                            .high_pass_filter_point = std::move(highPassFilterPoint),
                        },
                    };
                }
                else
                {
                    return ENVSItem
                    {
                        .volume = ENVSVolume
                        {
                            .volume_value = volumeValue,
                            .volume_point = std::move(volumePoint),
                        },
                        .low_pass_filter = ENVSLowPassFilter
                        {
                            .low_pass_filter_value = lowPassFilterValue,
                            .low_pass_filter_point = std::move(lowPassFilterPoint),
                        },
                    };
                }
            }

            inline auto decode_hirc(
                SoundBankInformation &info
            ) -> void
            {
                sen.readUint32();
                auto HIRCNumber = sen.readUint32();
                auto HIRCList = std::vector<HIRC>{};
                HIRCList.reserve(HIRCNumber);
                for (auto i : Range<uint32_t>(HIRCNumber))
                {
                    auto type = (uint32_t)sen.readUint8();
                    auto length = sen.readUint32();
                    auto id = sen.readUint32();
                    auto data = create_hex_string(sen.readBytes((static_cast<size_t>(length) - 4)));
                    HIRCList.emplace_back(HIRC
                    {
                        .id = id,
                        .type = type,
                        .data = data,
                    });
                }
                info.hierarchy = std::move(HIRCList);
                info.has_hierarchy = true;
                return;
            }

            inline auto decode_stid(
                SoundBankInformation& info
            ) -> void
            {
                sen.readUint32();
                auto unknown_type = sen.readUint32();
                auto STIDNumber = sen.readUint32();
                auto dataList = std::vector<STIDData>{};
                dataList.reserve(STIDNumber);
                for (auto i : Range<uint32_t>(STIDNumber))
                {
                    dataList.emplace_back(STIDData
                    {
                        .id = sen.readUint32(),
                        .name = sen.readStringByUint8(),
                    });
                }
                info.reference = STID
                {
                    .data = std::move(dataList),
                    .unknown_type = unknown_type,
                };
                info.has_reference = true;
                return;
            }

            inline auto decode_plat(
                SoundBankInformation& info
            ) -> void
            {
                sen.readUint32();
                info.platform_setting = PLAT
                {
                    .platform = sen.readStringByEmpty(),
                };
                info.has_platform_setting = true;
                return;
            }

            inline auto decode_didx(
                SoundBankInformation& info,
                std::string_view destination
            ) -> void
            {
                auto DIDXLength = sen.readUint32() + sen.read_pos;
                auto DIDX = std::vector<unsigned int>{};
                auto DATAList = std::vector<WEMDATATemp>{};
                for (auto i = 0; sen.read_pos < DIDXLength; i++)
                {
                    DIDX.emplace_back(sen.readUint32());
                    DATAList.emplace_back(WEMDATATemp
                        {
                            .offset = sen.readUint32(),
                            .length = sen.readUint32(),
                        });
                }
                if (sen.readString(4) != "DATA")
                {
                    throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_wem_data_bank")));
                }
                auto DATALength = sen.readUint32();
                auto WemDATAStartOffset = sen.read_pos;
                auto WEMBank = DataStreamView(sen.readBytes(static_cast<std::uint64_t>(DATALength)));
                for (auto i : Range<std::uint64_t>(DATAList.size()))
                {
                    auto wem = DataStreamView(WEMBank.readBytes(static_cast<std::uint64_t>(DATAList[i].length), static_cast<std::uint64_t>(DATAList[i].offset)));
                    wem.out_file(Path::Script::join(std::vector<std::string>{ destination.data(), "embedded_audio", fmt::format("{}.wem", DIDX[i]) }));
                }
                info.embedded_media = std::move(DIDX);
                info.has_embedded_media = true;
                return;
            }

            inline auto decode_stmg(
                SoundBankInformation& info
            ) -> void
            {
                sen.readUint32();
                auto volumeThresHold = create_hex_string(sen.readBytes(4));
                auto maxVoiceInstances = create_hex_string(sen.readBytes(2));
                auto unknown_type_1 = uint16_t{};
                if (info.bank_header.version >= 140)
                {
                    unknown_type_1 = sen.readUint16();
                }
                auto STMGStageNumber = sen.readUint32();
                auto stageGroupList = std::vector<STMGStageGroup>{};
                stageGroupList.reserve(STMGStageNumber);
                for (auto i : Range<std::uint32_t>(STMGStageNumber))
                {
                    auto id = sen.readUint32();
                    auto defaultTransitionTime = create_hex_string(sen.readBytes(4));
                    auto numberMS = sen.readUint32();
                    auto customTransiton = std::vector <std::string>{};
                    customTransiton.reserve(numberMS);
                    for (auto k : Range<std::uint32_t>(numberMS))
                    {
                        customTransiton.emplace_back(create_hex_string(sen.readBytes(12)));
                    }
                    stageGroupList[i] = STMGStageGroup
                    {
                        .id = id,
                        .data = STMGStageGroupData
                        {
                            .default_transition_time = defaultTransitionTime,
                            .custom_transition = std::move(customTransiton),
                        }
                    };
                }
                auto STMGSwitchNumber = sen.readUint32();
                auto switchGroupList = std::vector<STMGSwitchGroup>{};
                switchGroupList.reserve(STMGSwitchNumber);
                for (auto i : Range<std::uint32_t>(STMGSwitchNumber))
                {
                    auto id = sen.readUint32();
                    auto parameter = sen.readUint32();
                    auto parameterCategoty = std::uint8_t{};
                    if (info.bank_header.version >= 112)
                    {
                        parameterCategoty = sen.readUint8();
                    }
                    auto parameterNumber = sen.readUint32();
                    auto pointList = std::vector<std::string>{};
                    pointList.reserve(parameterNumber);
                    for (auto k : Range<std::uint32_t>(parameterNumber))
                    {
                        pointList.emplace_back(create_hex_string(sen.readBytes(12)));
                    }
                    switchGroupList.emplace_back(STMGSwitchGroup
                    {
                        .id = id,
                        .data = STMGSwitchGroupData
                        {
                            .parameter = parameter,
                            .parameter_category = parameterCategoty,
                            .point = pointList,
                        }
                    });
                }
                auto gameParameterNumber = sen.readUint32();
                auto gameParameterList = std::vector<STMGGameParameter>{};
                gameParameterList.reserve(gameParameterNumber);
                for (auto i : Range<std::uint32_t>(gameParameterNumber))
                {
                    if (info.bank_header.version >= 112)
                    {
                        gameParameterList.emplace_back(STMGGameParameter
                        {
                            .id = sen.readUint32(),
                            .data = create_hex_string(sen.readBytes(17)),
                        });
                    }
                    else
                    {
                        gameParameterList.emplace_back(STMGGameParameter
                        {
                            .id = sen.readUint32(),
                            .data = create_hex_string(sen.readBytes(4)),
                        });
                    }
                }
                auto unknown_type_2 = std::uint32_t{};
                if (info.bank_header.version >= 140) {
                    unknown_type_2 = sen.readUint32();
                }
                info.game_synchronization = STMG
                {
                    .volume_threshold = volumeThresHold,
                    .max_voice_instances = maxVoiceInstances,
                    .unknown_type_1 = static_cast<unsigned int>(unknown_type_1),
                    .stage_group = stageGroupList,
                    .switch_group = switchGroupList,
                    .game_parameter = gameParameterList,
                    .unknown_type_2 = static_cast<unsigned int>(unknown_type_2),
                };
                info.has_game_synchronization = true;
                return;
            }

            inline auto decode_type(
                SoundBankInformation& info,
                std::string_view destination
            ) -> void
            {
                auto type = sen.readString(4);
                switch (hash_sv(type))
                {
                    case hash_sv("DIDX"_sv):
                        decode_didx(info, destination);
                        return;
                    case hash_sv("INIT"_sv):
                        decode_init(info);
                        return;
                    case hash_sv("STMG"_sv):
                        decode_stmg(info);
                        return;
                    case hash_sv("ENVS"_sv):
                        decode_environment(info);
                        return;
                    case hash_sv("HIRC"_sv):
                        decode_hirc(info);
                        return;
                    case hash_sv("STID"_sv):
                        decode_stid(info);
                        return;
                    case hash_sv("PLAT"_sv):
                        decode_plat(info);
                        return;
                    case hash_sv("FXPR"_sv): {
                        throw Exception(fmt::format("{}", Kernel::Language::get("unsupported_fxpr")));
                    }
                    default: {
                        throw Exception(fmt::format("{} | Offset: {}", Kernel::Language::get("wwise.decode.invalid_bnk"), sen.get_write_pos()));
                    }
                }
            }

            public:

                explicit Decode(
                    std::string_view source
                ) : sen(source)
                {

                }

                ~Decode(

                ) = default;

                inline auto process(
                    std::string_view destination
                ) -> SoundBankInformation
                {
                    auto BKHD_magic = sen.readString(4);
                    if (BKHD_magic != "BKHD")
                    {
                        throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_bnk_magic")));
                    }
                    auto BKHD_length = sen.readUint32();
                    auto version = sen.readUint32();
                    auto id = sen.readUint32();
                    auto dataLength = BKHD_length - 12;
                    auto language = sen.readUint32();
                    auto head_expand = create_hex_string(sen.readBytes(static_cast<std::uint64_t>(dataLength)));
                    auto info = SoundBankInformation
                    {
                        .bank_header = BKHD
                        {
                            .version = version,
                            .id = id,
                            .language = language,
                            .head_expand = head_expand,
                        }
                    };
                    auto senLength = sen.size();
                    while (sen.read_pos < senLength) {
                        decode_type(info, destination);
                    }
                    if (sen.read_pos != senLength)
                    {
                        throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_reader")));
                    }
                    return info;
                }

                inline static auto process_fs(
                    std::string_view source,
                    std::string_view destination
                ) -> void
                {
                    FileSystem::write_json(Path::Script::join(
                        std::vector<std::string>{destination.data(), "definition.json"}),
                        Decode(source).process(destination)
                    );
                    return;
                }

	};

}