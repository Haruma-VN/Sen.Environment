#pragma once 

#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Encryption {

	// Byte definition

	typedef unsigned char byte;

	// Using C++ String

	using std::string;

	// Using C++ Span

	using std::span;

	struct MD5 {
		private:

			// hash data

			string dataHash;

			// input data

			span<byte> data;

		public:

			// constructor

			explicit MD5(
				const span<byte> &message
			);

			// destructor

			~MD5(

			) = default;

			// getter

			inline auto getData(

			) -> string;

			// call to hash md5 string

			inline auto hash(

			) -> void;

			

			// msg: Provide message to hash
			// return: the hashed string

			inline static auto hashData(
				const span<byte> &message
			) -> string
			{
				auto *md5 = new Encryption::MD5(message);
				md5->hash();
				auto result = md5->getData();
				delete md5;
				return result;
			}
	};

	MD5::MD5(const span<byte> &message){
		thiz.data = message;
	}

	inline auto MD5::getData(

	) -> string
	{
		return thiz.dataHash;
	}

	inline auto MD5::hash(

	) -> void
	{
		auto* md5 = new Dependencies::md5::MD5(thiz.data);
		thiz.dataHash = md5->toStr();
		delete md5;
		return;
	}

}