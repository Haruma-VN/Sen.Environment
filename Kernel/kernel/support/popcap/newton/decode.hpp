#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Newton {

	/**
	 * use namespace definition
	*/

	using namespace Sen::Kernel::Definition;

	/**
	 * decode 
	*/

	struct Decode {

		private:

			/**
			 * read enumeration
			*/
			template <auto use_uint>
			inline auto read_enumeration(

			)
			{
				if constexpr (use_uint) {
					return sen->readUint8();
				}
				else {
					return sen->readInt8();
				}
			}

			/**
			 * read boolean
			*/

			inline auto read_boolean(

			) -> bool
			{
				auto value = bool{};
				switch(sen->readUint8()){
					case 0x01: {
						value = true;
						break;
					}
					case 0x00:{
						value = false;
						break;
					}
					default:{
						throw Exception(fmt::format("{} {}", Language::get("popcap.newton.invalid_boolean_value") , sen->get_read_pos() - 1), std::source_location::current(), "read_boolean");
					}
				}
				return value;
			}

			/**
			 * read integer
			*/
			template <auto use_uint>
			inline auto read_integer(

			)
			{
				if constexpr (use_uint) {
					return sen->readUint32();
				}
				else {
					return sen->readInt32();
				}
			}

			/**
			 * read string
			*/

			inline auto read_string(

			) -> std::string
			{
				return sen->readString(static_cast<size_t>(sen->readUint32()));
			}

		public:

			// buffer reader

			std::unique_ptr<DataStreamView> sen;

			/**
			 * Process method
			*/

			inline auto process(

			) -> nlohmann::ordered_json
			{
				auto result = nlohmann::ordered_json{
					{"version", 1},
					{"content_version", 1}
				};
				auto groups = nlohmann::ordered_json::array_t{};
				result["slot_count"] = thiz.read_integer<true>();
				auto group_size = thiz.read_integer<true>();
				for(auto i : Range<int>(group_size)){
					auto group = nlohmann::ordered_json{};
					auto group_type = thiz.read_enumeration<true>();
					switch (static_cast<int>(group_type)){
						case 0x01:{
							group["type"] = "composite";
							break;
						}
						case 0x02:{
							group["type"] = "simple";
							break;
						}
						default:{
							throw Exception(fmt::format("{} {}. {}: group[\"type\"] == 1 || group[\"type\"] == 2, {} {}", Language::get("popcap.newton.invalid_group_type"), i, Language::get("conditional"), Language::get("but_received"), group_type), std::source_location::current(), "process");
						}
					}
					auto res = thiz.read_integer<true>();
					if (res != 0x00) {
						group["res"] = fmt::format("{}", res);
					}
					auto subgroups_count = thiz.read_integer<true>();
					auto resources_count = thiz.read_integer<false>();
					auto version = thiz.read_enumeration<true>();
					assert_conditional(version == 0x01, fmt::format("{} {} {} {}", Kernel::Language::get("popcap.newton.decode.unknown_version"), version, Kernel::Language::get("popcap.newton.decode.at_index"), i), "process");
      				auto group_has_parent = thiz.read_boolean();
					group["id"] = thiz.read_string();
					if (group_has_parent) {
						group["parent"] = thiz.read_string();
					}
					if (group_type == 0x01) {
						assert_conditional(resources_count == 0x00, fmt::format("{}, id: {}", Kernel::Language::get("popcap.newton.decode.resource_must_be_null_with_composite"), group["id"]), "process");
						auto subgroups = nlohmann::ordered_json::array_t{};
						for (auto subgroups_index : Range<int>(subgroups_count)) {
							auto subgroup = nlohmann::ordered_json{};
							auto sub_res = thiz.read_integer<true>();
							if (sub_res != 0x00) {
								subgroup["res"] = fmt::format("{}", sub_res);
							}
							subgroup["id"] = thiz.read_string();
							subgroups.emplace_back(subgroup);
						}
						group["subgroups"] = subgroups;
						groups.emplace_back(group);
					}
					if(group_type == 0x02){
						assert_conditional(subgroups_count == 0x00, fmt::format("{}, id: {}", Kernel::Language::get("popcap.newton.decode.subgroup_must_be_null_with_simple"), group["id"]), "process");
						auto resources = nlohmann::ordered_json::array_t{};
						for (auto resources_index : Range<int>(resources_count)){
          					auto sub_resources = nlohmann::ordered_json{};
							auto resource_type = thiz.read_enumeration<true>();
							switch(static_cast<int>(resource_type)){
								case 0x01:{
									sub_resources["type"] = "Image";
									break;
								}
								case 0x02:{
									sub_resources["type"] = "PopAnim";
									break;
								}
								case 0x03:{
									sub_resources["type"] = "SoundBank";
									break;
								}
								case 0x04:{
									sub_resources["type"] = "File";
									break;
								}
								case 0x05:{
									sub_resources["type"] = "PrimeFont";
									break;
								}
								case 0x06:{
									sub_resources["type"] = "RenderEffect";
									break;
								}
								case 0x07:{
									sub_resources["type"] = "DecodedSoundBank";
									break;
								}
								default:{
									throw Exception(fmt::format("{} {}, {}", Language::get("popcap.newton.invalid_resource_type"), group["id"].get<std::string>(), Language::get("popcap.newton.expected_from_to"), resource_type), std::source_location::current(), "process");
								}
							}
							auto slot = thiz.read_integer<true>();
							auto width = thiz.read_integer<false>();
							auto height = thiz.read_integer<false>();
							auto x = thiz.read_integer<false>();
							auto y = thiz.read_integer<false>();
							auto ax = thiz.read_integer<false>();
							auto ay = thiz.read_integer<false>();
							auto aw = thiz.read_integer<false>();
							auto ah = thiz.read_integer<false>();
							auto cols = thiz.read_integer<false>();
							auto rows = thiz.read_integer<false>();
							auto atlas = thiz.read_boolean();
          					auto is_sprite = aw != 0x00 and ah != 0x00;
							sub_resources["slot"] = slot;
							if(atlas){
								sub_resources["width"] = width;
								sub_resources["height"] = height;
							}
							if(is_sprite){
								sub_resources["ax"] = ax;
								sub_resources["ay"] = ay;
								sub_resources["aw"] = aw;
								sub_resources["ah"] = ah;
							}
							if(atlas){
								sub_resources["atlas"] = true;
								sub_resources["runtime"] = true;
							}
							if(x != 0x00 and x != 0x7FFFFFFF){
								sub_resources["x"] = x;
							}
							if(y != 0x00 and y != 0x7FFFFFFF){
								sub_resources["y"] = y;
							}
							if(cols != 0x01){
								sub_resources["cols"] = cols;
							}
							if(rows != 0x01){
								sub_resources["rows"] = rows;
							}
							thiz.read_enumeration<true>();
							thiz.read_enumeration<true>();
							auto resource_has_parent = thiz.read_boolean();
							sub_resources["id"] = thiz.read_string();
							sub_resources["path"] = thiz.read_string();
							if (resource_has_parent) {
								sub_resources["parent"] = thiz.read_string();
							}
							resources.emplace_back(sub_resources);
						}
        				group["resources"] = resources;
						groups.emplace_back(group);
					}
				}
				result["groups"] = groups;
				return result;
			}

			// constructor

			Decode(

			) = default;

			// constructor

			explicit Decode(
				std::string_view source
			) : sen(std::make_unique<DataStreamView>(source))
			{

			}

			// constructor

			explicit Decode(
				DataStreamView & it
			) : sen(&it)
			{

			}

			// destructor

			~Decode(

			) = default;

			/**
			 * Instance
			*/

			M_INSTANCE_OF_STRUCT(
				Decode
			);

			/**
			 * This method will decode newton file
			 * source: source file
			 * destination: destination file
			 * return: the decoded newton file
			*/

			inline static auto process_fs (
				std::string_view source,
				std::string_view destination
			) -> void
			{
				auto view = Decode{source};
				auto result = view.process();
				FileSystem::write_json(destination, result);
				return;
			}
	};
}