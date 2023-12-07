#pragma once
#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceGroup {

	// using string

	using std::string;

	// using nlohmann json

	using namespace nlohmann;

	// Resource Group class

	class BasicConversion {

		private:

			static auto rewrite_slot_count(
				nlohmann::json &resource
			) -> void
			{
				auto slot_group = std::map<std::string, unsigned int>();
				for(auto &e : resource["groups"]){
					if(e.find("resources") == e.end()){
						continue;
					}
					for(auto &c : e["resources"]){
						if(slot_group.find(c["id"]) == slot_group.end()){
							c["slot"] = slot_group.size();
							slot_group.insert(std::pair<std::string, unsigned int>(c["id"], slot_group.size()));
						}
						else{
							c["slot"] = slot_group[c["id"]];
						}
					}
				}
				resource["slot_count"] = slot_group.size();
				return;
			}


		public:

			/**
			 * inFile: the input file
			 * output: the output directory
			*/

			static auto split(
				const string &infile,
				const string &output
			) -> void
			{
				auto resource = FileSystem::readJson(infile);
				try_assert(!resource["groups"].is_null(), fmt::format("\"{}\" cannot be null in resources.json", "groups"));
				FileSystem::createDirectory(fmt::format("{}/{}", Path::toPosixStyle(output), "subgroup"));
				auto content = json{};
				for(auto &c : resource["groups"])
				{
					if(c.find("resources") != c.end())
					{
						for(auto &e : c["resources"])
						{
							e.erase("slot");
						}
					}
					if((c.find("resources") != c.end()) && (c.find("parent") != c.end()))
					{
						FileSystem::writeJson(fmt::format("{}/subgroup/{}.json", output, c["id"]), c);
					}
					if((c.find("subgroups") != c.end()) || (c.find("resources") != c.end() && c.find("parent") == c.end())){
						if(c.find("subgroups") != c.end())
						{
							content[c["id"]]["is_composite"] = true;
							for(auto &e : c["subgroups"]){
								content[c["id"]]["subgroups"][e["id"]] = json{{"type", e["res"]}};
							}
						}
						else{
							content[c["id"]] = json{
								{"is_composite",  false},
								{"subgroups", json{
									{c["id"], json{
										{"type", nullptr}
									}}
								}}
							};
							FileSystem::writeJson(fmt::format("{}/subgroup/{}.json", output, c["id"]), c);
						}
					}
				}
				FileSystem::writeJson(fmt::format("{}/content.json", output), content);
				return;
			}

			/**
			 * directoryPath: the path of directory
			 * file output: file output path
			*/

			static auto merge(
				const string &directoryPath,
				const string &fileOutput
			) -> void
			{
				auto content = FileSystem::readJson(Path::normalize(fmt::format("{}/{}", directoryPath, "content.json")));
				auto resources_json = nlohmann::json{
					{"version", 1},
					{"content_version", 1},
					{"slot_count", 0},
					{"groups", nlohmann::json::array()}
				};
				for(auto & [parent, parent_value] : content.items()){
					if(content[parent]["is_composite"]){
						auto composite_object = nlohmann::json{
							{"id", parent},
							{"type", "composite"},
							{"subgroups", nlohmann::json::array()}
						};
						for(auto & [subgroup, subgroup_value] : content[parent]["subgroups"].items()){
							auto resource_for_subgroup = nlohmann::json{{"id", subgroup}};
							if(!content[parent]["subgroups"][subgroup]["type"].is_null()){
								resource_for_subgroup["res"] = content[parent]["subgroups"][subgroup]["type"];
							}
							composite_object["subgroups"].push_back(resource_for_subgroup);
						}
						resources_json["groups"].push_back(composite_object);
					}
					for(auto & [subgroup, subgroup_value] : content[parent]["subgroups"].items()){
						auto resource_json_path = Path::normalize(fmt::format("{}/subgroup/{}.json", directoryPath, subgroup));
						auto resource_content = FileSystem::readJson(resource_json_path);
						try_assert(resource_content.find("resources") != resource_content.end(), fmt::format("Property \"{}\" cannot be null", "groups"));
						resources_json["groups"].push_back(resource_content);
					}
				}
				BasicConversion::rewrite_slot_count(resources_json);
				FileSystem::writeJson(fileOutput, resources_json);
				return;
			}
	};

}