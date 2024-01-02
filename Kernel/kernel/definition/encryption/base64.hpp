#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/basic/filesystem.hpp"
#include "kernel/definition/basic/path.hpp"
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

			inline static auto decode(
				const string &message
			) -> string const
			{
				return Sen::Kernel::Dependencies::Base64::base64_decode(message);
			}

			/**
			 * Pass message to encode
			*/

			inline static auto encode(
				const string &message
			) -> string const
			{
				return Sen::Kernel::Dependencies::Base64::base64_encode(message);
			}

			
			/**
			 * filePath: the file path to decode
			 * outPath: output file
			*/

			inline static auto decode_fs(
				const string &filePath,
				const string &outPath
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
				const string &filePath,
				const string &outPath
			) -> void
			{
				Sen::Kernel::FileSystem::write_file(outPath, Sen::Kernel::Definition::Encryption::Base64::encode(Sen::Kernel::FileSystem::read_file(filePath)));
				return;
			}
	};
}