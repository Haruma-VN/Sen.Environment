#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition {

	namespace Object {
		/**
		 * data: Object
		 * return: keys
		*/

		inline static auto keys(
			const nlohmann::ordered_json &data
		) -> std::vector<std::string>
		{
			auto result = std::vector<std::string>();
			for(auto &[key, value] : data.items()){
				result.push_back(key);
			}
			return result;
		}

		/**
		 * data: Object
		 * return: array of value
		*/

		inline static auto values(
			const nlohmann::ordered_json &data
		) -> nlohmann::ordered_json::array_t
		{
			auto result = nlohmann::ordered_json::array_t{};
			for(auto & [key, value] : data.items()){
				result.push_back(value);
			}
			return result;
		}


		template <typename T>
		struct Nullable {

			std::unique_ptr<T> value;
			bool is_null;

			Nullable(
			) : is_null(false), value(std::make_unique<T>(nullptr))
			{

			}

			~Nullable() = default;

			Nullable(T& value) : value(std::make_unique<T>(value))
			{

			}

			Nullable(Nullable&& that) = delete;

			auto operator =(Nullable&& that)->Nullable & = delete;

		};

		template <typename T>
		inline auto to_json(
			nlohmann::ordered_json& nlohmann_json_j,
			const Nullable<T>& nlohmann_json_t
		) -> void
		{
			if (nlohmann_json_t.is_null) {
				nlohmann_json_j = nullptr;
			}
			else {
				nlohmann_json_j = *(nlohmann_json_t.value);
			}
			return;
		}

		template <typename T>
		inline auto from_json(
			const nlohmann::ordered_json& nlohmann_json_j,
			Nullable<T>& nlohmann_json_t
		) -> void
		{

			if (nlohmann_json_j.is_null()) {
				nlohmann_json_t.is_null = true;
			}
			else {
				nlohmann_json_t.is_null = false;
				*nlohmann_json_t.value = std::make_unique<T>(nlohmann_json_j);
			}
			return;
		}
	}

	namespace Map {

		// U is useless, but we need to keep it
		template <typename T, typename U> 
		inline static auto keys(
			std::map<T, U> & map
		) -> std::vector<T>
		{
			auto keys = std::vector<T>{};
			for (auto & [key, value] : map) {
				keys.emplace_back(key);
			}
			return;
		}

		// U is useless, but we need to keep it
		template <typename T, typename U> 
		inline static auto keys(
			std::unordered_map<T, U> & map
		) -> std::vector<T>
		{
			auto keys = std::vector<T>{};
			for (auto & [key, value] : map) {
				keys.emplace_back(key);
			}
			return;
		}

	}
}