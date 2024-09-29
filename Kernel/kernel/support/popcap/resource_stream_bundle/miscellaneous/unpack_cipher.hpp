#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_stream_group/unpack.hpp"
#include "kernel/support/popcap/resource_stream_bundle/unpack.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle::Miscellaneous
{
	using namespace Definition;

	using namespace Sen::Kernel::Support::PopCap::ResourceStreamBundle;

	struct UnpackCipher
	{
	protected:
		inline static constexpr auto k_resource_data_section_compression_offset_in_packet = 0x10_size;

		inline static constexpr auto k_resource_information_section_offset_in_packet = 0x4C_size;

		inline static constexpr auto k_vaild_resource_information_section_offset_list = std::array<uint32_t, 2>{0x5C, 0x1000};

		template <auto t_version>
		inline static auto exchange_cipher_basic_subgroup(
			DataStreamView &stream,
			Common::BasicSubgroupInformation &value) -> void
		{
			stream.read_pos += Common::k_subgroup_name_string_block_size;
			value.offset = stream.readUint32();
			value.size = stream.readUint32();
			value.pool = stream.readUint32();
			value.resource_data_section_compression = stream.readUint32();
			value.information_section_size = stream.readUint32();
			value.general_resource_data_section_offset = stream.readUint32();
			value.general_resource_data_section_size = stream.readUint32();
			value.general_resource_data_section_size_original = stream.readUint32();
			value.general_resource_data_section_size_pool = stream.readUint32();
			value.texture_resource_data_section_offset = stream.readUint32();
			value.texture_resource_data_section_size = stream.readUint32();
			value.texture_resource_data_section_size_original = stream.readUint32();
			value.texture_resource_data_section_size_pool = stream.readUint32();
			stream.read_pos += 16_size;
			if constexpr (t_version >= 3_size)
			{
				value.texture_resource_count = stream.readUint32();
				value.texture_resource_begin = stream.readUint32();
			}
			return;
		}

		inline static auto find_subgroup_id(
			std::map<std::string, uint32_t> &value,
			uint32_t const &index) -> std::string
		{
			auto subgroup_id = std::string{};
			for (auto &[s_id, s_index] : value)
			{
				if (index == s_index)
				{
					subgroup_id = s_id;
					break;
				}
			}
			try_assert(!subgroup_id.empty(), "cannot_find_subgroup_index");
			value.erase(subgroup_id);
			return subgroup_id;
		}

		inline static auto exchange_packet(
            std::vector<uint8_t> const &packet_data,
            std::string destination
        ) -> void
        {
            auto packet_stream = DataStreamView{packet_data};
            auto packet_definition = ResourceStreamGroup::PacketStructure{};
            ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_definition, destination);
            return;
        }

		inline static auto process_package(
			DataStreamView &stream,
			BundleStructure &definition,
			ManifestStructure &manifest,
			std::string &destination) -> void
		{
			auto information_structure = Common::Information{};
			Common::exchange_to_header(stream, information_structure.header);
			assert_conditional(information_structure.header.magic == Common::k_magic_identifier, "invalid_magic_header", "process_package");
			auto index = std::find(Common::k_version_list.begin(), Common::k_version_list.end(), static_cast<int>(information_structure.header.version));
			assert_conditional((index != Common::k_version_list.end()), String::format(fmt::format("{}", Language::get("popcap.rsb.invalid_rsb_version")), std::to_string(static_cast<int>(information_structure.header.version))), "process");
			definition.version = information_structure.header.version;
			CompiledMapData::decode(stream, information_structure.header.group_id_section_offset, information_structure.header.group_id_section_size, information_structure.group_id, &Common::exchange_to_index);
			CompiledMapData::decode(stream, information_structure.header.subgroup_id_section_offset, information_structure.header.subgroup_id_section_size, information_structure.subgroup_id, &Common::exchange_to_index);
			// CompiledMapData::decode(stream, information_structure.header.resource_path_section_offset, information_structure.header.resource_path_section_size, information_structure.resource_path, &exchange_to_index);
			stream.read_pos = information_structure.header.group_information_section_offset;
			exchange_list(stream, information_structure.group_information, &Common::exchange_to_simple_group, static_cast<size_t>(information_structure.header.group_information_section_block_count));
			stream.read_pos = information_structure.header.subgroup_information_section_offset;
			if (definition.version == 1_ui)
			{
				try_assert(definition.version == 1_ui && information_structure.header.unknown_1 == 1_ui, "invalid_header_block");
				exchange_list(stream, information_structure.subgroup_information, &exchange_cipher_basic_subgroup<1_ui>, static_cast<size_t>(information_structure.header.subgroup_information_section_block_count));
			}
			else
			{
				try_assert(definition.version >= 3_ui && information_structure.header.unknown_1 == 0_ui, "invalid_header_block");
				exchange_list(stream, information_structure.subgroup_information, &exchange_cipher_basic_subgroup<3_ui>, static_cast<size_t>(information_structure.header.subgroup_information_section_block_count));
			}
			stream.read_pos = information_structure.header.pool_information_section_offset;
			// exchange_list(stream, information_structure.pool_information, &Common::exchange_to_pool, static_cast<size_t>(information_structure.header.pool_information_section_block_count));
			stream.read_pos = information_structure.header.texture_resource_information_section_offset;
			auto &texture_resource_information_section_block_size = information_structure.header.texture_resource_information_section_block_size;
			switch (texture_resource_information_section_block_size)
			{
			case Common::k_texture_resource_information_section_block_size_version_0:
            {
                for (auto index : Range(information_structure.header.texture_resource_information_section_block_count)) {
                    Common::exchange_to_texture<Common::k_texture_resource_information_section_block_size_version_0>(stream, information_structure.texture_resource_information[index]);
                }
                break;
            }
            case Common::k_texture_resource_information_section_block_size_version_1:
            {
                for (auto index : Range(information_structure.header.texture_resource_information_section_block_count)) {
                    Common::exchange_to_texture<Common::k_texture_resource_information_section_block_size_version_1>(stream, information_structure.texture_resource_information[index]);
                }
                break;
            }
            case Common::k_texture_resource_information_section_block_size_version_2:
            {
                for (auto index : Range(information_structure.header.texture_resource_information_section_block_count)) {
                    Common::exchange_to_texture<Common::k_texture_resource_information_section_block_size_version_2>(stream, information_structure.texture_resource_information[index]);
                }
                break;
            }
			default:
				assert_conditional(false, fmt::format("{}", Language::get("popcap.rsb.invalid_texture_resource_information_section_block_size")), "process_package");
			}
			if (information_structure.header.group_manifest_information_section_offset != 0_ui || information_structure.header.resource_manifest_information_section_offset != 0_ui || information_structure.header.string_manifest_information_section_offset != 0_ui)
			{
				try_assert(information_structure.header.group_manifest_information_section_offset != 0_ui && information_structure.header.resource_manifest_information_section_offset != 0_ui && information_structure.header.string_manifest_information_section_offset != 0_ui, "invalid_manifest_block_section_offset");
				Unpack::process_package_manifest(stream, information_structure.header, manifest);
			}
			definition.texture_information_section_size = texture_resource_information_section_block_size;
			for (auto &[group_id, group_index] : information_structure.group_id)
			{
				auto &simple_group_information = information_structure.group_information.at(group_index);
				auto group_information = GroupInformation{};
				auto original_id = Unpack::make_original_group_id(simple_group_information.id, group_information.composite);
				for (auto subgroup_index : Range(simple_group_information.subgroup_count))
				{
					auto &simple_subgroup_infomation = simple_group_information.subgroup_information.at(subgroup_index);
					auto &basic_subgroup_information = information_structure.subgroup_information.at(simple_subgroup_infomation.index);
					// auto &pool_information = information_structure.pool_information.at(simple_subgroup_infomation.index);
					auto subgroup_id = find_subgroup_id(information_structure.subgroup_id, simple_subgroup_infomation.index);
					auto subgroup_information = SubgroupInformation{};
					subgroup_information.category.resolution = simple_subgroup_infomation.resolution;
					if (definition.version >= 3 && simple_subgroup_infomation.locale != 0_ui)
					{
						Common::fourcc_from_integer(simple_subgroup_infomation.locale, subgroup_information.category.locale);
					}
					subgroup_information.compression = basic_subgroup_information.resource_data_section_compression;
					// packet_compression_from_data(basic_subgroup_information.resource_data_section_compression, subgroup_information.compression);
					auto texture_resource_begin = basic_subgroup_information.texture_resource_begin;
					// auto texture_resource_count = basic_subgroup_information.texture_resource_count;
					// try_assert(pool_information.texture_resource_begin == 0_ui, "invalid_texture_resource");
					// try_assert(pool_information.texture_resource_count == 0_ui, "invalid_texture_resource");
					auto packet_stream = DataStreamView{};
					debug(fmt::format("{}. pos: {}, size: {}", subgroup_id, basic_subgroup_information.offset, basic_subgroup_information.size));
					if (static_cast<size_t>(basic_subgroup_information.offset) + k_resource_information_section_offset_in_packet + 4_size >= stream.size()) {
						continue;
					}
					if (auto resource_information_section_offset = stream.readUint32(static_cast<size_t>(basic_subgroup_information.offset) + k_resource_information_section_offset_in_packet); std::find(k_vaild_resource_information_section_offset_list.begin(), k_vaild_resource_information_section_offset_list.end(), resource_information_section_offset) != k_vaild_resource_information_section_offset_list.end())
					{	
						/*
						if (static_cast<size_t>(basic_subgroup_information.offset) + static_cast<size_t>(basic_subgroup_information.size) > stream.size() || basic_subgroup_information.size == k_none_size) {
							basic_subgroup_information.size = basic_subgroup_information.information_section_size + basic_subgroup_information.general_resource_data_section_size + basic_subgroup_information.texture_resource_data_section_size;
						}
						*/
						basic_subgroup_information.size = basic_subgroup_information.information_section_size + basic_subgroup_information.general_resource_data_section_size + basic_subgroup_information.texture_resource_data_section_size;
						packet_stream.writeBytes(stream.getBytes(static_cast<size_t>(basic_subgroup_information.offset), static_cast<size_t>(basic_subgroup_information.offset) + static_cast<size_t>(basic_subgroup_information.size)));
						if (!(packet_stream.readUint32(k_begin_index) == ResourceStreamGroup::Common::k_magic_identifier && packet_stream.readUint32() == definition.version && packet_stream.readUint32(k_resource_information_section_offset_in_packet) == basic_subgroup_information.resource_data_section_compression))
						{
							auto information_structure_header = ResourceStreamGroup::Common::HeaderInformaiton{};
							information_structure_header.magic = ResourceStreamGroup::Common::k_magic_identifier;
							information_structure_header.version = definition.version;
							information_structure_header.resource_data_section_compression = basic_subgroup_information.resource_data_section_compression;
							information_structure_header.information_section_size = basic_subgroup_information.information_section_size;
							information_structure_header.general_resource_data_section_offset = basic_subgroup_information.general_resource_data_section_offset;
							information_structure_header.general_resource_data_section_size = basic_subgroup_information.general_resource_data_section_size;
							information_structure_header.general_resource_data_section_size_original = basic_subgroup_information.general_resource_data_section_size_original;
							information_structure_header.texture_resource_data_section_offset = basic_subgroup_information.texture_resource_data_section_offset;
							information_structure_header.texture_resource_data_section_size = basic_subgroup_information.texture_resource_data_section_size;
							information_structure_header.texture_resource_data_section_size_original = basic_subgroup_information.texture_resource_data_section_size_original;
							information_structure_header.resource_information_section_size = packet_stream.readUint32(k_resource_information_section_offset_in_packet - 4_size);
							information_structure_header.resource_information_section_offset = resource_information_section_offset;
							ResourceStreamGroup::Common::exchange_header(information_structure_header, packet_stream);
						}
						//stream.read_pos = current_pos;
					}
					else
					{
						//stream.read_pos = current_pos;
						continue; // pass
					}
					packet_stream.read_pos = k_begin_index;
					auto packet_structure = PacketStructure{};
					auto get_packet_structure_only = true;
					ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_structure, destination);
					// try_assert(subgroup_information.compression.general == packet_structure.compression.general, "invalid_general_compression");
					// try_assert(subgroup_information.compression.texture == packet_structure.compression.texture, "invalid_texture_compression");
					for (auto &packet_resource : packet_structure.resource)
					{
						if (packet_resource.use_texture_additional_instead)
						{
							auto &texture_information_structure = information_structure.texture_resource_information[static_cast<size_t>(texture_resource_begin + packet_resource.texture_additional.value.index)];
							packet_resource.texture_additional.value.texture_resource_information_section_block_size = texture_resource_information_section_block_size;
							//try_assert(packet_resource.texture_additional.value.dimension.width == texture_information_structure.size_width, "invalid_texture_width");
							//try_assert(packet_resource.texture_additional.value.dimension.height == texture_information_structure.size_height, "invalid_texture_height");
							packet_resource.texture_additional.value.texture_infomation = TextureResourceInformation{
								.pitch = texture_information_structure.pitch,
								.format = texture_information_structure.format,
								.alpha_size = texture_information_structure.alpha_size,
								.scale = texture_information_structure.scale};
							subgroup_information.category.is_image = true;
						}
					}
					if (subgroup_information.category.is_image && subgroup_information.category.resolution == static_cast<int>(k_none_size)) {
						try 
						{
							auto string_list = String{subgroup_id}.split("_");
							auto resolution = std::stoi(string_list.back());
							subgroup_information.category.resolution = resolution;
						}
						catch (std::invalid_argument &ex) {}
            			catch (std::out_of_range & ex) {}
					}
					subgroup_information.resource = packet_structure.resource;
					group_information.subgroup[subgroup_id] = subgroup_information;
				}
				definition.group[original_id] = group_information;
			}
			return;
		}

	public:
		inline static auto process_whole(
			DataStreamView &stream,
			BundleStructure &definition,
			ManifestStructure &manifest,
			std::string_view destination) -> void
		{
			auto bundle_destination = get_string(destination);
			process_package(stream, definition, manifest, bundle_destination);
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination) -> void
		{
			auto stream = DataStreamView{source};
			auto definition = BundleStructure{};
			auto manifest = ManifestStructure{};
			process_whole(stream, definition, manifest, destination);
			write_json(fmt::format("{}/data.json", destination), definition);
			if (manifest.manifest_has)
			{
				write_json(fmt::format("{}/resource.json", destination), manifest);
			}
			return;
		}
	};
}