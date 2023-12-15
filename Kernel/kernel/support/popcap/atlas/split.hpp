#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Atlas {

	using namespace Definition;

	enum Method
	{
		SPLIT_BY_PATH,
		SPLIT_BY_ID,
	};

	enum PathStyle 
	{
		OLD,
		NEW,
	};

	struct Common {

		public:
			inline static auto const WINDOWS_STYLE = std::string{"\\"};

			inline static auto const POSIX_STYLE = std::string{"/"};
	};

	struct Unpack {

		private:

			bool use_new_layout;

			auto convert_from_resource_group(
				const nlohmann::json & resource,
				const std::string & method
			) -> nlohmann::json 
			{
				auto result = nlohmann::json {
					{"method", method},
					{"expand_path", thiz.use_new_layout ? "new" : "old"},
					{"subgroup", resource["id"].get<std::string>()},
					{"trim", false},
					{"res", resource["res"].get<std::string>()}
				};
				auto group = nlohmann::json{};
				for(auto & subgroup : resource["resources"]){
					#define find_subgroup(sub) subgroup.find(sub) != subgroup.end()
					if(find_subgroup("ax") and find_subgroup("ay") and find_subgroup("aw") and find_subgroup("ah") and find_subgroup("path")){
						auto default_value = nlohmann::json{};
						#define property_is_defined(sub)\
						if(find_subgroup(sub)){ \
							default_value[sub] = subgroup[sub].get<int>(); \
						}
						#define property_is_defined_or(sub, or_value) \
						property_is_defined(sub) else             \
						{                                         \
							default_value[sub] = or_value;    \
						}
						property_is_defined_or("x", 0);
						property_is_defined_or("y", 0);
						property_is_defined("rows");
						property_is_defined("cols");
						auto atlas_wrapper = nlohmann::json{
							{"default", default_value},
							{"path", subgroup['path']}
						};
						group[subgroup["id"].get<std::string>()] = atlas_wrapper;
					}
				}
				result["groups"] = group;
				return result;
			}

		public:

			Unpack(
				bool use_new_layout
			) : use_new_layout(use_new_layout)
			{

			}

			~Unpack(

			) = default;
			

			auto process(
				const nlohmann::json & data,
				 std::vector<std::string> & images,
				Method split_method,
				const std::string & output_directory
			) -> void
			{
				auto resource_used = nlohmann::json{};
				resource_used.merge_patch(data);
				resource_used["resources"] = nlohmann::json::array();
				auto sprite_directory = Path::normalize(fmt::format("{}/{}", output_directory, "sprite"));
				auto use_split_by_path = Method::SPLIT_BY_PATH;
				auto png_table = std::map<std::string, Image<int>>{};
				for(auto & subgroup_children : data["resources"].get<std::vector<nlohmann::json>>()){
					auto current_subgroup_parent = String::replaceAll(subgroup_children["parent"].get<std::string>(), std::string{"ATLASIMAGE_ATLAS_"}, std::string{""});
					std::transform(current_subgroup_parent.begin(), current_subgroup_parent.end(), current_subgroup_parent.begin(), ::toupper);
					#define find_subgroup_children(sub) subgroup_children.find(sub) != subgroup_children.end()
					if(find_subgroup_children("ax") and find_subgroup_children("ay") and find_subgroup_children("aw") and find_subgroup_children("ah") and find_subgroup_children("path")){
						for(auto & png : images){
							if (png_table.find(png) == png_table.end()) {
								png_table[png] = ImageIO::read_png(png);
							}
							auto current_parent = std::regex_replace(png, std::regex{"\\.png$", std::regex_constants::icase}, std::string{""});
							std::transform(current_parent.begin(), current_parent.end(), current_parent.begin(), ::toupper);
							if (find_subgroup_children("parent") and (String{current_parent}.endsWith(current_subgroup_parent))){
								subgroup_children["path"] = thiz.use_new_layout
								? String::split(subgroup_children["path"].get<std::string>(), Common::WINDOWS_STYLE)
								: subgroup_children['path'].get<std::vector<std::string>>();
								ImageIO::write_png(Path::normalize(fmt::format("{}/{}.png", sprite_directory, use_split_by_path ? subgroup_children["path"].get<std::vector<std::string>>()[subgroup_children["path"].get<std::vector<std::string>>().size() - 1] : subgroup_children["id"].get<std::string>())), Image<int>::composite(png_table[png], Rectangle<int>(subgroup_children["ax"].get<int>(), subgroup_children["ay"].get<int>(), subgroup_children["aw"].get<int>(), subgroup_children["ah"].get<int>())));
								resource_used["resources"].push_back(subgroup_children);
							}
						}
					}
				}
				FileSystem::writeJson(Path::normalize(fmt::format("{}/{}", output_directory, "sprite.json")), thiz.convert_from_resource_group(resource_used, use_split_by_path ? std::string{"path"} : std::string{"id"}));
				return;
			}

			static auto process_fs(
				List<std::string> & source,
				const std::string & destination,
				const std::string & expandPath,
    			const std::string & method
			) -> void
			{
				auto c = Unpack{expandPath == std::string{"old"}};
				auto source_jsons = source.filter([](auto e)
				{ 
					return String{e}.match(std::regex("\\.json$", std::regex_constants::icase));
				});
				auto source_pngs = source.filter([](auto e)
				{ 
					return String{e}.match(std::regex("\\.png$", std::regex_constants::icase));
				});
				for(auto & e : source_jsons){
					c.process(FileSystem::readJson(e), source_pngs.value, method == "id" ? Method::SPLIT_BY_ID : Method::SPLIT_BY_PATH, destination);
				}
				return;
			}
		
	};

}