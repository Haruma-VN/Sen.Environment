#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/filesystem/common.hpp"
#include "kernel/definition/path/common.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption {

	// using c++ string

	using std::string;

	/**
	 * Base64 class to decode & encode
	*/

	class Base64 {

		public:

			/**
			 * Pass message to decode
			*/

			static auto decode(
				const string &message
			) -> string const
			{
				return Sen::Kernel::Dependencies::Base64::base64_decode(message);
			}

			/**
			 * Pass message to encode
			*/

			static auto encode(
				const string &message
			) -> string const
			{
				return Sen::Kernel::Dependencies::Base64::base64_encode(message);
			}

			
			/**
			 * filePath: the file path to decode
			 * outPath: output file
			*/

			static auto decode_fs(
				const string &filePath,
				const string &outPath
			) -> void
			{
				Sen::Kernel::FileSystem::writeFile(outPath, Sen::Kernel::Definition::Encryption::Base64::decode(Sen::Kernel::FileSystem::readFile(filePath)));
				return;
			}

			/**
			 * filePath: the file path to encode
			 * outPath: output file
			*/

			static auto encode_fs(
				const string &filePath,
				const string &outPath
			) -> void
			{
				Sen::Kernel::FileSystem::writeFile(outPath, Sen::Kernel::Definition::Encryption::Base64::encode(Sen::Kernel::FileSystem::readFile(filePath)));
				return;
			}
	};
}