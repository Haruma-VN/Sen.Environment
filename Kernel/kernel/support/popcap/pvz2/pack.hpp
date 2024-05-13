#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/pvz2/common.hpp"
#include "kernel/support/popcap/rsb/pack.hpp"
#include "kernel/support/popcap/rsb/definition.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2
{

	using namespace Definition;

	using namespace Sen::Kernel::Support::PopCap::RSB;

	class Pack
	{

	protected:
		// std::unique_ptr<DataStreamView> stream;

	protected:
		inline auto pack_packages(const std::string &source, const PackagesSetting &setting) const -> void
		{
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
				auto packet_info = nlohmann::ordered_json{{"path", path + ".ptx"}, 
				{"ptx_info", { 
					{"id", 0}, {"width", width}, 
					{"height", packet["dimension"]["height"].get<uint32_t>()}, 
					{"format", format == 0xFFFFFFFF ? 0 : format}, {"pitch", width * 4}}}};
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
					const auto rsg_data_size = data_stream.readUint32();
					const auto rsg_pos = data_stream.readUint32();
					auto format = data_stream.readUint32();
					const auto current_pos = data_stream.read_pos;
					const auto compression_flags = data_stream.readUint32(static_cast<size_t>(rsg_pos + 0x10));
					auto rsg_data = data_stream.getBytes(rsg_pos, rsg_pos + rsg_data_size);
					const auto res_size = data_stream.readUint32(current_pos);
					auto res_info = nlohmann::ordered_json::parse(data_stream.readString(res_size, static_cast<std::size_t>(data_stream.readUint32())));
					const auto subgroup_name = data_stream.readStringByEmpty();
					res_groups[composite_name]["subgroup"][subgroup_name] = res_info;
					rsg_bank[subgroup_name] = std::move(rsg_data);
					auto rsg_info = RSG_Info<uint32_t>{};
					if (res_info["type"] != nullptr)
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
			auto rsg_bank = std::map<std::string, std::vector<uint8_t>>{};
			auto res = nlohmann::ordered_json{};
			if (info.use_packages)
			{
				pack_packages(fmt::format("{}/resources", source), info.packages_setting);
			}
			for (const auto &composite_name : info.group)
			{
				process_data(composite_name,
							 FileSystem::read_binary<uint8_t>(fmt::format("{}/packet/{}.data", source, composite_name)),
							 manifest.group, rsg_bank, res["groups"]);
			}
			res["expand_path"] = info.use_newton ? "string" : "array";
			// FileSystem::write_json("C:/Users/Windows/Desktop/Example/res.json", res);
			// FileSystem::write_json("C:/Users/Windows/Desktop/Example/manifest.json", manifest);
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