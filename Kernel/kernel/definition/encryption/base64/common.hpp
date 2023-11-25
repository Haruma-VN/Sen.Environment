#pragma once 

#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/filesystem/common.hpp"

namespace Sen::Kernel::Encryption {

	template <typename T>
	struct Base64DataView {

		private:

			T *data;

			unsigned int length;

		public:

			Base64DataView(
				T *data, 
				unsigned int length
			) {
				thiz.data = data;
				thiz.length = length;
			}

			~Base64DataView(

			) = default;

			inline auto getData(

			) -> T *
			{
				return thiz.data;
			}

			inline auto size(

			) -> unsigned int
			{
				return thiz.length;
			}
	};

	struct Base64 {

		private:

			unsigned char *decodedData;

			unsigned int size;

			char *encodedData;

		public:

			Base64(
				char* encodedData,
				unsigned int size
			);

			Base64(
				unsigned char* decodedData,
				unsigned int size
			);

			~Base64(

			) = default;

			inline auto decode(

			) -> void;

			inline auto encode(

			) -> void;

			inline auto getDecodedData(

			) -> Base64DataView<unsigned char>;

			inline auto getEncodedData(

			) -> Base64DataView<char>;

			inline static auto decode_fs(
				string filePath
			) -> Base64DataView<unsigned char>
			{
				auto data = FileSystem::readBinary<char>(filePath);
				auto* base64 = new Base64{data.data(), static_cast<unsigned int>(data.size())};
				base64->decode();
				auto result = base64->getDecodedData();
				delete base64;
				return result;
			}

			inline static auto encode_fs(
				string filePath
			) -> Base64DataView<char>
			{
				auto data = FileSystem::readBinary<unsigned char>(filePath);
				auto* base64 = new Base64{data.data(), static_cast<unsigned int>(data.size())};
				base64->encode();
				auto result = base64->getEncodedData();
				delete base64;
				return result;
			}
	};

	Base64::Base64(
		char* encodedData,
		unsigned int size
	)
	{
		thiz.encodedData = encodedData;
		thiz.size = size;
	}

	Base64::Base64(
		unsigned char* decodedData,
		unsigned int size
	)
	{
		thiz.decodedData = decodedData;
		thiz.size = size;
	}

	inline auto Base64::encode(

	) -> void
	{
		auto beforeSize = thiz.size;
		thiz.size = BASE64_ENCODE_OUT_SIZE(thiz.size);
		base64_encode(thiz.decodedData, beforeSize, thiz.encodedData);
		return;
	}

	inline auto Base64::decode(

	) -> void
	{
		auto beforeSize = thiz.size;
		thiz.size = BASE64_ENCODE_OUT_SIZE(thiz.size);
		base64_decode(thiz.encodedData, beforeSize, thiz.decodedData);
		return;
	}

	inline auto Base64::getDecodedData(

	) -> Base64DataView<unsigned char>
	{
		return Base64DataView<unsigned char>{thiz.decodedData, thiz.size};
	}

	inline auto Base64::getEncodedData(

	) -> Base64DataView<char>
	{
		return Base64DataView<char>{thiz.encodedData, thiz.size};
	}

}