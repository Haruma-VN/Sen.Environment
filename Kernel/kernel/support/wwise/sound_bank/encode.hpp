#pragma once

#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank {

	#pragma region Category

	inline static auto constexpr IntegerCategory = true;

	#pragma endregion
	
	#pragma region encode

	inline static auto constexpr IsCustomConstraints = true;

	struct Encode {

		private:

			inline auto convert_hex_string(
				const std::string& hexString
			) -> std::vector<uint8_t>
			{
				// debug("convert_hex_string");
				auto hexStr = hexString;
				hexStr.erase(remove(hexStr.begin(), hexStr.end(), ' '), hexStr.end());
				auto bytes = std::vector<uint8_t>{};
				for (auto i = static_cast<unsigned int>(0); i < hexStr.length(); i += 2) {
					auto byteString = hexStr.substr(i, 2);
					auto byte = static_cast<std::uint8_t>(std::strtol(byteString.c_str(), NULL, 16));
					bytes.push_back(byte);
				}
				return bytes;
			}

		protected:

			template <typename T>
				requires std::is_integral<T>::value and IntegerCategory
			inline constexpr auto beautify_length(
				T size
			) -> T
			{
				// debug("beautify_length");
				if (size % 16 == 0) {
					return 0;
				}
				else
				{
					return 16 - (size % 16);
				}
			}

			inline auto insert_type(
				std::size_t lengthOffset
			) -> void
			{
				// debug("insert_type");
				auto current_position = sen.write_pos;
				sen.writeUint32(static_cast<std::uint32_t>(sen.write_pos - lengthOffset - 4), static_cast<std::uint64_t>(lengthOffset));
				sen.write_pos = current_position;
				return;
			}

			template <typename T> requires std::is_same<T, BKHD>::value or 
				std::is_same<T, std::vector<unsigned int>>::value or std::is_same<T, std::vector<INIT>>::value
				or std::is_same<T, STMG>::value or std::is_same<T, ENVS>::value or std::is_same<T, std::vector<HIRC>>::value
				or std::is_same<T, STID>::value or std::is_same<T, PLAT>::value
			inline auto encode_type(
				const SoundBankInformation & info,
				std::string_view source
			) -> void
			{
				// debug("encode_type");
				if constexpr (std::is_same<T, BKHD>::value and IsCustomConstraints) {
					encode_bkhd(info.bank_header);
				}
				if constexpr (std::is_same<T, std::vector<unsigned int>>::value and IsCustomConstraints) {
					encode_didx(info.embedded_media, source);
				}
				if constexpr (std::is_same<T, std::vector<INIT>>::value and IsCustomConstraints) {
					encode_init(info.initialization);
				}
				if constexpr (std::is_same<T, STMG>::value and IsCustomConstraints) {
					encode_stmg(info.game_synchronization, info.bank_header.version);
				}
				if constexpr (std::is_same<T, ENVS>::value and IsCustomConstraints) {
					encode_envs(info.environments, info.bank_header.version);
				}
				if constexpr (std::is_same<T, std::vector<HIRC>>::value and IsCustomConstraints) {
					encode_hirc(info.hierarchy);
				}
				if constexpr (std::is_same<T, STID>::value and IsCustomConstraints) {
					encode_stid(info.reference);
				}
				if constexpr (std::is_same<T, PLAT>::value and IsCustomConstraints) {
					encode_plat(info.platform_setting);
				}
				return;
			}

			inline auto encode_bkhd(
				const BKHD & BKHDInfo
			) -> void
			{
				// debug("encode_bkhd");
				auto head_expand = convert_hex_string(BKHDInfo.head_expand);
				sen.writeStringView("BKHD"_sv);
				sen.writeUint32(BKHDInfo.version, 8);
				sen.writeUint32(BKHDInfo.id);
				sen.writeUint32(BKHDInfo.language);
				sen.writeBytes(head_expand);
				insert_type(4);
				return;
			}

			inline auto encode_didx(
				const std::vector<unsigned int> & DIDXInfo, 
				std::string_view source
			) -> void
			{
				// debug("encode_didx");
				auto DATABank = DataStreamView{};
				sen.writeStringView("DIDX"_sv);
				auto DIDXLengthOffset = sen.write_pos;
				sen.writeNull(4);
				auto DIDXLength = DIDXInfo.size();
				for (auto i : Range<std::uint64_t>(DIDXLength))
				{
					auto wem = DataStreamView{ Path::Script::join(std::vector<std::string>{source.data(), fmt::format("{}.wem", DIDXInfo[i])})};
					sen.writeUint32(DIDXInfo[i]);
					sen.writeUint32(static_cast<std::uint32_t>(DATABank.write_pos));
					sen.writeUint32(static_cast<std::uint32_t>(wem.size()));
					DATABank.writeBytes(wem.getBytes(0, wem.size()));
					if (i < DIDXLength - 1) {
						DATABank.writeNull(beautify_length<std::uint64_t>((wem.size())));
					}
					wem.close();
				}
				insert_type(DIDXLengthOffset);
				// debug("data");
				sen.writeStringView("DATA"_sv);
				sen.writeUint32(static_cast<std::uint32_t>(DATABank.size()));
				debug("databank");
				debug(DATABank.size());
				// debug(sen.size());
				sen.writeBytes(DATABank.getBytes(0, DATABank.size()));
				DATABank.close();
				// debug("databank finish");
				return;
			}

			inline auto encode_init(
				const std::vector<INIT> &INITInfo
			) -> void
			{
				// debug("encode_init");
				sen.writeStringView("INIT"_sv);
				auto INITLengthOffset = sen.write_pos;
				sen.writeNull(4);
				auto INITLength = INITInfo.size();
				sen.writeUint32(static_cast<std::uint32_t>(INITLength));
				for (auto i : Range<std::uint32_t>(INITLength))
				{
					sen.writeUint32(INITInfo[i].id);
					sen.writeStringByEmpty(INITInfo[i].name);
				}
				insert_type(INITLengthOffset);
				return;
			}

			inline auto encode_stmg(
				const STMG & STMGInfo, 
				unsigned int version
			) -> void
			{
				// debug("encode_stmg");
				sen.writeStringView("STMG"_sv);
				auto STMGLengthOffset = sen.write_pos;
				sen.writeNull(4);
				auto volumeThresHold = convert_hex_string(STMGInfo.volume_threshold);
				if (volumeThresHold.size() != 4)
				{
					throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.encode.invalid_volume_threshold")), std::source_location::current(), 
						"encode_stmg");
				}
				auto maxVoiceInstances = convert_hex_string(STMGInfo.max_voice_instances);
				if (maxVoiceInstances.size() != 2)
				{
					throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.encode.invalid_max_voice_instances")), std::source_location::current(),
						"encode_stmg");
				}
				sen.writeBytes(volumeThresHold);
				sen.writeBytes(maxVoiceInstances);
				if (version >= 140)
				{
					sen.writeUint16(static_cast<std::uint16_t>(STMGInfo.unknown_type_1));
				}
				auto stageGroupLength = STMGInfo.stage_group.size();
				sen.writeUint32(static_cast<std::uint32_t>(stageGroupLength));
				for (auto i : Range<std::uint64_t>(stageGroupLength))
				{
					sen.writeUint32(STMGInfo.stage_group[i].id);
					auto defaultTransitionTime = convert_hex_string(STMGInfo.stage_group[i].data.default_transition_time);
					if (defaultTransitionTime.size() != 4)
					{
						throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.encode.invalid_default_transition_time")), std::source_location::current(),
							"encode_stmg");
					}
					sen.writeBytes(defaultTransitionTime);
					auto customTransitonLength = STMGInfo.stage_group[i].data.custom_transition.size();
					sen.writeUint32(static_cast<std::uint32_t>(customTransitonLength));
					for (auto k : Range<std::uint64_t>(customTransitonLength))
					{
						sen.writeBytes(convert_hex_string(STMGInfo.stage_group[i].data.custom_transition[k]));
					}
				}
				auto switchGroupLength = STMGInfo.switch_group.size();
				sen.writeUint32(static_cast<std::uint32_t>(switchGroupLength));
				for (auto i : Range<std::uint64_t>(switchGroupLength))
				{
					sen.writeUint32(STMGInfo.switch_group[i].id);
					sen.writeUint32(STMGInfo.switch_group[i].data.parameter);
					if (version == 112 || version == 140) {
						sen.writeUint8(static_cast<std::uint8_t>(STMGInfo.switch_group[i].data.parameter_category));
					}
					auto pointLength = STMGInfo.switch_group[i].data.point.size();
					sen.writeUint32(static_cast<std::uint32_t>(pointLength));
					for (auto k : Range<std::uint64_t>(pointLength))
					{
						sen.writeBytes(convert_hex_string(STMGInfo.switch_group[i].data.point[k]));
					}
				}
				auto gameParameterLength = STMGInfo.game_parameter.size();
				sen.writeUint32(static_cast<std::uint32_t>(gameParameterLength));
				for (auto i : Range<std::uint64_t>(gameParameterLength))
				{
					sen.writeUint32(STMGInfo.game_parameter[i].id);
					auto parameterData = convert_hex_string(STMGInfo.game_parameter[i].data);
					if (version == 112 && parameterData.size() != 17 || version >= 140 && parameterData.size() != 17)
					{
						throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.encode.invalid_parameter_data")), 
							std::source_location::current(),
							"encode_stmg");
					}
					if (version == 88 && parameterData.size() != 4)
					{
						throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.encode.invalid_parameter_data")), std::source_location::current(),
							"encode_stmg");
					}
					sen.writeBytes(parameterData);
				}
				if (version >= 140) {
					sen.writeUint32(STMGInfo.unknown_type_2);
				}
				insert_type(STMGLengthOffset);
				return;
			}

			inline auto encode_envs(
				const ENVS & ENVSInfo, 
				unsigned int version
			) -> void
			{
				// debug("encode_envs");
				sen.writeStringView("ENVS"_sv);
				auto ENVSLengthOffset = sen.write_pos;
				sen.writeNull(4);
				encode_envs_item(ENVSInfo.obstruction, version);
				encode_envs_item(ENVSInfo.occlusion, version);
				insert_type(ENVSLengthOffset);
				return;
			}

			inline auto encode_envs_item(
				const ENVSItem & ENVSItemInfo, 
				unsigned int version
			) -> void
			{
				// debug("encode_envs_item");
				sen.writeBytes(convert_hex_string(ENVSItemInfo.volume.volume_value));
				auto volumePointLength = ENVSItemInfo.volume.volume_point.size();
				sen.writeUint16(static_cast<std::uint16_t>(volumePointLength));
				for (auto i : Range<std::uint64_t>(volumePointLength))
				{
					sen.writeBytes(convert_hex_string(ENVSItemInfo.volume.volume_point[i]));
				}
				sen.writeBytes(convert_hex_string(ENVSItemInfo.low_pass_filter.low_pass_filter_value));
				auto lowPassFilterNumber = ENVSItemInfo.low_pass_filter.low_pass_filter_point.size();
				sen.writeUint16(static_cast<std::uint16_t>(lowPassFilterNumber));
				for (auto i : Range<std::uint64_t>(lowPassFilterNumber))
				{
					sen.writeBytes(convert_hex_string(ENVSItemInfo.low_pass_filter.low_pass_filter_point[i]));
				}
				if (version >= 112)
				{
					sen.writeBytes(convert_hex_string(ENVSItemInfo.high_pass_filter.high_pass_filter_value));
					auto highPassFilterNumber = ENVSItemInfo.high_pass_filter.high_pass_filter_point.size();
					sen.writeUint16(static_cast<std::uint16_t>(highPassFilterNumber));
					for (auto i : Range<std::uint64_t>(highPassFilterNumber))
					{
						sen.writeBytes(convert_hex_string(ENVSItemInfo.high_pass_filter.high_pass_filter_point[i]));
					}
				}
				return;
			}

			inline auto encode_hirc(
				const std::vector<HIRC>& HIRCInfo
			) -> void
			{
				// debug("encode_hirc");
				sen.writeStringView("HIRC"_sv);
				auto HIRCLengthOffset = sen.write_pos;
				sen.writeNull(4);
				auto HIRCLength = HIRCInfo.size();
				sen.writeUint32(static_cast<std::uint32_t>(HIRCLength));
				for (auto i : Range<std::uint64_t>(HIRCLength))
				{
					auto data = convert_hex_string(HIRCInfo[i].data);
					sen.writeUint8(static_cast<std::uint8_t>(HIRCInfo[i].type));
					sen.writeUint32(static_cast<std::uint32_t>(data.size() + 4));
					sen.writeUint32(static_cast<std::uint32_t>(HIRCInfo[i].id));
					sen.writeBytes(data);
				}
				insert_type(HIRCLengthOffset);
				return;
			}

			inline auto encode_stid(
				const STID &STIDInfo
			) -> void
			{
				// debug("encode_stid");
				sen.writeStringView("STID"_sv);
				auto STIDLengthOffset = sen.write_pos;
				sen.writeNull(4);
				sen.writeUint32(STIDInfo.unknown_type);
				auto STIDDataLength = STIDInfo.data.size();
				sen.writeUint32(static_cast<std::uint32_t>(STIDDataLength));
				for (auto i : Range<std::uint64_t>(STIDDataLength))
				{
					sen.writeUint32(STIDInfo.data[i].id);
					sen.writeStringByUint8(STIDInfo.data[i].name);
				}
				insert_type(STIDLengthOffset);
				return;
			}

			inline auto encode_plat(
				const PLAT &PLATInfo
			) -> void
			{
				// debug("encode_plat");
				sen.writeStringView("PLAT"_sv);
				auto PLATLengthOffset = sen.write_pos;
				sen.writeNull(4);
				sen.writeStringByEmpty(PLATInfo.platform);
				insert_type(PLATLengthOffset);
				return;
			}

			public:

				DataStreamView sen;

				explicit Encode(

				) : sen()
				{

				}

				~Encode(

				) = default;

				inline auto process(
					const SoundBankInformation & info, 
					std::string_view source
				) -> void
				{
					encode_type<BKHD>(info, source);
					if (info.has_initialization) {
						encode_type<std::vector<INIT>>(info, source);
					}
					if (info.has_game_synchronization) {
						encode_type<STMG>(info, source);
					}
					if (info.has_embedded_media) {
						encode_type<std::vector<unsigned int>>(info, source);
					}
					if (info.has_hierarchy) {
						encode_type<std::vector<HIRC>>(info, source);
					}
					if (info.has_environments) {
						encode_type<ENVS>(info, source);
					}
					if (info.has_reference) {
						encode_type<STID>(info, source);
					}
					if (info.has_platform_setting) {
						encode_type<PLAT>(info, source);
					}
					return;
				}

				inline static auto process_fs(
					std::string_view source,
					std::string_view destination
				) -> void
				{
					auto encoder = Encode{};
					encoder.process(
						FileSystem::read_json(Path::Script::join(source, "definition.json"_sv)),
						Path::Script::join(source, "embedded_audio"_sv)
					);
					debug(fmt::format("size: {}", encoder.sen.size()));
					encoder.sen.out_file(destination);
					return;
				}

	};

	#pragma endregion

}