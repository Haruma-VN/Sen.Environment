#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption {

	class SHA512 {

		public:

			/**
			 * message: the message to hash
			 * return: the hashed message
			*/

			inline static auto hash(
				const std::string & message
			) -> std::string
			{
				auto sha512 = std::make_shared<Sen::Kernel::Dependencies::SHA::SHA512>();
				auto result = sha512->hash(message);
				return result;
			}

			/**
			 * @param source: source file
			 * @returns: hashed string
			*/

			inline static auto hash_fs(
				const std::string & source
			) -> std::string const
			{
				return SHA512::hash(FileSystem::read_file(source));
			}

	};
}