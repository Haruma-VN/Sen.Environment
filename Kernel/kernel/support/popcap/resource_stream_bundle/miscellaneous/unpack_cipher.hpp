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
		inline static auto process_package(
			DataStreamView &stream,
			typename BundleStructure &definition,
			typename ManifestStructure &manifest,
			std::string_view destination) -> void
		{
			auto packet_data_section_view_stored = std::map<std::string, std::vector<uint8_t>>{};
			assert_conditional(stream.readUint32() == Common::k_magic_identifier, "invalid_magic_header", "process_package");
			auto version = stream.readUint32();
			auto index = std::find(Common::k_version_list.begin(), Common::k_version_list.end(), static_cast<int>(version));
			assert_conditional((index != Common::k_version_list.end()), "invalid_rsg_version", "process"); // TODO: add to localization.
			definition.version = version;
			auto information_structure = Common::Information{};
			Common::exchange_to_header(stream, information_structure.header);
			// CompiledMapData::decode(stream, information_structure.header.group_id_section_offset, information_structure.header.group_id_section_size, information_structure.group_id, &Common::exchange_to_index);
			CompiledMapData::decode(stream, information_structure.header.subgroup_id_section_offset, information_structure.header.subgroup_id_section_size, information_structure.subgroup_id, &Common::exchange_to_index);
			// CompiledMapData::decode(stream, information_structure.header.resource_path_section_offset, information_structure.header.resource_path_section_size, information_structure.resource_path, &Common::exchange_to_index);
			stream.read_pos = information_structure.header.group_information_section_offset;
			exchange_list(stream, information_structure.group_information, &Common::exchange_to_simple_group, static_cast<size_t>(information_structure.header.group_information_section_block_count));
			stream.read_pos = information_structure.header.subgroup_information_section_offset;
			if (definition.version == 1_ui)
            {
                exchange_list(stream, information_structure.subgroup_information, &Common::exchange_to_basic_subgroup<1_ui>, static_cast<size_t>(information_structure.header.subgroup_information_section_block_count));
            }
            else
            {
                exchange_list(stream, information_structure.subgroup_information, &Common::exchange_to_basic_subgroup<3_ui>, static_cast<size_t>(information_structure.header.subgroup_information_section_block_count));
            }
			// stream.read_pos = information_structure.header.pool_information_section_offset;
			// exchange_list(stream, information_structure.pool_information, &Common::exchange_to_pool, static_cast<size_t>(information_structure.header.pool_information_section_block_count));
			stream.read_pos = information_structure.header.texture_resource_information_section_offset;
			auto &texture_resource_information_section_block_size = information_structure.header.texture_resource_information_section_block_size;
			switch (texture_resource_information_section_block_size)
			{
			case Common::k_texture_resource_information_section_block_size_version_0:
			{
				exchange_list(stream, information_structure.texture_resource_information, &Common::exchange_to_texture<Common::k_texture_resource_information_section_block_size_version_0>, static_cast<size_t>(information_structure.header.texture_resource_information_section_block_count));
				break;
			}
			case Common::k_texture_resource_information_section_block_size_version_1:
			{
				exchange_list(stream, information_structure.texture_resource_information, &Common::exchange_to_texture<Common::k_texture_resource_information_section_block_size_version_1>, static_cast<size_t>(information_structure.header.texture_resource_information_section_block_count));
				break;
			}
			case Common::k_texture_resource_information_section_block_size_version_2:
			{
				exchange_list(stream, information_structure.texture_resource_information, &Common::exchange_to_texture<Common::k_texture_resource_information_section_block_size_version_2>, static_cast<size_t>(information_structure.header.texture_resource_information_section_block_count));
				break;
			}
			default:
				assert_conditional(false, "invalid_texture_resource_information_section_block_size", "process_package"); // TODO: add to localization.
			}
			if (information_structure.header.group_manifest_information_section_offset != 0_ui || information_structure.header.resource_manifest_information_section_offset != 0_ui || information_structure.header.string_manifest_information_section_offset != 0_ui)
			{
				assert(information_structure.header.group_manifest_information_section_offset != 0_ui && information_structure.header.resource_manifest_information_section_offset != 0_ui && information_structure.header.string_manifest_information_section_offset != 0_ui);
				Unpack::process_package_manifest(stream, information_structure.header, manifest);
			}
			definition.texture_information_section_size = texture_resource_information_section_block_size;
			for (auto &[group_id, group_index] : information_structure.group_id)
            {
                auto &simple_group_information = information_structure.group_information.at(group_index);
				auto group_information = GroupInformation{};
				auto original_id = toupper_back(Unpack::make_original_group_id(simple_group_information.id, group_information.composite));
				for (auto subgroup_index : Range(simple_group_information.subgroup_count))
				{
					auto &simple_subgroup_infomation = simple_group_information.subgroup_information.at(subgroup_index);
					auto &basic_subgroup_information = information_structure.subgroup_information.at(simple_subgroup_infomation.index);
					auto result = std::find_if(information_structure.subgroup_id.begin(), information_structure.subgroup_id.end(), [subgroup_index](const auto &element) { return element.second == subgroup_index; });
					auto &subgroup_id = (*result).first;
					auto subgroup_information = SubgroupInformation{};
					subgroup_information.category.resolution = simple_subgroup_infomation.resolution;
					if (version >= 3_ui && simple_subgroup_infomation.locale != 0_ui)
					{
						Common::fourcc_from_integer(simple_subgroup_infomation.locale, subgroup_information.category.locale);
					}
					basic_subgroup_information.resource_data_section_compression = subgroup_information.compression;
					auto texture_resource_begin = basic_subgroup_information.texture_resource_begin;
					auto texture_resource_count = basic_subgroup_information.texture_resource_count;
					auto packet_data = stream.getBytes(basic_subgroup_information.offset, basic_subgroup_information.offset + basic_subgroup_information.size);
					auto packet_stream = DataStreamView{packet_data};
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
						ResourceStreamGroup::Common::exchange_header(information_structure_header, packet_stream);
					}
					auto packet_structure = PacketStructure{};
					ResourceStreamGroup::Unpack::process_whole(packet_stream, packet_structure, destination);
					for (auto &packet_resource : packet_structure.resource)
					{
						if (packet_resource.use_texture_additional_instead)
						{
							auto &texture_information_structure = information_structure.texture_resource_information[static_cast<size_t>(texture_resource_begin + packet_resource.texture_additional.value.index)];
							packet_resource.texture_additional.value.texture_resource_information_section_block_size = texture_resource_information_section_block_size;
							assert_test(packet_resource.texture_additional.value.dimension.width == texture_information_structure.size_width);
							assert_test(packet_resource.texture_additional.value.dimension.height == texture_information_structure.size_height);
							packet_resource.texture_additional.value.texture_infomation = TextureResourceInformation{
								.pitch = texture_information_structure.pitch,
								.format = texture_information_structure.format,
								.alpha_size = texture_information_structure.alpha_size,
								.scale = texture_information_structure.scale};
						}
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
			typename BundleStructure &definition,
			typename ManifestStructure &manifest,
			std::string_view destination) -> void
		{
			process_package(stream, definition, manifest, destination);
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
			write_json(fmt::format("{}/definition.json", destination), definition);
			if (manifest.manifest_has)
            {
                write_json(fmt::format("{}/manifest.json", destination), manifest);
            }
            return;
		}
	};
}