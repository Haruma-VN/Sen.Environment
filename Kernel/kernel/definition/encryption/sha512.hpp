#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Dependencies::Encryption {

	class SHA512 {

		public:

			/**
			 * message: the message to hash
			 * return: the hashed message
			*/

			static auto hash(
				const std::string &message
			) -> std::string const
			{
				auto sha512 = new Sen::Kernel::Dependencies::SHA::SHA512{};
				auto const result = sha512->hash(message);
				delete sha512;
				return result;
			}

	};
}