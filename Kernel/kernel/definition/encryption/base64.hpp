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
			 * filePath: the file path to decode
			 * outPath: output file
			*/

			inline static auto decode_fs(
				const std::string &filePath,
				const std::string &outPath
			) -> void
			{
				Sen::Kernel::FileSystem::write_file(outPath, Sen::Kernel::Definition::Encryption::Base64::decode(Sen::Kernel::FileSystem::read_file(filePath)));
				return;
			}

			/**
			 * filePath: the file path to encode
			 * outPath: output file
			*/

			inline static auto encode_fs(
				const std::string &filePath,
				const std::string &outPath
			) -> void
			{
				Sen::Kernel::FileSystem::write_file(outPath, Sen::Kernel::Definition::Encryption::Base64::encode(Sen::Kernel::FileSystem::read_file(filePath)));
				return;
			}

			inline static auto encode_fs_as_multiple_thread(
				const std::vector<std::vector<std::string>> & paths
			) -> void 
			{
				auto threads = std::vector<std::thread>{};
				auto file_mutexes = std::map<std::string, std::mutex>{};
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

			inline static auto decode_fs_as_multiple_thread(
				const std::vector<std::vector<std::string>> & paths
			) -> void 
			{
				auto threads = std::vector<std::thread>{};
				auto file_mutexes = std::map<std::string, std::mutex>{};
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