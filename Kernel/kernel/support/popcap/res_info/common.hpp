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

			explicit Common(

			) noexcept = default;

			// destructor

			~Common(

			) = default;

			/**
			 * abstract method
			*/

			virtual auto split(
				std::string_view source, 
				std::string_view destination
			) -> void = 0;

			/**
			 * abstract method
			*/

			virtual auto merge(
				std::string_view source, 
				std::string_view destination
			) -> void = 0;

	};
	
	// Basic Conversion for JSON

	class BasicConversion : Common {
		

		protected:

			/**
			 * resource: resource data
			 * return: json map
			*/

			inline auto generate_subgroup(
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

			inline auto convert_info(
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

			explicit BasicConversion(

			) noexcept = default;

			// destructor

			~BasicConversion(

			) = default;

			/**
			 * split method
			 * source: source file
			 * destination: destination directory
			 * return: the newly directory with splitted content
			*/

			inline auto split(
				const std::string_view source,
				const std::string_view destination
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

			inline auto merge(
				std::string_view source,
				std::string_view destination
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
			 * Singleton
			*/

			inline static auto instance(

			) -> BasicConversion&
			{
				static auto INSTANCE = BasicConversion{};
				return INSTANCE;
			}

			/**
			 * Quick split method
			 * @param source: source file
			 * @param destination: destination directory
			 * return: the newly directory with splitted content
			*/

			inline static auto split_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				ResInfo::BasicConversion::instance().split(source, destination);
				return;
			}

			/**
			 * Quick merge method
			 * @param source: source directory
			 * @param destination: destination file
			 * @return: the newly res-info
			*/

			inline static auto merge_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				ResInfo::BasicConversion::instance().merge(source, destination);
				return;
			}

	};
}