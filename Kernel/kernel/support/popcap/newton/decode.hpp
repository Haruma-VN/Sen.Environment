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

			// buffer reader

			DataStreamView sen;

			/**
			 * read enumeration
			*/

			inline auto read_enumeration(

			) -> uint8_t
			{
				return sen.readUint8();
			}

			/**
			 * read boolean
			*/

			inline auto read_boolean(

			) -> bool
			{
				auto value = bool{};
				switch(sen.readUint8()){
					case 0x01: {
						value = true;
						break;
					}
					case 0x00:{
						value = false;
						break;
					}
					default:{
						throw Exception(fmt::format("Invalid boolean value at offset: {}", sen.get_read_pos() - 1));
					}
				}
				return value;
			}

			/**
			 * read integer
			*/

			inline auto read_integer(

			) -> int
			{
				return sen.readInt32();
			}

			/**
			 * read string
			*/

			inline auto read_string(

			) -> std::string
			{
				return sen.readString(static_cast<size_t>(sen.readUint32()));
			}

		public:

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
				result["slot_count"] = thiz.read_integer();
				auto group_size = thiz.read_integer();
				for(auto i : Range<int>(group_size)){
					auto group = nlohmann::ordered_json{};
					auto group_type = thiz.read_enumeration();
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
							throw Exception(fmt::format("Unknown group index at index {}", i));
						}
					}
					auto res = thiz.read_integer();
					if (res != 0x00) {
						group["res"] = fmt::format("{}", res);
					}
					auto subgroups_count = thiz.read_integer();
					auto resources_count = thiz.read_integer();
					auto version = thiz.read_enumeration();
					try_assert(version == 0x01, fmt::format("Unknown version {} at index {}", version, i));
      				auto group_has_parent = thiz.read_boolean();
					group["id"] = thiz.read_string();
					if (group_has_parent) {
						group["parent"] = thiz.read_string();
					}
					if (group_type == 0x01) {
						try_assert(resources_count == 0x00, "Property \"resources\" must have size 0 with composite");
						auto subgroups = nlohmann::ordered_json::array_t{};
						for (auto subgroups_index : Range<int>(subgroups_count)) {
							auto subgroup = nlohmann::ordered_json{};
							auto sub_res = thiz.read_integer();
							if (sub_res != 0x00) {
								subgroup["res"] = fmt::format("{}", sub_res);
							}
							subgroup["id"] = thiz.read_string();
							subgroups.push_back(subgroup);
						}
						group["subgroups"] = subgroups;
						groups.push_back(group);
					}
					if(group_type == 0x02){
						try_assert(subgroups_count == 0x00, "Property \"subgroup\" must have size 0 with simple");
						auto resources = nlohmann::ordered_json::array_t{};
						for (auto resources_index : Range<int>(resources_count)){
          					auto sub_resources = nlohmann::ordered_json{};
							auto resource_type = thiz.read_enumeration();
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
									throw Exception(fmt::format("resource type cannot be specified at {}", group["id"].get<std::string>()));
								}
							}
							auto slot = thiz.read_integer();
							auto width = thiz.read_integer();
							auto height = thiz.read_integer();
							auto x = thiz.read_integer();
							auto y = thiz.read_integer();
							auto ax = thiz.read_integer();
							auto ay = thiz.read_integer();
							auto aw = thiz.read_integer();
							auto ah = thiz.read_integer();
							auto cols = thiz.read_integer();
							auto rows = thiz.read_integer();
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
							thiz.read_enumeration();
							thiz.read_enumeration();
							auto resource_has_parent = thiz.read_boolean();
							sub_resources["id"] = thiz.read_string();
							sub_resources["path"] = thiz.read_string();
							if (resource_has_parent) {
								sub_resources["parent"] = thiz.read_string();
							}
							switch (static_cast<int>(resource_type)) {
								case 0x02: {
									sub_resources["forceOriginalVectorSymbolSize"] = true;
									break;
								}
								case 0x06: {
									sub_resources["srcpath"] = fmt::format("res\\common\\{}", sub_resources["path"].get<std::string>());
									break;
								}
							}
							resources.push_back(sub_resources);
						}
        				group["resources"] = resources;
						groups.push_back(group);
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
				const std::string & source
			) : sen(source)
			{

			}

			// constructor

			explicit Decode(
				const String & source
			) : sen(source.value)
			{

			}

			// constructor

			explicit Decode(
				DataStreamView & it
			) : sen(it)
			{

			}

			// destructor

			~Decode(

			) = default;

			/**
			 * This method will decode newton file
			 * source: source file
			 * destination: destination file
			 * return: the decoded newton file
			*/

			inline static auto process_fs (
				const std::string & source,
				const std::string & destination
			) -> void
			{
				auto decode = Decode{source};
				auto result = decode.process();
				FileSystem::write_json(destination, result);
				return;
			}
	};
}