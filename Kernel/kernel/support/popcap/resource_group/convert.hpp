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

			// constructor

			Common(

			) = default;

			// destructor

			~Common(

			) = default;
			
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

			// empty string

			inline static auto const emptyString = std::string{""};

			// empty type

			inline static auto const emptyType = std::string{"0"};
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

			inline auto convert_atlas(
				const nlohmann::ordered_json & subgroup
			) -> nlohmann::ordered_json
			{
				auto result = nlohmann::ordered_json {
					{"type", subgroup["res"]}
				};
				auto atlas = std::vector<nlohmann::ordered_json>{};
				for(auto & element : subgroup["resources"]){
					if(element.find("atlas") != element.end() && element["atlas"].get<bool>()){
						atlas.emplace_back(element);
					}
				}
				for(auto & parent : atlas) {
					auto atlas_data = nlohmann::ordered_json {
						{"type", parent["type"].get<std::string>()},
						{"path", thiz.use_string_for_style ? String::split(parent["path"].get<std::string>(), Common::WindowStyle) : parent["path"].get<std::vector<std::string>>() },
						{"dimension", nlohmann::ordered_json {
							{"width", parent["width"].get<int>() },
							{"height", parent["height"].get<int>() }
						}}
					};
					auto children_in_current_parent = std::vector<nlohmann::ordered_json>{};
					for(auto & element : subgroup["resources"]) {
						if(element["parent"].get<std::string>() == parent["id"].get<std::string>()) {
							children_in_current_parent.emplace_back(element);
						}
					}
					for(auto & element : children_in_current_parent) {
						auto children_data = nlohmann::ordered_json {
							{"type", element["type"].get<std::string>()},
							{"path", thiz.use_string_for_style ? String::split(element["path"].get<std::string>(), Common::WindowStyle) : element["path"].get<std::vector<std::string>>() },
							{
								"default", nlohmann::ordered_json {
									{"ax", element["ax"].get<int>()},
									{"ay", element["ay"].get<int>()},
									{"aw", element["aw"].get<int>()},
									{"ah", element["ah"].get<int>()}
							}}
						};
						if(element.find("x") != element.end() and element["x"] != Common::DefaultCoordinateOffset){
							children_data["default"]["x"] = element["x"].get<int>();
						}
						else{
							children_data["default"]["x"] = Common::DefaultCoordinateOffset;
						}
						if(element.find("y") != element.end() and element["y"] != Common::DefaultCoordinateOffset){
							children_data["default"]["y"] = element["y"].get<int>();
						}
						else{
							children_data["default"]["y"] = Common::DefaultCoordinateOffset;
						}
						if(element.find("rows") != element.end() and element["rows"] != Common::DefaultLayoutOffset){
							children_data["default"]["rows"] = element["rows"].get<int>();
						}
						if(element.find("cols") != element.end() and element["cols"] != Common::DefaultLayoutOffset){
							children_data["default"]["cols"] = element["cols"].get<int>();
						}
						atlas_data["data"][element["id"].get<std::string>()] = children_data;
					}
					result["packet"][parent["id"].get<std::string>()] = atlas_data;
				}
				return result;
			}

			/**
			 * This function will convert common data to json map
			 * subgroup: subgroup data
			 * return: map
			*/

			inline auto convert_common(
				const nlohmann::ordered_json & subgroup
			) -> nlohmann::ordered_json
			{
				auto result = nlohmann::ordered_json {
					{"type", nullptr},
					{"packet", nlohmann::ordered_json{
						{"type", "File"}
					}}
				};
				auto data = nlohmann::ordered_json{};
				for(auto & element : subgroup["resources"]) {
					auto data_s = nlohmann::ordered_json {
						{"type", element["type"].get<std::string>()},
						{"path", thiz.use_string_for_style ? String::split(element["path"].get<std::string>(), Common::WindowStyle) : element["path"].get<std::vector<std::string>>() }
					};
					if(element.find("forceOriginalVectorSymbolSize") != element.end()) {
						data_s["forceOriginalVectorSymbolSize"] = element["forceOriginalVectorSymbolSize"].get<bool>();
					}
					if(element.find("srcpath") != element.end()) {
						data_s["srcpath"] = thiz.use_string_for_style ? String::split(element["srcpath"].get<std::string>(), Common::WindowStyle) : element["srcpath"].get<std::vector<std::string>>();
					}
					data[element["id"].get<std::string>()] = data_s;
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

			inline static auto first_where(
				const nlohmann::ordered_json & resource_group,
				const std::string & id
			) -> nlohmann::ordered_json
			{
				for(auto & element : resource_group["groups"]){
					if(element["id"] == id) {
						return element;
					}
				}
				throw Exception(fmt::format("Cannot find id {} in resource group", id), std::source_location::current(), "first_where");
			}

		
		public:

			/**
			 * This function will convert whole resource group to res info
			 * source: resource group after deserialized
			 * return: res info
			*/

			inline auto convert_whole(
				const nlohmann::ordered_json & resource_group
			) -> nlohmann::ordered_json
			{
				assert_conditional(resource_group.find("groups") != resource_group.end(), fmt::format("\"{}\" cannot be null in resource group", "groups"), "convert_whole");
				auto result = nlohmann::ordered_json {
					{"expand_path", thiz.use_string_for_style ? Common::String : Common::Array}
				};
				for(auto & element : resource_group["groups"]) {
					if(element.find("subgroups") != element.end()){
						auto subgroup = nlohmann::ordered_json {
							{"is_composite", true}
						};
						for(auto & k : element["subgroups"]) {
							if (k.find("res") != k.end() and k["res"].get<std::string>() != "0") {
								subgroup["subgroup"][k["id"].get<std::string>()] = thiz.convert_atlas(thiz.first_where(resource_group, k["id"]));
							}
							else {
								subgroup["subgroup"][k["id"].get<std::string>()] = thiz.convert_common(thiz.first_where(resource_group, k["id"]));
							}
						}
						result["groups"][element["id"].get<std::string>()] = subgroup;
					}
					if(element.find("parent") == element.end() && element.find("resources") != element.end()) {
						auto subgroup = nlohmann::ordered_json {
							{"is_composite", false}
						};
						subgroup["subgroup"][element["id"].get<std::string>()] = thiz.convert_common(element);
						result["groups"][element["id"].get<std::string>()] = subgroup;
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

			inline static auto convert_fs(
				std::string_view source,
				std::string_view destination,
				PathStyle style
			) -> void 
			{
				auto view = std::make_unique<ResourceGroup::Convert>(style);
				FileSystem::write_json(destination, view->convert_whole(FileSystem::read_json(source)));
				return;
			}

			// default constructor

			explicit constexpr Convert(
				PathStyle style
			) noexcept : use_string_for_style(style == PathStyle::WindowStyle)
			{

			}

			// default destructor

			~Convert(

			) = default;
	};
}