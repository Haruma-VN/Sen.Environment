#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/wwise/sound_bank/definition.hpp"
#include "kernel/support/wwise/sound_bank/common.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{
	using namespace Definition;

	using namespace Sen::Kernel::Support::Miscellaneous::Shared;

	struct Encode : Common
	{
	protected:
		inline static auto exchange_hex_string(
			std::string const & value
		) -> std::vector<uint8_t>
		{
			auto hexString = value;
			hexString.erase(remove(hexString.begin(), hexString.end(), ' '), hexString.end());
			auto bytes = std::vector<uint8_t>{};
			for (auto i = static_cast<unsigned int>(0); i < hexString.length(); i += 2)
			{
				auto byteString = hexString.substr(i, 2);
				auto byte = static_cast<std::uint8_t>(std::strtol(byteString.data(), NULL, 16));
				bytes.push_back(byte);
			}
			return bytes;
		}

		inline static auto chuck_sign_set(
			DataStreamView &stream,
			ChunkSign &value) -> void
		{
			stream.writeString(value.id);
			value.pos = stream.write_pos;
			stream.writeNull(4_size);
			return;
		}

		inline static auto chuck_sign_write(
			DataStreamView &stream,
			ChunkSign const &value) -> void
		{
			auto current_pos = stream.write_pos;
			auto chuck_size = current_pos - static_cast<size_t>(value.pos) - k_block_size;
			stream.writeUint32(static_cast<uint32_t>(chuck_size), static_cast<size_t>(value.pos));
			stream.write_pos = current_pos;
			return;
		}

		inline static auto exchange_bank_header(
			DataStreamView &stream,
			BKHD const &value) -> void
		{
			assert_conditional(value.version > 52_ui, String::format(fmt::format("{}", Language::get("wwise.soundbank.decode.unsupported_bank_version")), std::to_string(value.version)), "exchange_bank_header");
			stream.writeUint32(value.version);
			k_version = value.version;
			stream.writeUint32(value.soundbank_id);
			stream.writeUint32(value.language_id);
			stream.writeUint32(value.head_info.values);
			if (k_version > 76_ui)
			{
				stream.writeUint32(value.head_info.project_id);
			}
			if (k_version > 141_ui)
			{
				stream.writeUint32(value.head_info.soundbank_type);
				stream.writeBytes(exchange_hex_string(value.head_info.bank_hash));
			}
			stream.writeNull(value.head_info.padding);
			return;
		}

		inline static auto exchange_embedded_media(
			DataStreamView &stream,
			std::vector<uint32_t> const &value,
			ChunkSign &sign,
			std::string_view source) -> void
		{
			chuck_sign_set(stream, sign);
			auto data_stream = DataStreamView{};
			auto data_count = value.size();
			for (auto data_index : Range(data_count))
			{
				auto data = FileSystem::read_binary<uint8_t>(fmt::format("{}/embedded_audio/{}.wem", source, value[data_index]));
				stream.writeUint32(value[data_index]);
				stream.writeUint32(static_cast<uint32_t>(data_stream.write_pos));
				stream.writeUint32(static_cast<uint32_t>(data.size()));
				data_stream.writeBytes(data);
				if (data_index < data_count - 1_size)
				{
					data_stream.writeNull(compute_padding_size(data_stream.write_pos, k_data_block_padding_size));
				}
			}
			chuck_sign_write(stream, sign);
			auto data_sign = ChunkSign{
				.id = ChunkSignFlag::data,
				.size = static_cast<uint32_t>(data_stream.size())};
			exchange_chuck_sign(data_sign, stream);
			stream.writeBytes(data_stream.toBytes());
			return;
		}

		inline static auto exchange_plugin_chunk(
			DataStreamView &stream,
			std::vector<INIT> const &value) -> void
		{
			auto initialization_count = value.size();
			exchange_list<true, uint32_t>(stream, value, [](DataStreamView &stream, INIT const &element)
														 {
				stream.writeUint32(element.id);
				stream.writeStringByEmpty(element.name); 
			});
			return;
		}

		inline static auto exchange_game_synchronization(
			DataStreamView &stream,
			STMG const &value) -> void
		{
			if (k_version > 140_ui)
			{
				stream.writeUint16(value.filter_behavior);
			}
			stream.writeFloat(value.volume_threshold);
			if (k_version > 53_ui)
			{
				stream.writeUint16(value.max_num_voices_limit_internal);
			}
			if (k_version > 126_ui)
			{
				stream.writeUint16(value.max_num_dangerous_virt_voices_limit_internal);
			}
			stream.writeUint32(static_cast<uint32_t>(value.state_group.size()));
			for (auto &state : value.state_group)
			{
				stream.writeUint32(state.id);
				stream.writeUint32(state.data.default_transition_time);
				stream.writeUint32(static_cast<uint32_t>(state.data.state_transition.size()));
				for (auto &state_transition : state.data.state_transition)
				{
					stream.writeUint32(state_transition.state_from);
					stream.writeUint32(state_transition.state_to);
					stream.writeUint32(state_transition.transition_time);
				}
			}
			stream.writeUint32(static_cast<uint32_t>(value.switch_group.size()));
			for (auto &switch_layer : value.switch_group)
			{
				stream.writeUint32(switch_layer.id);
				stream.writeUint32(switch_layer.data.rtpc_id);
				if (k_version > 89_ui)
				{
					stream.writeUint8(switch_layer.data.rtpc_type);
				}
				exchange_list<true, uint32_t>(stream, switch_layer.data.graph_point, &exchange_from_graph_point);
			}
			if (k_version <= 38_ui)
				return;
			stream.writeUint32(static_cast<uint32_t>(value.game_parameter.size()));
			for (auto &game_parameter : value.game_parameter)
			{
				stream.writeUint32(game_parameter.id);
				stream.writeFloat(game_parameter.data.value);
				if (k_version > 89_ui)
				{
					stream.writeUint32(game_parameter.data.ramp_type);
					stream.writeFloat(game_parameter.data.ramp_up);
					stream.writeFloat(game_parameter.data.ramp_down);
					stream.writeUint8(game_parameter.data.built_in_param);
				}
			}
			if (k_version <= 118_ui)
				return;
			if (k_version <= 122_ui)
			{
				stream.writeUint32(static_cast<uint32_t>(value.acoustic_texture.size()));
				for (auto &texture : value.acoustic_texture)
				{
					stream.writeUint32(texture.id);
					stream.writeUint16(texture.data.on_off_band[0]);
					stream.writeUint16(texture.data.on_off_band[1]);
					stream.writeUint16(texture.data.on_off_band[2]);
					stream.writeUint16(texture.data.filter_type_band[0]);
					stream.writeUint16(texture.data.filter_type_band[1]);
					stream.writeUint16(texture.data.filter_type_band[2]);
					stream.writeUint16(texture.data.frequecy_band[0]);
					stream.writeUint16(texture.data.frequecy_band[1]);
					stream.writeUint16(texture.data.frequecy_band[2]);
					stream.writeFloat(texture.data.factor_band[0]);
					stream.writeFloat(texture.data.factor_band[1]);
					stream.writeFloat(texture.data.factor_band[2]);
					stream.writeFloat(texture.data.gain_band[0]);
					stream.writeFloat(texture.data.gain_band[1]);
					stream.writeFloat(texture.data.gain_band[2]);
				}
				stream.writeUint32(static_cast<uint32_t>(value.diffuse_reverberator.size()));
				for (auto &diffuse : value.diffuse_reverberator)
				{
					stream.writeUint32(diffuse.id);
					stream.writeFloat(diffuse.data.time);
					stream.writeFloat(diffuse.data.hf_ratio);
					stream.writeFloat(diffuse.data.dry_level);
					stream.writeFloat(diffuse.data.wet_level);
					stream.writeFloat(diffuse.data.spread);
					stream.writeFloat(diffuse.data.density);
					stream.writeFloat(diffuse.data.quality);
					stream.writeFloat(diffuse.data.diffusion);
					stream.writeFloat(diffuse.data.scale);
				}
			}
			else
			{
				stream.writeUint32(static_cast<uint32_t>(value.acoustic_texture.size()));
				for (auto &texture : value.acoustic_texture)
				{
					stream.writeFloat(texture.data.absorption_offset);
					stream.writeFloat(texture.data.absorption_low);
					stream.writeFloat(texture.data.absorption_mid_low);
					stream.writeFloat(texture.data.absorption_mid_high);
					stream.writeFloat(texture.data.absorption_high);
					stream.writeFloat(texture.data.scattering);
				}
			}
			return;
		}

		inline static auto exchange_hierarchy(
			DataStreamView &stream,
			std::vector<HIRC> const &value) -> void
		{
			stream.writeUint32(static_cast<uint32_t>(value.size()));
			for (auto &hierarchy : value)
			{
				auto type = exchange_hierarchy_type(hierarchy.type);
				stream.writeUint8(static_cast<uint8_t>(type));
				auto section_pos = stream.write_pos;
				stream.writeNull(4_size);
				stream.writeUint32(hierarchy.id);
				if (hierarchy.type == k_hierarchy_event_type_string)
				{
					k_version <= 122_ui ? stream.writeUint32(static_cast<uint32_t>(hierarchy.child.size())) : stream.writeVarInt32(static_cast<uint32_t>(hierarchy.child.size()));
					for (auto &child : hierarchy.child)
					{
						stream.writeUint32(child);
					}
				}
				else
				{
					stream.writeBytes(exchange_hex_string(hierarchy.data));
				}
				auto current_pos = stream.write_pos;
				auto section_length = current_pos - section_pos - k_block_size;
				stream.writeUint32(static_cast<uint32_t>(section_length), section_pos);
				stream.write_pos = current_pos;
			}
			return;
		}

		inline static auto exchange_environment_item(
			DataStreamView &stream,
			ENVSItem const &value) -> void
		{
			stream.writeBoolean(value.volume.curve_enabled);
			stream.writeUint8(value.volume.curve_scaling);
			exchange_list<true, uint16_t>(stream, value.volume.graph_point, &exchange_from_graph_point);
			stream.writeBoolean(value.low_pass_filter.curve_enabled);
			stream.writeUint8(value.low_pass_filter.curve_scaling);
			exchange_list<true, uint16_t>(stream, value.low_pass_filter.graph_point, &exchange_from_graph_point);
			if (k_version > 89_ui)
			{
				stream.writeBoolean(value.high_pass_filter.curve_enabled);
				stream.writeUint8(value.high_pass_filter.curve_scaling);
				exchange_list<true, uint16_t>(stream, value.high_pass_filter.graph_point, &exchange_from_graph_point);
			}
			return;
		}

		inline static auto exchange_environments_settings(
			DataStreamView &stream,
			ENVS const &value) -> void
		{
			exchange_environment_item(stream, value.obstruction);
			exchange_environment_item(stream, value.occlusion);
			return;
		}

		inline static auto exchange_string_mapping(
			DataStreamView &stream,
            STID const &value
		) -> void
		{
			stream.writeUint32(value.type);
			exchange_list<true, uint32_t>(stream, value.data, [](DataStreamView &stream, STIDDATA const &element) {
                stream.writeUint32(element.id);
                stream.writeStringByUint8(element.name);
            });
			return;
		}

		inline static auto exchange_platform_settings(
            DataStreamView &stream,
            PLAT const &value
        ) -> void
        {
            stream.writeStringByEmpty(value.platform);
            return;
        }

		inline static auto exchange_sound_bank(
			DataStreamView &stream,
			SoundBankInformation const &definition,
			std::string_view source) -> void
		{
			auto sign = ChunkSign{};
			sign.id = ChunkSignFlag::bkhd;
			chuck_sign_set(stream, sign);
			exchange_bank_header(stream, definition.bank_header);
			chuck_sign_write(stream, sign);
			if (definition.has_embedded_media)
			{
				sign.id = ChunkSignFlag::didx;
				exchange_embedded_media(stream, definition.embedded_media, sign, source);
			}
			if (definition.has_initialization)
			{
				sign.id = ChunkSignFlag::init;
				chuck_sign_set(stream, sign);
				exchange_plugin_chunk(stream, definition.initialization);
				chuck_sign_write(stream, sign);
			}
			if (definition.has_game_synchronization)
			{
				sign.id = ChunkSignFlag::stmg;
				chuck_sign_set(stream, sign);
				exchange_game_synchronization(stream, definition.game_synchronization);
				chuck_sign_write(stream, sign);
			}
			if (definition.has_hierarchy)
			{
				sign.id = ChunkSignFlag::hirc;
				chuck_sign_set(stream, sign);
				exchange_hierarchy(stream, definition.hierarchy);
				chuck_sign_write(stream, sign);
			}
			if (definition.has_environments)
			{
				sign.id = ChunkSignFlag::envs;
				chuck_sign_set(stream, sign);
				exchange_environments_settings(stream, definition.environments);
				chuck_sign_write(stream, sign);
			}
			if (definition.has_reference)
			{
				sign.id = ChunkSignFlag::stid;
				chuck_sign_set(stream, sign);
				exchange_string_mapping(stream, definition.reference);
				chuck_sign_write(stream, sign);
			}
			if (definition.has_platform_setting)
			{
				sign.id = ChunkSignFlag::plat;
				chuck_sign_set(stream, sign);
				exchange_platform_settings(stream, definition.platform_setting);
				chuck_sign_write(stream, sign);
			}
			return;
		}

	public:
		inline static auto process_whole(
			DataStreamView &stream,
			SoundBankInformation const &definition,
			std::string_view source) -> void
		{
			exchange_sound_bank(stream, definition, source);
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination) -> void
		{
			auto stream = DataStreamView{};
			auto definition = *FileSystem::read_json(fmt::format("{}/definition.json", source));
			process_whole(stream, definition, source);
			stream.out_file(destination);
			return;
		}
	};
}