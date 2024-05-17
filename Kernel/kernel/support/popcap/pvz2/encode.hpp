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

	private:
		inline static auto constexpr packer_max_width = 4096;
		inline static auto constexpr packer_max_height = 4096;
		inline static auto constexpr packer_padding = 1;
		inline static auto constexpr log2e = 1.4426950408889634;

	protected:
		DataStreamView stream;

		inline auto toupper(
			std::string &str) const -> void
		{
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
			return;
		}

		inline auto toupperback(
			const std::string &str) const -> std::string
		{
			auto upper_str = std::string{str.data(), str.size()};
			std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);
			return upper_str;
		}

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

		// popanim
		inline auto encode_popanim(const std::string &source) const -> std::vector<uint8_t>
		{
			auto popanim_encode = Animation::Convert::FromFlash{};
			auto record_info = *FileSystem::read_json(fmt::format("{}/record.json", source));
			popanim_encode.process(source, record_info);
			auto animation_encode = std::make_unique<Animation::Encode>(popanim_encode.animation);
			animation_encode->process();
			return animation_encode->sen->toBytes();
		}

		template <auto write_type>
		inline auto encode_file(
			std::string_view source,
			const DataInfo &info) const -> void
		{
			static_assert(write_type == false || write_type == true, "write_type must be true or false");
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
					data[id] = nlohmann::ordered_json{{"type", type_to_string(packet_type)}, {"path", path}};
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
				else if (packet_type == PopAnim)
				{
					auto path_list = String{path}.split("/");
					path_list.insert(path_list.begin() + 1, "768"); // 768
					path = fmt::format("{}.pam", String::join(path_list, "/"));
					packet_bank[path] = std::move(encode_popanim(data_path));
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
			if constexpr (write_type)
			{
				stream.writeUint32(0);
			}
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

		inline auto convert_texture(
			uint32_t &format,
			const Definition::Image<int> &image) const -> std::vector<uint8_t>
		{
			switch (format)
			{
			case 0x00000000:
			{ // RGBA_8888
				return Texture::Encode::rgba(image);
			}
			case 0xFFFFFFFF:
			{ // ARGB_8888
				return Texture::Encode::argb(image);
			}
			case 0x0000001E:
			{ // RGBA_PVRTC_4BPP
				return Texture::Encode::rgb_pvrtc_4bpp(image);
			}
			case 0x00000093:
			{ // RGB_ETC1_A_8
				return Texture::Encode::rgb_etc1_a_8(image);
			}
			case 0x00000092:
			{ // RGB_ETC1_A_PALETTE
				format += 1;
				return Texture::Encode::rgb_etc1_a_palette(image);
			}
			case 0x00000094:
			{ // RGBA_PVRTC_4BPP_A8
				return Texture::Encode::rgb_pvrtc_4bpp_a8(image);
			}
			default:
			{
				throw Exception("invaild_image_format", std::source_location::current(), "convert_texture");
			}
			}
		}

		inline auto encode_image(
			std::string_view source,
			std::string_view s_name,
			uint32_t &format,
			uint32_t compression_flags,
			const std::map<std::string, DataResInfo> &info) const -> void
		{
			// join_sprites
			auto images_map = std::map<std::string, Definition::Image<int>>{};
			auto packer_list = std::vector<MaxRectsAlgorithm::Rectangle>{};

			auto temp_width = packer_max_width;
			auto temp_height = packer_max_height;
			for (const auto &[id, data] : info)
			{
				const auto path = fmt::format("{}/{}", source, data.path);
				auto image = Definition::ImageIO::read_png(path);
				images_map.emplace(id, image);
				packer_list.emplace_back(MaxRectsAlgorithm::Rectangle(image.width, image.height, id));
				if (image.width > temp_width)
				{
					temp_width = image.width;
				}
				if (image.height > temp_height)
				{
					temp_height = image.height;
				}
			}
			temp_width = std::pow(2, std::ceil(std::log(temp_width) * log2e));
			temp_height = std::pow(2, std::ceil(std::log(temp_height) * log2e));
			auto packer = MaxRectsAlgorithm::MaxRectsPacker(temp_width, temp_height, packer_padding);
			packer.addArray(packer_list);
			auto &bins = packer.bins;
			auto image_list = std::vector<Definition::Image<int>>{};
			auto destination = std::map<std::string, Definition::Image<int>>{};
			auto packet = nlohmann::ordered_json{};
			auto packet_bank = std::map<std::string, std::vector<uint8_t>>{};
			auto packet_info = RSG::PacketInfo{4, compression_flags};
			auto rsg_info = std::vector<RSG::ResInfo>{};
			for (const auto &i : Range(bins.size()))
			{
				auto &bin = bins[i];
				const auto parent_name = fmt::format("{}_{}", s_name, (i < 10) ? "0" + std::to_string(i) : std::to_string(i));
				const auto packet_id = fmt::format("ATLASIMAGE_ATLAS_{}", toupperback(parent_name));
				auto path = fmt::format("atlases/{}", parent_name);
				packet[packet_id] = nlohmann::ordered_json{
					{"type", "Image"}, {"path", path}, {"dimension", {{"width", bin.width}, {"height", bin.height}}}};
				for (auto &rect : bin.rects)
				{
					auto &info_data = info.at(rect.source);
					auto sprite_path_list = String{info_data.path}.split("/");
					sprite_path_list.insert(sprite_path_list.begin() + 1, "1536");
					auto length = sprite_path_list.size();
					if (length - 2 >= 0 && sprite_path_list.at(length - 2) == "media") {
						sprite_path_list.erase(sprite_path_list.end() - 2); // delete media
					}
					if (length - 3 >= 0 && sprite_path_list.at(length - 3) == "libray") {
						sprite_path_list.erase(sprite_path_list.end() - 3); // delete libray
					}
					auto ptx_default = nlohmann::ordered_json{
						{"ax", rect.x}, {"ay", rect.y}, {"aw", rect.width}, {"ah", rect.height}, {"x", info_data.ptx_default_info.x}, {"y", info_data.ptx_default_info.y}};
					if (info_data.ptx_default_info.cols != 0)
					{
						ptx_default["cols"] = info_data.ptx_default_info.cols;
					}
					if (info_data.ptx_default_info.rows != 0)
					{
						ptx_default["rows"] = info_data.ptx_default_info.rows;
					}
					packet[packet_id]["data"][rect.source] = nlohmann::ordered_json{
						{"type", "Image"},
						{"path", String::join(sprite_path_list, "/")},
						{"default", ptx_default}};
					auto &image = images_map.at(rect.source);
					image_list.emplace_back(image);
				}
				for (auto i : Range(bin.rects.size()))
				{
					image_list[i].x = bin.rects[i].x;
					image_list[i].y = bin.rects[i].y;
				}
				auto image_merged = Definition::Image<int>::transparent(Definition::Dimension{bin.width, bin.height});
				Definition::ImageIO::join(image_merged, image_list);
				path += ".ptx";
				packet_bank.emplace(path, convert_texture(format, image_merged));
				auto res_info = RSG::ResInfo{path, true};
				res_info.ptx_info = RSG::PTXInfo{static_cast<uint32_t>(i), static_cast<uint32_t>(bin.width), static_cast<uint32_t>(bin.height)};
				rsg_info.emplace_back(res_info);
			}
			auto subgroup = nlohmann::ordered_json{};
			subgroup["type"] = "1536";
			subgroup["packet"] = packet;
			packet_info.res = rsg_info;
			auto rsg_pack = RSG::Pack{};
			rsg_pack.process<false>("", packet_info, packet_bank);
			auto rsg_data = rsg_pack.sen.toBytes();
			// write_to_data
			stream.writeUint32(rsg_data.size());
			stream.writeUint32(stream.write_pos + 16);
			stream.writeUint32(format);
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
			std::string_view s_name,
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
			if (type == Image)
			{
				auto format = info.format;
				const auto compression_flags = static_cast<std::uint32_t>(info.compression_flags);
				encode_image(source, s_name, format, compression_flags, info.data);
			}
			else if (type == File)
			{
				encode_file<true>(source, info);
			}
			else
			{
				throw Exception("invaild_data_type", std::source_location::current(), "encode_composite_group"); // TODO add localizaiton;
			}
			return;
		}

		inline auto process(
			std::string_view source,
			const Info &info) const -> void
		{
			const auto resources_folder = fmt::format("{}/resources", source);
			const auto is_composite = info.is_composite;
			stream.writeString(data_magic);
			stream.writeUint32(1);
			stream.writeNull(8);
			stream.writeUint32(info.groups.size());
			for (const auto &[s_name, value] : info.groups)
			{
				if (is_composite)
				{
					stream.writeUint32(1);
					encode_composite_group(resources_folder, s_name, value);
					stream.writeStringByEmpty(s_name);
				}
				else
				{
					stream.writeUint32(0);
					encode_file<false>(resources_folder, value);
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
			Encode &&that)
			-> Encode & = delete;

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
			encode.process_whole(source);
			encode.stream.out_file(destination);
			return;
		}
	};
}