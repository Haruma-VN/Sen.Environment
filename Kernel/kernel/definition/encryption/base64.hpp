#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/basic/filesystem.hpp"
#include "kernel/definition/basic/path.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption {

	/**
	 * Base64 class to decode & encode
	*/

	class Base64 {

		public:

			/**
			 * Pass message to decode
			*/

			inline static auto decode(
				const std::string &message
			) -> std::string const
			{
				return Sen::Kernel::Dependencies::Base64::base64_decode(message);
			}

			/**
			 * Pass message to encode
			*/

			inline static auto encode(
				const std::string &message
			) -> std::string const
			{
				return Sen::Kernel::Dependencies::Base64::base64_encode(message);
			}

			/**
			 * Pass message to encode
			*/

			inline static auto encode(
				const unsigned char* message,
				std::size_t size
			) -> std::string const
			{
				return Sen::Kernel::Dependencies::Base64::base64_encode(message, size);
			}

			
			/**
			 * filePath: the file path to decode
			 * outPath: output file
			*/

			inline static auto decode_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				Sen::Kernel::FileSystem::write_file(destination, Sen::Kernel::Definition::Encryption::Base64::decode(Sen::Kernel::FileSystem::read_file(source)));
				return;
			}

			/**
			 * filePath: the file path to encode
			 * outPath: output file
			*/

			inline static auto encode_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				Sen::Kernel::FileSystem::write_file(destination, Sen::Kernel::Definition::Encryption::Base64::encode(Sen::Kernel::FileSystem::read_file(source)));
				return;
			}

			/**
			 * Process
			*/

			inline static auto encode_fs_as_multiple_thread(
				const std::vector<std::vector<std::string>> & paths
			) -> void 
			{
				auto threads = std::vector<std::thread>{};
				auto file_mutexes = std::unordered_map<std::string, std::mutex>{};
				for (const auto & data : paths) {
					threads.emplace_back([=, &file_mutexes]() { 
					auto lock_source = std::lock_guard<std::mutex>(file_mutexes[data[0]]);
					auto lock_destination = std::lock_guard<std::mutex>(file_mutexes[data[1]]);
						Base64::encode_fs(data[0], data[1]); 
					});
				}
				for (auto & thread : threads) {
					thread.join();
				}
				return;
			}

			/**
			 * Process
			*/

			inline static auto decode_fs_as_multiple_thread(
				const std::vector<std::vector<std::string>> & paths
			) -> void 
			{
				auto threads = std::vector<std::thread>{};
				auto file_mutexes = std::unordered_map<std::string, std::mutex>{};
				for (const auto & data : paths) {
					threads.emplace_back([=, &file_mutexes]() { 
					auto lock_source = std::lock_guard<std::mutex>(file_mutexes[data[0]]);
					auto lock_destination = std::lock_guard<std::mutex>(file_mutexes[data[1]]);
						Base64::decode_fs(data[0], data[1]); 
					});
				}
				for (auto & thread : threads) {
					thread.join();
				}
				return;
			}
	};
}