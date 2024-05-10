#pragma once

#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{

#pragma region Category

	inline static auto constexpr IntegerCategory = true;

#pragma endregion

#pragma region encode

	inline static auto constexpr IsCustomConstraints = true;

	struct Encode
	{

	private:
		inline auto convert_hex_string(
			const std::string &hexString) -> std::vector<uint8_t>
		{
			// debug("convert_hex_string");
			auto hexStr = hexString;
			hexStr.erase(remove(hexStr.begin(), hexStr.end(), ' '), hexStr.end());
			auto bytes = std::vector<uint8_t>{};
			for (auto i = static_cast<unsigned int>(0); i < hexStr.length(); i += 2)
			{
				auto byteString = hexStr.substr(i, 2);
				auto byte = static_cast<std::uint8_t>(std::strtol(byteString.data(), NULL, 16));
				bytes.push_back(byte);
			}
			return bytes;
		}

		size_t pos;
		inline auto set_chuck_pos() -> void
		{
			pos = stream.write_pos;
			stream.writeNull(4);
			return;
		}

		inline auto write_chuck_size() -> void
		{
			const auto current = stream.write_pos;
			const auto chuck_size = current - pos - 4;
			stream.writeUint32(chuck_size, pos);
			stream.write_pos = current;
			return;
		}

	protected:
		DataStreamView stream;

		uint32_t version;

	protected:
		inline auto encode_bank_header(const BKHD &info) -> void
		{
			stream.writeString("BKHD"_sv);
			set_chuck_pos();
			stream.writeUint32(info.version);
			assert_conditional(info.version > 26, String::format(fmt::format("{}", Language::get("wwise.soundbank.encode.unsupported_bank_version")), std::to_string(info.version)), "encode_bank_header");
			version = info.version;
			stream.writeUint32(info.soundbank_id);
			stream.writeUint32(info.language_id);
			stream.writeUint32(info.head_info.values);
			if (version > 76)
			{
				stream.writeUint32(info.head_info.project_id);
			}
			if (version > 141)
			{
				stream.writeUint32(info.head_info.soundbank_type);
				stream.writeBytes(convert_hex_string(info.head_info.bank_hash));
			}
			stream.writeNull(info.head_info.padding);
			write_chuck_size();
			return;
		}

		template <typename T>
		auto constexpr wem_padding_size(
			const T& size
		) -> T
		{
			if (size % 16 == 0)
			{
				return 0;
			}
			else
			{
				return (16 - (size % 16));
			}
		}

		inline auto encode_embedded_media(
			const std::vector<uint32_t> &info,
			std::string_view source) -> void
		{
			auto data_bank = DataStreamView{};
			stream.writeString("DIDX"_sv);
			set_chuck_pos();
			const auto didx_count = info.size();
			for (const auto &i : Range(didx_count))
			{
				auto wem = FileSystem::read_binary<uint8_t>(fmt::format("{}/{}.wem", source, info[i]));
				stream.writeUint32(info[i]);
				stream.writeUint32(data_bank.write_pos);
				stream.writeUint32(wem.size());
				data_bank.writeBytes(wem);
				if (i < didx_count - 1)
				{
					data_bank.writeNull(wem_padding_size<std::size_t>(wem.size()));
				}
			}
			write_chuck_size();
			stream.writeString("DATA"_sv);
			stream.writeUint32(data_bank.size());
			stream.writeBytes(data_bank.toBytes());
			return;
		}

		inline auto encode_plugin_chuck(const std::vector<INIT> &info) -> void
		{
			stream.writeString("INIT"_sv);
			set_chuck_pos();
			const auto init_count = info.size();
			stream.writeUint32(init_count);
			for (const auto &i : Range(init_count))
			{
				stream.writeUint32(info[i].id);
				stream.writeStringByEmpty(info[i].name);
			}
			write_chuck_size();
			return;
		}

		inline auto encode_global_settings(
			const STMG &info) -> void
		{
			stream.writeString("STMG"_sv);
			set_chuck_pos();
			if (version > 140)
			{
				stream.writeUint16(info.filter_behavior);
			}
			stream.writeFloat(info.volume_threshold);
			if (version > 53)
			{
				stream.writeUint16(info.max_num_voices_limit_internal);
			}
			if (version > 126)
			{
				stream.writeUint16(info.max_num_dangerous_virt_voices_limit_internal);
			}
			const auto state_count = info.state_group.size();
			stream.writeUint32(state_count);
			for (const auto &state_group : info.state_group)
			{
				stream.writeUint32(state_group.id);
				stream.writeUint32(state_group.data.default_transition_time);
				if (version < 52)
				{
					throw Exception("");
				}
				stream.writeUint32(state_group.data.state_transition.size());
				for (const auto &state_transition : state_group.data.state_transition)
				{
					stream.writeUint32(state_transition.state_from);
					stream.writeUint32(state_transition.state_to);
					stream.writeUint32(state_transition.transition_time);
				}
			}
			stream.writeUint32(info.switch_group.size());
			for (const auto &switch_group : info.switch_group)
			{
				stream.writeUint32(switch_group.id);
				stream.writeUint32(switch_group.data.rtpc_id);
				if (version > 89)
				{
					stream.writeUint8(switch_group.data.rtpc_type);
				}
				stream.writeUint32(switch_group.data.graph_point.size());
				for (const auto &graph_point : switch_group.data.graph_point)
				{
					stream.writeFloat(graph_point.from);
					stream.writeFloat(graph_point.to);
					stream.writeUint32(graph_point.interp);
				}
			}
			if (version <= 38)
			{
				write_chuck_size();
				return;
			}
			stream.writeUint32(info.game_parameter.size());
			for (const auto &game_parameter : info.game_parameter)
			{
				stream.writeUint32(game_parameter.id);
				stream.writeFloat(game_parameter.data.value);
				if (version > 89)
				{
					stream.writeUint32(game_parameter.data.ramp_type);
					stream.writeFloat(game_parameter.data.ramp_up);
					stream.writeFloat(game_parameter.data.ramp_down);
					stream.writeUint8(game_parameter.data.built_in_param);
				}
			}
			if (version <= 118)
			{
				// Nothing to do
			}
			else if (version <= 122)
			{
				stream.writeUint32(info.acoustic_texture.size());
				for (const auto &acoustic_texture : info.acoustic_texture)
				{
					stream.writeUint32(acoustic_texture.id);
					stream.writeUint16(acoustic_texture.data.on_off_band[0]);
					stream.writeUint16(acoustic_texture.data.on_off_band[1]);
					stream.writeUint16(acoustic_texture.data.on_off_band[2]);
					stream.writeUint16(acoustic_texture.data.filter_type_band[0]);
					stream.writeUint16(acoustic_texture.data.filter_type_band[1]);
					stream.writeUint16(acoustic_texture.data.filter_type_band[2]);
					stream.writeUint16(acoustic_texture.data.frequecy_band[0]);
					stream.writeUint16(acoustic_texture.data.frequecy_band[1]);
					stream.writeUint16(acoustic_texture.data.frequecy_band[2]);
					stream.writeFloat(acoustic_texture.data.factor_band[0]);
					stream.writeFloat(acoustic_texture.data.factor_band[1]);
					stream.writeFloat(acoustic_texture.data.factor_band[2]);
					stream.writeFloat(acoustic_texture.data.gain_band[0]);
					stream.writeFloat(acoustic_texture.data.gain_band[1]);
					stream.writeFloat(acoustic_texture.data.gain_band[2]);
				}
			}
			else
			{
				stream.writeUint32(info.acoustic_texture.size());
				for (const auto &acoustic_texture : info.acoustic_texture)
				{
					stream.writeFloat(acoustic_texture.data.absorption_offset);
					stream.writeFloat(acoustic_texture.data.absorption_low);
					stream.writeFloat(acoustic_texture.data.absorption_mid_low);
					stream.writeFloat(acoustic_texture.data.absorption_mid_high);
					stream.writeFloat(acoustic_texture.data.absorption_high);
					stream.writeFloat(acoustic_texture.data.scattering);
				}
			}
			if (version <= 118)
			{
				// Nothing to do
			}
			else if (version <= 122)
			{
				stream.writeUint32(info.diffuse_reverberator.size());
				for (const auto &diffuse_reverberator : info.diffuse_reverberator)
				{
					stream.writeUint32(diffuse_reverberator.id);
					stream.writeFloat(diffuse_reverberator.data.time);
					stream.writeFloat(diffuse_reverberator.data.hf_ratio);
					stream.writeFloat(diffuse_reverberator.data.dry_level);
					stream.writeFloat(diffuse_reverberator.data.wet_level);
					stream.writeFloat(diffuse_reverberator.data.spread);
					stream.writeFloat(diffuse_reverberator.data.density);
					stream.writeFloat(diffuse_reverberator.data.quality);
					stream.writeFloat(diffuse_reverberator.data.diffusion);
					stream.writeFloat(diffuse_reverberator.data.scale);
				}
			}
			write_chuck_size();
			return;
		}

		inline auto encode_env_item(
			const ENVSItem &info) -> void
		{
			stream.writeBoolean(info.volume.curve_enabled);
			stream.writeUint8(info.volume.curve_scaling);
			stream.writeUint16(info.volume.graph_point.size());
			for (const auto &graph_point : info.volume.graph_point)
			{
				stream.writeFloat(graph_point.from);
				stream.writeFloat(graph_point.to);
				stream.writeUint32(graph_point.interp);
			}
			stream.writeBoolean(info.low_pass_filter.curve_enabled);
			stream.writeUint8(info.low_pass_filter.curve_scaling);
			stream.writeUint16(info.low_pass_filter.graph_point.size());
			for (const auto &graph_point : info.low_pass_filter.graph_point)
			{
				stream.writeFloat(graph_point.from);
				stream.writeFloat(graph_point.to);
				stream.writeUint32(graph_point.interp);
			}
			if (version > 89)
			{
				stream.writeBoolean(info.high_pass_filter.curve_enabled);
				stream.writeUint8(info.high_pass_filter.curve_scaling);
				stream.writeUint16(info.high_pass_filter.graph_point.size());
				for (const auto &graph_point : info.high_pass_filter.graph_point)
				{
					stream.writeFloat(graph_point.from);
					stream.writeFloat(graph_point.to);
					stream.writeUint32(graph_point.interp);
				}
			}
			return;
		}

		inline auto encode_env_settings(
			const ENVS &info) -> void
		{
			stream.writeString("ENVS"_sv);
			set_chuck_pos();
			encode_env_item(info.obstruction);
			encode_env_item(info.occlusion);
			write_chuck_size();
			return;
		}

		inline static auto hirc_type_126 = std::map<std::string, uint32_t>{
            std::pair("stateful_property_setting", 0x01),
            std::pair("sound", 0x02),
            std::pair("event_action", 0x03),
            std::pair("event", 0x04),
            std::pair("sound_playlist_container", 0x05),
            std::pair("sound_switch_container", 0x06),
            std::pair("actor_mixer", 0x07),
            std::pair("audio_bus", 0x08),
            std::pair("sound_blend_container", 0x09),
            std::pair("music_segment", 0x0a),
            std::pair("music_track", 0x0b),
            std::pair("music_switch_container", 0x0c),
            std::pair("music_playlist_container", 0x0d),
            std::pair("attenuation", 0x0e),
            std::pair("dialogue_event", 0x0f),
            std::pair("feedback_bus", 0x10),
            std::pair("feedback_node", 0x11),
            std::pair("effect", 0x12),
            std::pair("source", 0x13),
            std::pair("auxiliary_audio_bus", 0x14),
            std::pair("low_frequency_oscillator_modulator", 0x15),
            std::pair("envelope_modulator", 0x16),
            std::pair("audio_device", 0x17)};

        inline static auto hirc_type_128 = std::map<std::string, uint32_t>{
            std::pair("stateful_property_setting", 0x01),
            std::pair("sound", 0x02),
            std::pair("event_action", 0x03),
            std::pair("event", 0x04),
            std::pair("sound_playlist_container", 0x05),
            std::pair("sound_switch_container", 0x06),
            std::pair("actor_mixer", 0x07),
            std::pair("audio_bus", 0x08),
            std::pair("sound_blend_container", 0x09),
            std::pair("music_segment", 0x0a),
            std::pair("music_track", 0x0b),
            std::pair("music_switch_container", 0x0c),
            std::pair("music_playlist_container", 0x0d),
            std::pair("attenuation", 0x0e),
            std::pair("dialogue_event", 0x0f),
            std::pair("effect", 0x10),
            std::pair("source", 0x11),
            std::pair("auxiliary_audio_bus", 0x12),
            std::pair("low_frequency_oscillator_modulator", 0x13),
            std::pair("envelope_modulator", 0x14),
            std::pair("audio_device", 0x15),
            std::pair("time_modulator", 0x16)};

		inline auto get_hierarchy_type(const std::string & type_str) -> uint32_t
        {
            if (version <= 126)
            {
				assert_conditional(hirc_type_126.contains(type_str), String::format(fmt::format("{}", Language::get("wwise.soundbank.encode.invalid_type"), type_str)), "get_hierarchy_type");
            }
            else
            {
				assert_conditional(hirc_type_128.contains(type_str), String::format(fmt::format("{}", Language::get("wwise.soundbank.encode.invalid_type"), type_str)), "get_hierarchy_type");
                return hirc_type_128[type_str];
            }
        }

		inline auto encode_hierachy(
			const std::vector<HIRC> &info
		) -> void
		{
			stream.writeString("HIRC"_sv);
			const auto pos = stream.write_pos;
			stream.writeNull(4);
			stream.writeUint32(info.size());
			for (const auto &hirc : info) {
				const auto hirc_type = get_hierarchy_type(hirc.type);
				if (version <= 48) {
					stream.writeUint32(hirc_type);
				}
				else {
					stream.writeUint8(static_cast<uint8_t>(hirc_type));
				}
				set_chuck_pos();
				stream.writeUint32(hirc.id);
				if (hirc_type == 0x04) {
					if (version <= 122) {
						stream.writeUint32(hirc.child.size());
					}
					else {
						stream.writeVarInt32(hirc.child.size());
					}
					for (const auto & child : hirc.child) {
						stream.writeUint32(child);
					}
				}
				else {
					stream.writeBytes(convert_hex_string(hirc.data));
				}
				write_chuck_size();
			}
			const auto current = stream.write_pos;
			stream.writeUint32(current - pos - 4, pos);
			stream.write_pos = current;
			return;
		}

		inline auto encode_string_mapping(
			const STID & info
		) -> void {
			set_chuck_pos();
			stream.writeUint32(info.type);
			stream.writeUint32(info.data.size());
			for (const auto &stid_data : info.data) {
				stream.writeUint32(stid_data.id);
				stream.writeStringByUint8(stid_data.name);
			}
			write_chuck_size();
			return;
		}

		inline auto encode_platform_settings(
			const PLAT &info
		) -> void
		{
			stream.writeString("PLAT"_sv);
			set_chuck_pos();
			stream.writeStringByEmpty(info.platform);
			write_chuck_size();
			return;
		}

	public:
		explicit Encode(

			) : stream()
		{
		}

		~Encode(

			) = default;

		inline auto process(
			const SoundBankInformation &info,
			std::string_view source) -> void
		{
			encode_bank_header(info.bank_header);
			if (info.has_embedded_media)
			{
				encode_embedded_media(info.embedded_media, source);
			}
			if (info.has_initialization)
			{
				encode_plugin_chuck(info.initialization);
			}
			if (info.has_game_synchronization)
			{
				encode_global_settings(info.game_synchronization);
			}
			if (info.has_hierarchy) {
				encode_hierachy(info.hierarchy);
			}
			if (info.has_environments)
			{
				encode_env_settings(info.environments);
			}
			if (info.has_reference) {
				encode_string_mapping(info.reference);
			}
			if (info.has_platform_setting) {
				encode_platform_settings(info.platform_setting);
			}
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination) -> void
		{
			auto encoder = Encode{};

			encoder.process(
				*FileSystem::read_json(fmt::format("{}/definition.json", source)),
				fmt::format("{}/embedded_audio", source));
			encoder.stream.out_file(destination);
			return;
		}
	};

#pragma endregion

}