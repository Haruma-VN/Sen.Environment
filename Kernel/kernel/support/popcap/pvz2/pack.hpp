#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/pvz2/common.hpp"
#include "kernel/support/popcap/rsb/pack.hpp"
#include "kernel/support/popcap/rsb/definition.hpp"
#include "kernel/support/popcap/res_info/convert.hpp"
#include "kernel/support/popcap/newton/encode.hpp"
#include "kernel/support/popcap/rton/encode.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2
{

	using namespace Definition;

	using namespace Sen::Kernel::Support::PopCap::RSB;

	class Pack
	{

	private:
		using Rijndael = Sen::Kernel::Definition::Encryption::Rijndael;

	protected:
		inline static auto const PackagesName = std::string{"Packages"};

		inline static auto const ManifestGroupName = std::string{"__MANIFESTGROUP__"};

		uint32_t mutable ptx_info_size;

	protected:
		inline auto encode_rton(const std::string &source, bool rton_encryted) const -> std::vector<uint8_t>
		{
			auto parser = ondemand::parser{};
			auto str = padded_string::load(source);
			auto json = static_cast<ondemand::document>(parser.iterate(str));
			auto rton_encode = RTON::Encode{};
			rton_encode.encode_rton(json);
			if (rton_encryted)
			{
				const auto key = "65bd1b2305f46eb2806b935aab7630bb"_sv;
				const auto iv = "1b2305f46eb2806b935aab76"_sv;
				auto encrypted_data = DataStreamView{};
				encrypted_data.writeUint8(0x10);
				encrypted_data.writeUint8(0x00);
				auto padding = iv.size() - ((rton_encode.sen->size() + iv.size() - 1) % iv.size() + 1);
				rton_encode.sen->writeNull(padding);
				encrypted_data.writeBytes(Rijndael::encrypt<std::size_t, Rijndael::Mode::CBC>(
					reinterpret_cast<char *>(rton_encode.sen->getBytes(0, rton_encode.sen->size()).data()),
					key,
					iv,
					rton_encode.sen->size()));
				return encrypted_data.toBytes();
			}
			return rton_encode.sen->toBytes();
		}

		inline auto pack_packages(
			const std::string &source,
			const PackagesSetting &setting,
			std::map<std::string, RSG_Group<uint32_t>> &manifest_groups,
			std::map<std::string, std::vector<uint8_t>> &rsg_bank,
			nlohmann::ordered_json &res_groups) const -> void
		{
			auto packages_items_list = FileSystem::read_whole_directory(source);
			if (setting.auto_convert_json)
			{
				auto packages_json_list = std::vector<std::string>{};
				for (auto &p : packages_items_list)
				{
					if (Common::compare_string(Path::getExtension(p), ".json"))
					{
						FileSystem::write_binary(fmt::format("{}/{}.rton", Path::getParents(p), Path::getFileNameWithoutExtension(p)), encode_rton(p, setting.rton_encryted));
					}
				}
			}
			const auto source_size = source.size() + 1;
			auto packet_bank = std::map<std::string, std::vector<uint8_t>>{};
			auto packet_info = RSG::PacketInfo{4, 1};
			auto res_info = std::vector<RSG::ResInfo>{};
			auto rsg_info = RSG_Info<uint32_t>{};
			rsg_info.packet_info.compression_flags = 1;
			rsg_info.category = nlohmann::ordered_json::array({0, nullptr});
			auto data = nlohmann::ordered_json{};
			for (auto &p : packages_items_list)
			{
				if (Common::compare_string(Path::getExtension(p), ".rton"))
				{
					auto item_data = FileSystem::read_binary<uint8_t>(p);
					p.erase(p.begin(), p.begin() + source_size);
					auto path = String::replaceAll(p, Common::WindowStyle, Common::PosixStyle);
					packet_bank.emplace(path, std::move(item_data));
					res_info.emplace_back(RSG::ResInfo{path, false});
					rsg_info.packet_info.res.emplace_back(nlohmann::ordered_json{{"path", path}});
					data[fmt::format("RESFILE_PACKAGES_{}", Path::getFileNameWithoutExtension(path))] = nlohmann::ordered_json{
						{"type", "File"}, {"path", path}};
				}
			}
			res_groups[PackagesName] = nlohmann::ordered_json{{"is_composite", false}, {"subgroup", {}}};
			res_groups[PackagesName]["subgroup"][PackagesName] = nlohmann::ordered_json{{"type", nullptr}, {"packet", {{"type", "File"}, {"data", data}}}};
			packet_info.res = res_info;
			auto rsg_pack = RSG::Pack{};
			rsg_pack.process<false>("", packet_info, packet_bank);
			rsg_bank[PackagesName] = std::move(rsg_pack.sen.toBytes());
			auto rsg_group = RSG_Group<uint32_t>{};
			rsg_group.subgroup[PackagesName] = rsg_info;
			manifest_groups[PackagesName] = rsg_group;
			return;
		}

		inline auto process_manifest(
			bool use_newton,
			std::map<std::string, RSG_Group<uint32_t>> &manifest_groups,
			std::map<std::string, std::vector<uint8_t>> &rsg_bank,
			nlohmann::ordered_json &res) const -> void
		{
			auto result = nlohmann::ordered_json{
				{"version", 1},
				{"content_version", 1},
				{"slot_count", 0},
				{"groups", nlohmann::ordered_json::array()}};
			res["expand_path"] = use_newton ? "string" : "array";
			PopCap::ResInfo::Convert::convert(res, result);
			auto packet_info = RSG::PacketInfo{4, 1};
			auto res_info = std::vector<RSG::ResInfo>{};
			auto rsg_info = RSG_Info<uint32_t>{};
			rsg_info.packet_info.compression_flags = 1;
			rsg_info.category = nlohmann::ordered_json::array({0, nullptr});
			auto packet_bank = std::map<std::string, std::vector<uint8_t>>{};
			if (use_newton)
			{
				auto newton_encode = Newton::Encode{result};
				newton_encode.process();
				auto newton_path = "PROPERTIES/RESOURCES.NEWTON"_sv;
				packet_bank.emplace(newton_path, std::move(newton_encode.sen->toBytes()));
				res_info.emplace_back(RSG::ResInfo{newton_path.data(), false});
				rsg_info.packet_info.res.emplace_back(nlohmann::ordered_json{{"path", newton_path}});
			}
			// RTON pack
			auto rton_encode = RTON::Encode{};
			auto parser = ondemand::parser{};
			auto str = result.dump();
			auto str_json = simdjson::padded_string(str.data(), str.size());
			auto json = static_cast<ondemand::document>(parser.iterate(str_json));
			rton_encode.encode_rton(json);
			auto rton_path = "PROPERTIES/RESOURCES.RTON"_sv;
			packet_bank.emplace(rton_path, std::move(rton_encode.sen->toBytes()));
			res_info.emplace_back(RSG::ResInfo{rton_path.data(), false});
			rsg_info.packet_info.res.emplace_back(nlohmann::ordered_json{{"path", rton_path}});
			packet_info.res = res_info;
			auto rsg_pack = RSG::Pack{};
			rsg_pack.process<false>("", packet_info, packet_bank);
			rsg_bank[ManifestGroupName] = std::move(rsg_pack.sen.toBytes());
			auto rsg_group = RSG_Group<uint32_t>{};
			rsg_group.subgroup[ManifestGroupName] = rsg_info;
			manifest_groups[ManifestGroupName] = rsg_group;
			return;
		}

		inline auto resinfo_to_submanifest(
			nlohmann::ordered_json &res_info,
			RSG_Info<uint32_t> &rsg_info) const -> void
		{
			rsg_info.category = nlohmann::ordered_json::array({0, nullptr});
			for (const auto &[id, packet] : res_info["packet"]["data"].items())
			{
				const auto path = packet["path"].get<std::string>();
				if (path == "!program")
				{
					continue;
				}
				rsg_info.packet_info.res.emplace_back(
					nlohmann::ordered_json{{"path", path}});
			}
			return;
		}

		inline auto resinfo_to_submanifest_composite(
			nlohmann::ordered_json &res_info,
			RSG_Info<uint32_t> &rsg_info,
			uint32_t format) const -> void
		{
			rsg_info.category = nlohmann::ordered_json::array({1536, nullptr});
			for (const auto &[id, packet] : res_info["packet"].items())
			{
				const auto path = packet["path"].get<std::string>();
				if (path == "!program")
				{
					continue;
				}
				auto width = packet["dimension"]["width"].get<uint32_t>();
				auto ptx_info = nlohmann::ordered_json{{"id", 0}, {"width", width}, {"height", packet["dimension"]["height"].get<uint32_t>()}, {"format", format == 0xFFFFFFFF ? 0 : format}, {"pitch", width * 4}};
				if (ptx_info_size != 0x10) {
					ptx_info["alpha_size"] = 100;
				}
				if (ptx_info_size == 0x18) {
					ptx_info["alpha_format"] = 0;
				}
				auto packet_info = nlohmann::ordered_json{{"path", path + ".ptx"}, {"ptx_info", ptx_info}};
				rsg_info.packet_info.res.emplace_back(packet_info);
			}
			return;
		}

		inline auto process_data(
			const std::string &composite_name,
			const std::vector<uint8_t> &data,
			std::map<std::string, RSG_Group<uint32_t>> &manifest_groups,
			std::map<std::string, std::vector<uint8_t>> &rsg_bank,
			nlohmann::ordered_json &res_groups) const -> void
		{
			auto data_stream = DataStreamView{data};
			assert_conditional(data_stream.readString(4) == data_magic, "invaild_data_magic", "process"); // TODO add localizaiton;
			assert_conditional(data_stream.readUint32() == 1, "invaild_data_version", "process");		  // TODO add localizaiton;
			data_stream.read_pos += 8;
			const auto num_packet = data_stream.readUint32();
			auto composite_type = 0;
			auto rsg_group = RSG_Group<uint32_t>{};
			for (const auto &i : Range(num_packet))
			{
				composite_type = data_stream.readUint32();
				if (composite_type == 0)
				{
					const auto rsg_data_size = data_stream.readUint32();
					const auto rsg_pos = data_stream.readUint32();
					const auto current_pos = data_stream.read_pos;
					const auto compression_flags = data_stream.readUint32(static_cast<size_t>(rsg_pos + 0x10));
					auto rsg_data = data_stream.getBytes(rsg_pos, rsg_pos + rsg_data_size);
					const auto res_size = data_stream.readUint32(current_pos);
					auto res_info = nlohmann::ordered_json::parse(data_stream.readString(res_size, static_cast<std::size_t>(data_stream.readUint32())));
					const auto subgroup_name = data_stream.readStringByEmpty();
					res_groups[composite_name]["subgroup"][subgroup_name] = res_info;
					rsg_bank[subgroup_name] = std::move(rsg_data);
					auto rsg_info = RSG_Info<uint32_t>{};
					rsg_info.packet_info.compression_flags = compression_flags;
					resinfo_to_submanifest(res_info, rsg_info);
					rsg_group.subgroup[subgroup_name] = rsg_info;
				}
				else if (composite_type == 1)
				{
					const auto res_type = data_stream.readUint32();
					const auto rsg_data_size = data_stream.readUint32();
					const auto rsg_pos = data_stream.readUint32();
					auto format = res_type == 1 ? data_stream.readUint32() : 0;
					const auto current_pos = data_stream.read_pos;
					const auto compression_flags = data_stream.readUint32(static_cast<size_t>(rsg_pos + 0x10));
					auto rsg_data = data_stream.getBytes(rsg_pos, rsg_pos + rsg_data_size);
					const auto res_size = data_stream.readUint32(current_pos);
					auto res_info = nlohmann::ordered_json::parse(data_stream.readString(res_size, static_cast<std::size_t>(data_stream.readUint32())));
					const auto subgroup_name = data_stream.readStringByEmpty();
					res_groups[composite_name]["subgroup"][subgroup_name] = res_info;
					rsg_bank[subgroup_name] = std::move(rsg_data);
					auto rsg_info = RSG_Info<uint32_t>{};
					if (res_type == 1)
					{
						resinfo_to_submanifest_composite(res_info, rsg_info, format);
					}
					else
					{
						resinfo_to_submanifest(res_info, rsg_info);
					}
					rsg_info.packet_info.compression_flags = compression_flags;
					rsg_group.subgroup[subgroup_name] = rsg_info;
				}
				else
				{
					throw Exception("invaild_data_type", std::source_location::current(), "process"); // TODO add localizaiton;
				}
			}
			const auto is_composite = composite_type == 1;
			rsg_group.is_composite = is_composite;
			res_groups[composite_name]["is_composite"] = is_composite;
			manifest_groups[composite_name] = rsg_group;
			return;
		}

		inline auto process(std::string_view source, const ManifestInfo &info, std::string_view destination) const -> void
		{
			auto manifest = RSB::Manifest<std::uint32_t>{};
			manifest.version = 4;
			manifest.ptx_info_size = info.ptx_info_size;
			ptx_info_size = info.ptx_info_size;
			auto rsg_bank = std::map<std::string, std::vector<uint8_t>>{};
			auto res = nlohmann::ordered_json{};
			for (const auto &composite_name : info.group)
			{
				process_data(composite_name,
							 FileSystem::read_binary<uint8_t>(fmt::format("{}/packet/{}.data", source, composite_name)),
							 manifest.group, rsg_bank, res["groups"]);
			}
			if (info.use_packages)
			{
				pack_packages(fmt::format("{}/resources", source), info.packages_setting, manifest.group, rsg_bank, res["groups"]);
			}
			process_manifest(info.use_newton, manifest.group, rsg_bank, res);
			auto rsb_pack = RSB::Pack{};
			rsb_pack.process<true>(source, manifest, rsg_bank);
			rsb_pack.sen->out_file(destination);
			return;
		}

	public:
		explicit Pack(

			) = default;

		~Pack(

			) = default;

		auto operator=(
			Pack &&that) -> Pack & = delete;

		Pack(
			Pack &&that) = delete;

		inline auto process_whole(
			std::string_view source,
			const ManifestInfo &info,
			std::string_view destination) const -> void
		{
			process(source, info, destination);
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination) -> void
		{
			using Pack = Pack;
			auto pack = Pack{};
			auto info = *FileSystem::read_json(fmt::format("{}/info.json", source));
			pack.process(source, info, destination);
			// pack.stream->out_file(destination);
			return;
		}
	};

}