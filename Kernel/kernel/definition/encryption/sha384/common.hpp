#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Dependencies::Encryption {

	class SHA384 {

		public:

			/**
			 * message: the message to hash
			 * return: the hashed message
			*/

			static auto hash(
				const std::string &message
			) -> std::string const
			{
				auto sha384 = new Sen::Kernel::Dependencies::SHA::SHA384{};
				auto const result = sha384->hash(message);
				delete sha384;
				return result;
			}

	};
}