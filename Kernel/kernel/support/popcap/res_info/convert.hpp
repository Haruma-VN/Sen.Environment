#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_group/convert.hpp"
#include "kernel/support/popcap/resource_group/common.hpp"

namespace Sen::Kernel::Support::PopCap::ResInfo {
	
	// use definition

	using namespace Definition;

	// sub information

	struct SubInformation {

		public:

			// id

			std::string id;

			// parent

			std::string parent;

			// constructor

			SubInformation(

			) = default;

			// should use this constructor

			SubInformation(
				const std::string & id,
				const std::string & parent
			) : id(id), parent(parent)
			{

			}
			
			// destructor

			~SubInformation(

			) = default;


	};

	// using namespace resource group

	using namespace ResourceGroup;

	// virtual class inheritit common

	class Virtual : public ResourceGroup::Common {

		protected:

			// virtual composite generator

			virtual auto generate_composite(
				const std::string & id,
    			const nlohmann::ordered_json & composite
			) -> nlohmann::ordered_json = 0;

			// virtual common generator

			virtual auto generate_common(
				const SubInformation & extra_information,
				const nlohmann::ordered_json & resource_information
			) -> nlohmann::ordered_json = 0;

			// virtual image generator

			virtual auto generate_image(
				const SubInformation & extra_information,
				const nlohmann::ordered_json & resource_information
			) -> nlohmann::ordered_json = 0;

		public:

			// composite

			inline static auto const Composite = std::string{"composite"};

			// simple

			inline static auto const Simple = std::string{"simple"};

			// constructor

			Virtual(

			) = default;

			// destructor

			~Virtual(

			) = default;

			// need override this method

			virtual auto convert_whole(
				const nlohmann::ordered_json & res_info
			) -> nlohmann::ordered_json = 0;
			
	};

	// convert class

	class Convert : public Virtual, public RewriteSlot {

		protected:

			/**
			 * Generate composite for object
			 * id: current id
			 * composite: composite json object
			 * return: new json object that has been generated
			*/

			auto generate_composite(
				const std::string & id,
    			const nlohmann::ordered_json & composite
			) -> nlohmann::ordered_json override final
			{
				auto result = nlohmann::ordered_json{
					{ "type", Convert::Composite },
					{ "id", id },
					{ "subgroups", nlohmann::ordered_json::array() }
				};
				for(auto & [element, value] : composite["subgroup"].items()) {
					auto subgroup = nlohmann::ordered_json {
						{"id", element}
					};
					if(!composite["subgroup"][element]["type"].is_null() and composite["subgroup"][element]["type"].get<std::string>() != Convert::emptyType){
						subgroup["res"] = composite["subgroup"][element]["type"].get<std::string>();
					}
					result["subgroups"].push_back(subgroup);
				}
				return result;
			}

			/**
			 * Generate common file information
			 * extra information: necessary information, parent can be empty btw
			 * resource_information: necessary resource information
			 * return: generated common information
			*/

			auto generate_common(
				const SubInformation & extra_information,
				const nlohmann::ordered_json & resource_information
			) -> nlohmann::ordered_json override final
			{
				auto result = nlohmann::ordered_json {
					{"type", Simple},
					{"id", extra_information.id},
					{"resources", nlohmann::ordered_json::array() }
				};
				if(!extra_information.parent.empty()){
					result["parent"] = extra_information.parent;
				}
				for(auto & [key, value] : resource_information["packet"]["data"].items()){
					auto resource = nlohmann::ordered_json {
						{"type", value["type"].get<std::string>()},
						{"slot", 0},
						{"id", key}
					};
					if(thiz.use_string_for_style){
						resource["path"] = String::join(value["path"].get<std::vector<std::string>>(), ResourceGroup::Common::WindowStyle);
					}
					else{
						resource["path"] = value["path"].get<std::vector<std::string>>();
					}
					if(value.find("srcpath") != value.end()){
						if(thiz.use_string_for_style){
							resource["srcpath"] = String::join(value["srcpath"].get<std::vector<std::string>>(), ResourceGroup::Common::WindowStyle);
						}
						else{
							resource["srcpath"] = value["srcpath"].get<std::vector<std::string>>();
						}
					}
					if(value.find("forceOriginalVectorSymbolSize") != value.end() and value["forceOriginalVectorSymbolSize"].get<bool>()){
						resource["forceOriginalVectorSymbolSize"] = true;
					}
					result["resources"].push_back(resource);
				}
				return result;
			}

			/**
			 * Generate atlas information
			 * extra information: necessary information, parent can not be empty
			 * resource_information: necessary resource information
			 * return: generated atlas information
			*/

			auto generate_image(
				const SubInformation & extra_information,
				const nlohmann::ordered_json & resource_information
			) -> nlohmann::ordered_json override final
			{
				auto result = nlohmann::ordered_json {
					{"type", Simple},
					{"id", extra_information.id},
					{"res", resource_information["type"].get<std::string>()},
					{"parent", extra_information.parent},
					{"resources", nlohmann::ordered_json::array()}
				};
				for(auto & [key, value] : resource_information["packet"].items()){
					auto resource = nlohmann::ordered_json {
						{"type", value["type"].get<std::string>()},
						{"slot", 0},
						{"id", key},
						{"atlas", true},
						{"runtime", true},
						{"width", value["dimension"]["width"].get<int>()},
						{"height", value["dimension"]["height"].get<int>()}
					};
					if(thiz.use_string_for_style){
						resource["path"] = String::join(value["path"].get<std::vector<std::string>>(), ResourceGroup::Common::WindowStyle);
					}
					else{
						resource["path"] = value["path"].get<std::vector<std::string>>();
					}
					result["resources"].push_back(resource);
					for(auto & [sub, sub_value] : value["data"].items()){
						auto sub_resource = nlohmann::ordered_json {
							{"type", sub_value["type"].get<std::string>()},
							{"slot", 0},
							{"id", sub},
							{"parent", key}
						};
						if(thiz.use_string_for_style){
							sub_resource["path"] = String::join(sub_value["path"].get<std::vector<std::string>>(), ResourceGroup::Common::WindowStyle);
						}
						else{
							sub_resource["path"] = sub_value["path"].get<std::vector<std::string>>();
						}
						if (sub_value["default"].find("x") != sub_value["default"].end() and sub_value["default"]["x"].get<int>() != ResourceGroup::Common::DefaultCoordinateOffset) {
							sub_resource["x"] =  sub_value["default"]["x"].get<int>();
						}
						if (sub_value["default"].find("y") != sub_value["default"].end() and sub_value["default"]["y"].get<int>() != ResourceGroup::Common::DefaultCoordinateOffset) {
							sub_resource["y"] =  sub_value["default"]["y"].get<int>();
						}
						if (sub_value["default"].find("rows") != sub_value["default"].end() and sub_value["default"]["rows"].get<int>() != ResourceGroup::Common::DefaultLayoutOffset) {
							sub_resource["rows"] =  sub_value["default"]["rows"].get<int>();
						}
						if (sub_value["default"].find("cols") != sub_value["default"].end() and sub_value["default"]["cols"].get<int>() != ResourceGroup::Common::DefaultLayoutOffset) {
							sub_resource["cols"] =  sub_value["default"]["cols"].get<int>();
						}
						sub_resource["ax"] = sub_value["default"]["ax"].get<int>();
						sub_resource["ay"] = sub_value["default"]["ay"].get<int>();
						sub_resource["aw"] = sub_value["default"]["aw"].get<int>();
						sub_resource["ah"] = sub_value["default"]["ah"].get<int>();
						result["resources"].push_back(sub_resource);
					}
				}
				return result;
			}


		private:

			// use windows path : new style

			bool use_string_for_style = true;


		public:

			// constructor

			Convert(

			) = default;

			// destructor

			~Convert(

			) = default;

			/**
			 * This method do everything 
			 * res_info: Res Info after deserialized as object
			 * return: Resource Group
			*/

			auto convert_whole(
				const nlohmann::ordered_json & res_info
			) -> nlohmann::ordered_json override final
			{
				try_assert(res_info.find("expand_path") != res_info.end(), fmt::format("Property \"{}\" cannot be null in Res-Info", "expand_path"));
				try_assert(res_info.find("groups") != res_info.end(), fmt::format("Property \"{}\" cannot be null in Res-Info", "groups"));
				if(res_info["expand_path"].get<std::string>() == "string"){
					thiz.use_string_for_style = true;
				}
				else{
					thiz.use_string_for_style = false;
				}
				auto result = nlohmann::ordered_json{
					{"version", 1},
					{"content_version", 1},
					{"slot_count", 0},
					{"groups", nlohmann::ordered_json::array()}
				};
				for(auto & [composite_name, group] : res_info["groups"].items()){
					if(group["is_composite"].get<bool>()){
        				result["groups"].push_back(thiz.generate_composite(composite_name, group));
						for(auto & [subgroup_name, subgroup_value] : group["subgroup"].items()){
							if (!subgroup_value["type"].is_null() and subgroup_value["type"].get<std::string>() != Convert::emptyType) {
								result["groups"].push_back(
									thiz.generate_image(
										SubInformation(subgroup_name, composite_name),
										subgroup_value
									)
								);
							}
							else{
								result["groups"].push_back(
									thiz.generate_common(
										SubInformation(subgroup_name, composite_name),
										subgroup_value
									)
								);
							}
						}
					}
					else{
						for(auto & [subgroup_name, subgroup_value] : group["subgroup"].items()){
							result["groups"].push_back(
								thiz.generate_common(
									SubInformation(subgroup_name, Convert::emptyString),
									subgroup_value
								)
							);
						}
					}
				}
				Convert::rewrite_slot_count(result);
				return result;
			}

			/**
			 * Quick convert function
			 * @param res_info: Res-Info object
			 * @return: nlohmann json object as Resource Group
			*/

			static auto convert(
				const nlohmann::ordered_json & res_info
			) -> nlohmann::ordered_json const
			{
				auto* convert_c = new ResInfo::Convert();
				auto result = convert_c->convert_whole(res_info);
				delete convert_c;
				convert_c = nullptr;
				return result;
			}

			/**
			 * Quick convert function for file
			 * @param source: Res-Info file
			 * @param destination: Resource-Group file
			 * @return: Res-Info to Resource-Group
			*/

			static auto convert_fs(
				const std::string & source,
				const std::string & destination
			) -> void
			{
				FileSystem::writeJson(destination, Convert::convert(FileSystem::readJson(source)));
				return;
			}

	};
};