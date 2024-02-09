#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Newton {

	/**
	 * use namespace definition
	*/

	using namespace Sen::Kernel::Definition; 

	/**
	 * Encode Newton class
	*/

	struct Encode {

		private:

			/**
			 * Resource Enumeration
			*/

			inline static std::map<std::string, unsigned char> const ResourceTypeEnumeration = std::map<std::string, unsigned char> {
				{"Image", 0x01_byte},
				{"PopAnim", 0x02_byte},
				{"SoundBank", 0x03_byte},
				{"File", 0x04_byte},
				{"PrimeFont", 0x05_byte},
				{"RenderEffect", 0x06_byte},
				{"DecodedSoundBank", 0x07_byte}
			};

			/***
			 * Newton JSON
			*/

			nlohmann::ordered_json resource;

	public :

			std::unique_ptr<DataStreamView> sen;

			// constructor

			explicit Encode(

			) : sen(std::make_unique<DataStreamView>())
			{

			}

			// destructor

			~Encode(

			) = default;

			// constructor

			explicit Encode(
				std::string_view source
			) : resource(FileSystem::read_json(source)), sen(std::make_unique<DataStreamView>())
			{
			}

			/**
			 * ---------------------
			 * Process method
			 * ---------------------
			 */

			inline auto process(

			) -> void
			{
				assert_conditional(is_not_null_object(resource, "slot_count"), fmt::format("{}", Kernel::Language::get("popcap.newton.encode.slot_count_cannot_be_null")), "process");
				sen->writeInt32(resource["slot_count"].get<int>());
				sen->writeInt32(static_cast<int>(resource["groups"].size()));
				for(auto & m_data : thiz.resource["groups"]){
					if(m_data["type"] == "composite"){
						sen->writeUint8(0x01);
					}
					else if(m_data["type"] == "simple"){
						sen->writeUint8(0x02);
					}
					else{
						throw Exception(fmt::format("{} {} {} {}", Kernel::Language::get("popcap.newton.encode.unknown_type"), m_data["type"], Kernel::Language::get("popcap.newton.encode.at_group_id"), m_data["id"]), std::source_location::current(), "process");
					}
					auto subgroups_count = is_null_object(m_data, "subgroups") ? 0x00 : m_data["subgroups"].size();
      				auto resources_count = is_null_object(m_data, "resources") ? 0x00 : m_data["resources"].size();
					sen->writeInt32(is_null_object(m_data, "res") ? 0x00 : std::stoi(m_data["res"].get<std::string>()));
					sen->writeInt32(subgroups_count);
					sen->writeInt32(resources_count);
					sen->writeUint8(0x01);
					if (is_not_null_object(m_data, "parent")) {
						sen->writeUint8(0x01);
					} 
					else {
						sen->writeUint8(0x00);
					}
					sen->writeUint32(m_data["id"].get<std::string>().size());
					sen->writeString(m_data["id"]);
					if (is_not_null_object(m_data, "parent")) {
						sen->writeUint32(m_data["parent"].get<std::string>().size());
						sen->writeString(m_data["parent"]);
					}
					if(m_data["type"] == "composite"){
						assert_conditional(is_null_object(m_data, "resources"), fmt::format("{}", Kernel::Language::get("popcap.newton.encode.resource_must_be_null_with_composite")), "process");
						for(auto & current : m_data["subgroups"]){
							if(is_not_null_object(current, "res")){
								sen->writeInt32(std::stoi(current["res"].get<std::string>()));
							}
							else{
								sen->writeInt32(0x00);
							}
							sen->writeUint32(current["id"].get<std::string>().size());
							sen->writeString(current["id"]);
						}
					}
					if(m_data["type"] == "simple"){
						assert_conditional(is_null_object(m_data, "subgroups"), fmt::format("{}", Kernel::Language::get("popcap.newton.encode.subgroup_must_be_null_with_simple")), "process");
						for(auto & resource_x : m_data["resources"]){
							auto m_type = resource_x["type"].get<std::string>();
							if(m_type == "Image"){
								sen->writeUint8(0x01);
							}
							else if(m_type == "PopAnim"){
								sen->writeUint8(0x02);
							}
							else if(m_type == "SoundBank"){
								sen->writeUint8(0x03);
							}
							else if(m_type == "File"){
								sen->writeUint8(0x04);
							}
							else if(m_type == "PrimeFont"){
								sen->writeUint8(0x05);
							}
							else if(m_type == "RenderEffect"){
								sen->writeUint8(0x06);
							}
							else if(m_type == "DecodedSoundBank"){
								sen->writeUint8(0x07);
							}
							else {
								throw Exception(fmt::format("{} {} {}", Kernel::Language::get("popcap.newton.encode.invalid_type"), Kernel::Language::get("popcap.newton.encode.at_group_id"), resource_x["id"]), std::source_location::current(), "process");
							}
							sen->writeInt32(resource_x["slot"]);
							if (is_null_object(resource_x, "width")) {
								sen->writeInt32(0x00);
							} 
							else {
								sen->writeInt32(resource_x["width"]);
							}
							if (is_null_object(resource_x, "height")) {
								sen->writeInt32(0x00);
							} 
							else {
								sen->writeInt32(resource_x["height"]);
							}
							auto is_sprite = is_not_null_object(resource_x, "aw") and resource_x["aw"] != 0 and is_not_null_object(resource_x, "ah") and resource_x["ah"] != 0;
							if (is_null_object(resource_x, "x")) {
								if (is_sprite) {
									sen->writeInt32(0x00);
								} 
								else {
									sen->writeInt32(0x7FFFFFFF);
								}
							} 
							else {
								sen->writeInt32(resource_x["x"]);
							}
							if (is_null_object(resource_x, "y")) {
								if (is_sprite) {
									sen->writeInt32(0x00);
								} 
								else {
									sen->writeInt32(0x7FFFFFFF);
								}
							} 
							else {
								sen->writeInt32(resource_x["y"]);
							}
							if (is_null_object(resource_x, "ax")) {
								sen->writeInt32(0x00);
							} 
							else {
								sen->writeInt32(resource_x["ax"]);
							}
							if (is_null_object(resource_x, "ay")) {
								sen->writeInt32(0x00);
							} 
							else {
								sen->writeInt32(resource_x["ay"]);
							}
							if (is_null_object(resource_x, "aw")) {
								sen->writeInt32(0x00);
							} 
							else {
								sen->writeInt32(resource_x["aw"]);
							}
							if (is_null_object(resource_x, "ah")) {
								sen->writeInt32(0x00);
							} 
							else {
								sen->writeInt32(resource_x["ah"]);
							}
							if (is_null_object(resource_x, "cols")) {
								sen->writeInt32(0x01);
							} 
							else {
								sen->writeInt32(resource_x["cols"]);
							}
							if (is_null_object(resource_x, "rows")) {
								sen->writeInt32(0x01);
							} 
							else {
								sen->writeInt32(resource_x["rows"]);
							}
							if (is_not_null_object(resource_x, "atlas") and resource_x["atlas"]) {
								sen->writeUint8(0x01);
							} 
							else {
								sen->writeUint8(0x00);
							}
							sen->writeUint8(0x01);
							sen->writeUint8(0x01);
							auto resource_has_parent = is_not_null_object(resource_x, "parent");
							if (resource_has_parent) {
								sen->writeUint8(0x01);
							} 
							else {
								sen->writeUint8(0x00);
							}
							sen->writeUint32(resource_x["id"].get<std::string>().size());
							sen->writeString(resource_x["id"]);
							sen->writeUint32(resource_x["path"].get<std::string>().size());
							sen->writeString(resource_x["path"]);
							if (resource_has_parent) {
								sen->writeUint32(resource_x["parent"].get<std::string>().size());
								sen->writeString(resource_x["parent"]);
							}
						}
					}
				}
				return;
			}

			/**
			 * Quick method for process file
			 * @param source: source file
			 * @param destination: destination file
			 * @returns: the output file after encoded
			*/

			inline static auto process_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				auto encode = Encode{ source };
				encode.process();
				encode.sen->out_file(destination);
				return;
			}

	};
}