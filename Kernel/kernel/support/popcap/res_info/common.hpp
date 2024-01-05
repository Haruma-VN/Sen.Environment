#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::ResInfo {

	// use definition

	using namespace Definition;

	// Base

	class Common {

		protected:

			/**
			 * abstract method
			*/

			virtual auto generate_subgroup(
				const nlohmann::ordered_json & resource
			) -> nlohmann::ordered_json = 0;

			/**
			 * abstract method
			*/

			virtual auto convert_info(
				const nlohmann::ordered_json &resource
			) -> nlohmann::ordered_json = 0;

		public:

			// constructor

			Common(

			) = default;

			// destructor

			~Common(

			) = default;

			/**
			 * abstract method
			*/

			virtual auto split(
				const std::string & source, 
				const std::string & destination
			) -> void = 0;

			/**
			 * abstract method
			*/

			virtual auto merge(
				const std::string & source, 
				const std::string & destination
			) -> void = 0;

	};
	
	// Basic Conversion for JSON

	class BasicConversion : Common {
		

		protected:

			/**
			 * resource: resource data
			 * return: json map
			*/

			auto generate_subgroup(
				const nlohmann::ordered_json & resource
			) -> nlohmann::ordered_json override final
			{
				return nlohmann::ordered_json {
					{"is_composite", resource["is_composite"]},
					{"subgroups", Object::keys(resource["subgroup"])}
				};
			}

			/**
			 * 
			*/

			auto convert_info(
				const nlohmann::ordered_json & resource
			) -> nlohmann::ordered_json override final
			{
				auto result = nlohmann::ordered_json{
					{"information", nlohmann::ordered_json {
						{"expand_path", resource["expand_path"]}
					}}
				};
				auto group = nlohmann::ordered_json{};
				auto group_key = Object::keys(resource["groups"]);
				for(auto i : Range<size_t>(group_key.size())){
					group[group_key[i]] = thiz.generate_subgroup(resource["groups"][group_key[i]]);
				}
				result["groups"] = group;
				return result;
			}

		public:

			// constructor

			BasicConversion(

			) = default;

			// destructor

			~ BasicConversion(

			) = default;

			/**
			 * split method
			 * source: source file
			 * destination: destination directory
			 * return: the newly directory with splitted content
			*/

			auto split(
				const std::string & source,
				const std::string & destination
			) -> void override final
			{
				auto res_info = FileSystem::read_json(source);
				auto info = thiz.convert_info(res_info);
				auto group_directory = Path::normalize(fmt::format("{}/{}", destination, "groups"));
				FileSystem::createDirectory(group_directory);
				FileSystem::write_json(Path::normalize(fmt::format("{}/{}", destination, "info.json")), info);
				auto keys = Object::keys(res_info["groups"]);
				for(auto i : Range<size_t>(keys.size())){
					auto subgroup_key = Object::keys(res_info["groups"][keys.at(i)]["subgroup"]);
					for(auto j : Range<size_t>(subgroup_key.size())){
						FileSystem::write_json(Path::normalize(fmt::format("{}/{}.json", group_directory, subgroup_key.at(j))), res_info["groups"][keys.at(i)]["subgroup"][subgroup_key.at(j)]);
					}
				}
				return;
			}

			/**
			 * merge method
			 * source: source directory
			 * destination: destination file
			 * return: the newly res-info
			*/

			auto merge(
				const std::string & source,
				const std::string & destination
			) -> void override final
			{
				auto info = FileSystem::read_json(Path::normalize(fmt::format("{}/{}", source, "info.json")));
				auto res_info = nlohmann::ordered_json{
					{"expand_path", info["information"]["expand_path"]}
				};
				for(auto & [group_name, group_value] : info["groups"].items()) {
					auto subgroups = nlohmann::ordered_json{
						{"is_composite", group_value["is_composite"]}
					};
					for(auto & subgroup_name : group_value["subgroups"]) {
						subgroups["subgroup"][subgroup_name.get<std::string>()] = FileSystem::read_json(Path::normalize(fmt::format("{}/{}/{}.json", source, "groups", subgroup_name)));
					}
					res_info["groups"][group_name] = subgroups;
				}
				FileSystem::write_json(destination, res_info);
				return;
			}

			/**
			 * Quick split method
			 * @param source: source file
			 * @param destination: destination directory
			 * return: the newly directory with splitted content
			*/

			static auto split_fs(
				const std::string & source,
				const std::string & destination
			) -> void
			{
				auto resource_handler = ResInfo::BasicConversion{};
				resource_handler.split(source, destination);
				return;
			}

			/**
			 * Quick merge method
			 * @param source: source directory
			 * @param destination: destination file
			 * @return: the newly res-info
			*/

			static auto merge_fs(
				const std::string & source,
				const std::string & destination
			) -> void
			{
				auto resource_handler = ResInfo::BasicConversion{};
				resource_handler.merge(source, destination);
				return;
			}

	};
}