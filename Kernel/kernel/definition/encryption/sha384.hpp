#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption {

	class SHA384 {

		public:

			/**
			 * message: the message to hash
			 * return: the hashed message
			*/

			inline static auto hash(
				const std::string & message
			) -> std::string 
			{
				auto sha384 = std::make_shared<Sen::Kernel::Dependencies::SHA::SHA384>();
				auto result = sha384->hash(message);
				return result;
			}

			/**
			 * @param source: source file
			 * @return: hashed string
			*/

			inline static auto hash_fs(
				const std::string & source
			) -> std::string 
			{
				return SHA384::hash(FileSystem::readFile(source));
			}

	};
}