#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/pvz2/common.hpp"
#include "kernel/support/popcap/rsg/pack.hpp"
#include "kernel/support/popcap/animation/encode.hpp"
#include "kernel/support/popcap/animation/convert/from_flash.hpp"
#include "kernel/support/popcap/rton/encode.hpp"
#include "kernel/support/wwise/sound_bank/encode.hpp"
#include "kernel/support/texture/encode.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2
{

	using namespace Definition;

	using namespace simdjson;

	class Encode
	{

	protected:
		DataStreamView stream;

	protected:
		// rton
		inline auto encode_rton(std::string_view source) const -> std::vector<uint8_t>
		{
			auto parser = ondemand::parser{};
			auto str = padded_string::load(source);
			auto json = static_cast<ondemand::document>(parser.iterate(str));
			auto rton_encode = RTON::Encode{};
			rton_encode.encode_rton(json);
			return rton_encode.sen->toBytes();
		}

		// soundbank
		inline auto encode_soundbank(std::string_view source) const -> std::vector<uint8_t>
		{
			auto encoder = WWise::SoundBank::Encode{};
			encoder.process(*FileSystem::read_json(fmt::format("{}/definition.json", source)),
							fmt::format("{}/embedded_audio", source));
			return encoder.stream.toBytes();
		}

		inline auto encode_file(
			std::string_view source,
			const DataInfo &info) const -> void
		{
			const auto type = info.type;
			const auto compression_flags = static_cast<std::uint32_t>(info.compression_flags);
			auto subgroup = nlohmann::ordered_json{};
			subgroup["type"] = nullptr;
			auto data = nlohmann::ordered_json{};
			auto packet_bank = std::map<std::string, std::vector<uint8_t>>{};
			auto packet_info = RSG::PacketInfo{4, compression_flags};
			auto rsg_info = std::vector<RSG::ResInfo>{};
			for (const auto &[id, packet] : info.data)
			{
				auto path = packet.path;
				const auto packet_type = packet.type;
				if (path == "!program")
				{	
					rsg_info.emplace_back(RSG::ResInfo{path, false});
					continue;
				}
				const auto data_path = fmt::format("{}/{}", source, path);
				if (packet_type == Data)
				{
					path = fmt::format("{}/{}.rton", Path::getParents(path), Path::getFileNameWithoutExtension(path));
					packet_bank[path] = std::move(encode_rton(data_path));
				}
				else if (packet_type == SoundBank || packet_type == DecodedSoundBank)
				{
					path = fmt::format("{}/{}.bnk", Path::getParents(path), Path::getFileNameWithoutExtension(path));
					packet_bank[path] = std::move(encode_soundbank(data_path));
				}
				else if (packet_type == File || packet_type == PrimeFont || packet_type == RenderEffect || packet_type == PopAnim || packet_type == Image)
				{
					packet_bank[path] = std::move(FileSystem::read_binary<uint8_t>(data_path));
				}
				else
				{
					throw Exception("invaild_packet_type", std::source_location::current(), "encode_file"); // TODO add localization;
				}
				data[id] = nlohmann::ordered_json{{"type", type_to_string(packet_type)}, {"path", path}};
				rsg_info.emplace_back(RSG::ResInfo{path, false});
			}
			packet_info.res = rsg_info;
			subgroup["packet"] = nlohmann::ordered_json{{"type", type_to_string(type)}, {"data", data}};
			auto rsg_pack = RSG::Pack{};
			rsg_pack.process<false>("", packet_info, packet_bank);
			auto rsg_data = rsg_pack.sen.toBytes();
			stream.writeUint32(rsg_data.size());
			stream.writeUint32(stream.write_pos + 12);
			const auto before_pos = stream.write_pos;
			stream.writeNull(8);
			stream.writeBytes(rsg_data);
			const auto current_pos = stream.write_pos;
			auto subgroup_info = subgroup.dump();
			stream.writeUint32(subgroup_info.size(), before_pos);
			stream.writeUint32(current_pos);
			stream.writeString(subgroup_info, current_pos);
			return;
		}

		inline auto encode_composite_group(
			std::string_view source,
			const DataInfo &info
		) const -> void
		{
			return;
		}

		inline auto process(
			std::string_view source,
			const Info &info) const -> void
		{
			const auto resources_folder = fmt::format("{}/resources", source);
			const auto is_composite = info.is_composite;
			debug(is_composite);
			stream.writeString(data_magic);
			stream.writeUint32(1);
			stream.writeNull(8);
			stream.writeUint32(info.groups.size());
			for (const auto &[s_name, value] : info.groups)
			{
				if (is_composite)
				{
					stream.writeUint32(1);
					encode_composite_group(resources_folder, value);
					stream.writeStringByEmpty(s_name);
				}
				else
				{
					stream.writeUint32(0);
					encode_file(resources_folder, value);
					stream.writeStringByEmpty(s_name);
				}
			}
			return;
		}

	public:
		explicit Encode(

			) = default;

		~Encode(

			) = default;

		auto operator=(
			Encode &&that) -> Encode & = delete;

		Encode(
			Encode &&that) = delete;

		inline auto process_whole(
			std::string_view source) const -> void
		{
			auto data_info = *FileSystem::read_json(fmt::format("{}/info.json", source));
			process(source, data_info);
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination) -> void
		{
			auto encode = Encode{};
			debug(source);
			encode.process_whole(source);
			encode.stream.out_file(destination);
			return;
		}
	};

}