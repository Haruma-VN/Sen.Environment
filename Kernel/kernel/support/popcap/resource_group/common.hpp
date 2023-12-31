#pragma once
#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceGroup {

	// using nlohmann json

	using namespace nlohmann;

	// Resource Group class

	class RewriteSlot {

		protected:

			static auto rewrite_slot_count(
				nlohmann::ordered_json &resource
			) -> void
			{
				auto slot_group = std::map<std::string, size_t>();
				for(auto &e : resource["groups"]){
					if(e.find("resources") == e.end()){
						continue;
					}
					for(auto &c : e["resources"]){
						if(slot_group.find(c["id"]) == slot_group.end()){
							c["slot"] = slot_group.size();
							slot_group.insert(std::pair<std::string, size_t>(c["id"], slot_group.size()));
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

			RewriteSlot(

			) = default;

			~RewriteSlot(

			) = default;
	};

	class BasicConversion : public RewriteSlot {

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
				auto resource = FileSystem::read_json(infile);
				try_assert(!resource["groups"].is_null(), fmt::format("\"{}\" cannot be null in resources.json", "groups"));
				FileSystem::createDirectory(fmt::format("{}/{}", Path::toPosixStyle(output), "subgroup"));
				auto content = json{};
				for(auto & c : resource["groups"])
				{
					if(c.find("resources") != c.end())
					{
						for(auto & e : c["resources"])
						{
							e.erase("slot");
						}
					}
					if((c.find("resources") != c.end()) && (c.find("parent") != c.end()))
					{
						FileSystem::write_json(fmt::format("{}/subgroup/{}.json", output, c["id"]), c);
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
							FileSystem::write_json(fmt::format("{}/subgroup/{}.json", output, c["id"]), c);
						}
					}
				}
				FileSystem::write_json(fmt::format("{}/content.json", output), content);
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
				auto content = FileSystem::read_json(Path::normalize(fmt::format("{}/{}", directoryPath, "content.json")));
				auto resources_json = nlohmann::ordered_json{
					{"version", 1},
					{"content_version", 1},
					{"slot_count", 0}
				};
				auto groups = nlohmann::ordered_json::array_t();
				for(auto & [parent, parent_value] : content.items()){
					if(content[parent]["is_composite"]){
						auto composite_object = nlohmann::ordered_json{
							{"id", parent},
							{"type", "composite"},
							{"subgroups", nlohmann::ordered_json::array()}
						};
						for(auto & [subgroup, subgroup_value] : content[parent]["subgroups"].items()){
							auto resource_for_subgroup = nlohmann::ordered_json{{"id", subgroup}};
							if(!content[parent]["subgroups"][subgroup]["type"].is_null()){
								resource_for_subgroup["res"] = content[parent]["subgroups"][subgroup]["type"];
							}
							composite_object["subgroups"].push_back(resource_for_subgroup);
						}
						groups.push_back(composite_object);
					}
					for(auto & [subgroup, subgroup_value] : content[parent]["subgroups"].items()){
						auto resource_json_path = Path::normalize(fmt::format("{}/subgroup/{}.json", directoryPath, subgroup));
						auto resource_content = FileSystem::read_json(resource_json_path);
						try_assert(resource_content.find("resources") != resource_content.end(), fmt::format("Property \"{}\" cannot be null", "groups"));
						groups.push_back(resource_content);
					}
				}
				resources_json["groups"] = groups;
				BasicConversion::rewrite_slot_count(resources_json);
				FileSystem::write_json(fileOutput, resources_json);
				return;
			}
	};

}