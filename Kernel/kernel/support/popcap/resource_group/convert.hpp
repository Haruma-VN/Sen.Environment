#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceGroup { 

	// Path style

	enum PathStyle
	{

		// old path style

		ArrayStyle,

		// new path style

		WindowStyle,

	};

	/**
	 * Common class
	*/

	class Common {

		public:
			
			// x and y

			inline static auto constexpr DefaultCoordinateOffset = 0;

			// cols and rows

			inline static auto constexpr DefaultLayoutOffset = 1;

			// New path style

			inline static auto const WindowStyle = std::string{"\\"};

			// Array

			inline static auto const Array = std::string{"array"};
			
			// String

			inline static auto const String = std::string{"string"};
	};

	/**
	 * Convert class
	*/

	class Convert : Common {

		private:

			// use windows path : new style

			bool use_string_for_style;

			/**
			 * This function will convert atlas
			 * subgroup: the subgroup json object
			 * return: the newly converted json
			*/

			auto convert_atlas(
				const nlohmann::json & subgroup
			) -> nlohmann::json
			{
				auto result = nlohmann::json {
					{"type", subgroup["res"]}
				};
				auto atlas = std::vector<nlohmann::json>{};
				for(auto & element : subgroup["resources"]){
					if(element.find("atlas") != element.end() && element["atlas"]){
						atlas.push_back(element);
					}
				}
				for(auto & parent : atlas) {
					auto atlas_data = nlohmann::json {
						{"type", parent["type"]},
						{"path", thiz.use_string_for_style ? String::split(parent["path"], Common::WindowStyle) : parent["path"].get<std::vector<std::string>>() },
						{"dimension", nlohmann::json {
							{"width", parent["width"] },
							{"height", parent["height"] }
						}}
					};
					auto children_in_current_parent = std::vector<nlohmann::json>{};
					for(auto & element : subgroup["resources"]) {
						if(element["parent"] == parent["id"]) {
							children_in_current_parent.push_back(element);
						}
					}
					for(auto & element : children_in_current_parent) {
						auto children_data = nlohmann::json {
							{"type", element["type"]},
							{"path", thiz.use_string_for_style ? String::split(element["path"], Common::WindowStyle) : element["path"].get<std::vector<std::string>>() },
							{
								"default", nlohmann::json {
									{"ax", element["ax"]},
									{"ay", element["ay"]},
									{"aw", element["aw"]},
									{"ah", element["ah"]}
							}}
						};
						if(element.find("x") != element.end() and element["x"] != Common::DefaultCoordinateOffset){
							children_data["default"]["x"] = element["x"];
						}
						else{
							children_data["default"]["x"] = Common::DefaultCoordinateOffset;
						}
						if(element.find("y") != element.end() and element["y"] != Common::DefaultCoordinateOffset){
							children_data["default"]["y"] = element["y"];
						}
						else{
							children_data["default"]["y"] = Common::DefaultCoordinateOffset;
						}
						if(element.find("rows") != element.end() and element["rows"] != Common::DefaultLayoutOffset){
							children_data["default"]["rows"] = element["rows"];
						}
						if(element.find("cols") != element.end() and element["cols"] != Common::DefaultLayoutOffset){
							children_data["default"]["cols"] = element["cols"];
						}
						atlas_data["data"][element["id"]] = children_data;
					}
					result["packet"][parent["id"]] = atlas_data;
				}
				return result;
			}

			/**
			 * This function will convert common data to json map
			 * subgroup: subgroup data
			 * return: map
			*/

			auto convert_common(
				const nlohmann::json & subgroup
			) -> nlohmann::json
			{
				auto result = nlohmann::json {
					{"type", nullptr},
					{"packet", nlohmann::json{
						{"type", "File"}
					}}
				};
				auto data = nlohmann::json{};
				for(auto & element : subgroup["resources"]) {
					auto data_s = nlohmann::json {
						{"type", element["type"]},
						{"path", thiz.use_string_for_style ? String::split(element["path"], Common::WindowStyle) : element["path"].get<std::vector<std::string>>() }
					};
					if(element.find("forceOriginalVectorSymbolSize") != element.end()) {
						data_s["forceOriginalVectorSymbolSize"] = element["forceOriginalVectorSymbolSize"];
					}
					if(element.find("srcpath") != element.end()) {
						data_s["srcpath"] = thiz.use_string_for_style ? String::split(element["srcpath"], Common::WindowStyle) : element["srcpath"].get<std::vector<std::string>>();
					}
					data[element["id"]] = data_s;
				}
				result["packet"]["data"] = data;
				return result;
			}

			/**
			 * This function will find the first subgroup in the resource group
			 * resource_group: resource group
			 * id: the parent id
			 * return: the subgroup
			*/

			auto first_where(
				const nlohmann::json & resource_group,
				const std::string & id
			) -> nlohmann::json
			{
				for(auto & element : resource_group["groups"]){
					if(element["id"] == id) {
						return element;
					}
				}
				throw runtime_error(fmt::format("Cannot find id {} in resource group", id));
			}

		
		public:

			/**
			 * This function will convert whole resource group to res info
			 * source: resource group after deserialized
			 * return: res info
			*/

			auto convert_whole(
				const nlohmann::json & resource_group
			) -> nlohmann::json
			{
				try_assert(resource_group.find("groups") != resource_group.end(), fmt::format("\"{}\" cannot be null in resource group", "groups"));
				auto result = nlohmann::json {
					{"expand_path", thiz.use_string_for_style ? Common::String : Common::Array}
				};
				for(auto & element : resource_group["groups"]) {
					if(element.find("subgroups") != element.end()){
						auto subgroup = nlohmann::json {
							{"is_composite", true}
						};
						for(auto & k : element["subgroups"]) {
							if (k.find("res") != k.end() and k["res"].get<std::string>() != "0") {
								subgroup["subgroup"][k["id"]] = thiz.convert_atlas(thiz.first_where(resource_group, k["id"]));
							}
							else {
								subgroup["subgroup"][k["id"]] = thiz.convert_common(thiz.first_where(resource_group, k["id"]));
							}
						}
						result["groups"][element["id"]] = subgroup;
					}
					if(element.find("parent") == element.end() && element.find("resources") != element.end()) {
						auto subgroup = nlohmann::json {
							{"is_composite", false}
						};
						subgroup["subgroup"][element["id"]] = thiz.convert_common(element);
						result["groups"][element["id"]] = subgroup;
					}
					
				}
				return result;
			}

			// convert method

			/**
			 * This is a quick method to call to convert resource group to res info
			 * @param source: source file
			 * @param destination: destination file
			 * @param style: resource path style
			 * @return: Resource group in source converted to Res-Info in destination
			*/

			static auto convert_fs(
				const std::string & source,
				const std::string & destination,
				PathStyle style
			) -> void 
			{
				auto *convert_c = new ResourceGroup::Convert{style};
				FileSystem::writeJson(destination, convert_c->convert_whole(FileSystem::readJson(source)));
				delete convert_c;
				convert_c = nullptr;
				return;
			}

			// default constructor

			Convert(
				PathStyle style
			) : use_string_for_style(style == PathStyle::WindowStyle)
			{

			}

			// default destructor

			~Convert(

			) = default;
	};
}