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

			std::map<std::string, unsigned char> ResourceTypeEnumeration = std::map<std::string, unsigned char> {
				{"Image", 0x01},
				{"PopAnim", 0x02},
				{"SoundBank", 0x03},
				{"File", 0x04},
				{"PrimeFont", 0x05},
				{"RenderEffect", 0x06},
				{"DecodedSoundBank", 0x07}
			};

			/***
			 * Newton JSON
			*/

			nlohmann::json newton_resource;

			/**
			 * writer: sen writer
			 * str: the string
			*/ 

			#define write_string(writer, str)    \
				writer.writeInt32LE(static_cast<int32_t>(str.size())); \
				writer.writeString(str);

			/**
			 * writer: sen writer
			 * condition: boolean condition
			*/

			#define write_boolean(writer, condition) \
			if (condition)                       \
			{                                    \
				writer.writeUint8(0x01); \
			} \
			else{ \
				writer.writeUint8(0x00); \
			}

	public :

		// constructor

		Encode(

			) = default;

	// destructor

	~Encode(

		) = default;

	// constructor

	explicit Encode(
		const std::string &source) : newton_resource(FileSystem::readJson(source))
	{
	}

	// constructor

	explicit Encode(
		const String &source) : newton_resource(FileSystem::readJson(source.value))
	{
	}

	/**
	 * process
	 */

	auto process(

		) -> Buffer::Vector
	{
		auto sen = Buffer::Vector{};
		try_assert(thiz.newton_resource.find("slot_count") != thiz.newton_resource.end(), "slot_count cannot be null");
		sen.writeInt32LE(thiz.newton_resource["slot_count"].get<int32_t>());
		try_assert(thiz.newton_resource.find("groups") != thiz.newton_resource.end(), "groups cannot be null");
		try_assert(thiz.newton_resource["groups"].is_array(), "groups must be an array");
		auto groups_count = static_cast<int32_t>(thiz.newton_resource["groups"].get<std::vector<nlohmann::json>>().size());
		sen.writeInt32LE(groups_count);
		for (auto group_index : Range<int>(groups_count))
		{
			auto m_data = thiz.newton_resource["groups"].get<std::vector<nlohmann::json>>()[group_index].get<nlohmann::json>();
			if (m_data["type"].get<std::string>() == "composite")
			{
				sen.writeUint8(static_cast<uint8_t>(0x01));
			}
			else if (m_data["type"].get<std::string>() == "simple")
			{
				sen.writeUint8(static_cast<uint8_t>(0x02));
			}
			else
			{
				throw std::runtime_error(fmt::format("Unknown type {} at id {}", m_data["type"].get<std::string>(), m_data["id"].get<std::string>()));
			}
					#define is_defined_or(property, or_value) \
						m_data.find(property) != m_data.end() ? static_cast<int>(m_data[property].size()) : or_value
					auto subgroups_count = is_defined_or("subgroups", 0x00);
					auto resources_count = is_defined_or("resources", 0x00);
					sen.writeInt32LE(m_data.find("res") != m_data.end() ? std::stoi(m_data["res"].get<std::string>()) : 0x00);
					sen.writeInt32LE(subgroups_count);
					sen.writeInt32LE(resources_count);
					sen.writeUint8(0x01);
					if (m_data.find("parent") != m_data.end()) {
						sen.writeUint8(0x01);
					} 
					else {
						sen.writeUint8(0x00);
					}
					write_string(sen, m_data["id"].get<std::string>());
					if (m_data.find("parent") != m_data.end()) {
						write_string(sen, m_data["parent"].get<std::string>());
					}
					if (m_data["type"].get<std::string>() == "composite") {
						try_assert(m_data.find("resources") != m_data.end(), fmt::format("resources must be null at id {}", m_data["id"].get<std::string>()));
						for (auto i : Range<int>(subgroups_count)) {
							auto & current = m_data["subgroups"][i];
							if (current.find("res") != current.end()) {
								sen.writeInt32LE(std::stoi(current["res"].get<std::string>()));
							} 
							else {
								sen.writeInt32LE(0x00);
							}
							write_string(sen, current["id"].get<std::string>());
						}
					}
					if (m_data["type"].get<std::string>() == "simple"){
						for(auto resources_index : Range<int>(resources_count)){
          					auto resource_x = m_data["resources"].get<std::vector<nlohmann::json>>()[resources_index].get<nlohmann::json>();
							try_assert(ResourceTypeEnumeration.find(resource_x["type"].get<std::string>()) != ResourceTypeEnumeration.end(), fmt::format("Unsigned resources type enumeration: {}", resource_x["type"].get<std::string>()));
							sen.writeUint8(ResourceTypeEnumeration[resource_x["type"].get<std::string>()]);
							sen.writeInt32LE(resource_x["slot"].get<int>());
							#define m_data_defined_or(property, value)                      \
							if (resource_x.find(property) != resource_x.end())          \
							{                                                           \
								sen.writeInt32LE(resource_x[property].get<int>()); \
							}                                                           \
							else                                                        \
							{                                                           \
								sen.writeInt32LE(value);                           \
							}
							m_data_defined_or("width", 0x00);
							m_data_defined_or("height", 0x00);
							auto is_sprite = (is_not_null_object(resource_x, "aw") and resource_x["aw"] != 0x00) and (is_not_null_object(resource_x, "ah") and resource_x["ah"] != 0x00);
							if(is_null_object(resource_x, "x")){
								if(is_sprite){
									sen.writeInt32LE(0x00);
								}
								else{
									sen.writeInt32LE(0x7FFFFFFF);
								}
							}
							else{
								sen.writeInt32LE(resource_x["x"].get<int>());
							}
							if(is_null_object(resource_x, "y")){
								if(is_sprite){
									sen.writeInt32LE(0x00);
								}
								else{
									sen.writeInt32LE(0x7FFFFFFF);
								}
							}
							else{
								sen.writeInt32LE(resource_x["y"].get<int>());
							}
							#define m_writer_defined_or(property, value) \
							if(is_null_object(resource_x, property)){\
								sen.writeInt32LE(value);\
							} \
							else{ \
								sen.writeInt32LE(resource_x["property"].get<int>());\
							}
							m_writer_defined_or("ax", 0x00);
							m_writer_defined_or("ay", 0x00);
							m_writer_defined_or("aw", 0x00);
							m_writer_defined_or("ah", 0x00);
							m_writer_defined_or("cols", 0x01);
							m_writer_defined_or("rows", 0x01);
							write_boolean(sen, is_not_null_object(resource_x, "atlas") and resource_x["atlas"].get<bool>());
							sen.writeUint8(0x01);
							sen.writeUint8(0x01);
							auto resource_has_parent = is_not_null_object(resource_x, "parent");
							write_boolean(sen, resource_has_parent);
							write_string(sen, resource_x["id"].get<std::string>());
							write_string(sen, resource_x["path"].get<std::string>());
							if(resource_has_parent){
								write_string(sen, resource_x["parent"].get<std::string>());
							}
						}
					}
				}
				return sen;
			}

			static auto process_fs(
				const std::string & source,
				const std::string & destination
			) -> void
			{
				auto encoder = Encode{source};
				auto sen = encoder.process();
				sen.outFile(destination);
				return;
			}

	};
}