#pragma once


#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/basic/filesystem.hpp"

namespace Sen::Kernel::Definition::Diff {

	/**
	 * Encode & Decode Struct
	*/

	struct VCDiff {

		public:

			/**
			 * VCDiff Flag
			*/

			enum Flag {
				VCD_STANDARD_FORMAT = 0x00,
				VCD_FORMAT_INTERLEAVED = 0x01,
				VCD_FORMAT_CHECKSUM = 0x02,
				VCD_FORMAT_JSON = 0x04
			};

			/**
			 * Constructor
			*/

			VCDiff(

			) = default;

			/**
			 * Destructor
			*/

			~VCDiff(

			) = default;

			/**
			 * Encode method
			 * @param before: before buffer
			 * @param before_size: buffer size
			 * @param after: after buffer
			 * @param after_size: buffer size
			 * @param flag: encoder flag
			 * @returns: encoded buffer
			*/

			inline static auto encode(
				const char* before,
				size_t before_size,
				const char* after,
				size_t after_size,
				Flag flag
			) -> std::vector<char>
			{
				auto encoding = std::string{};
				auto encoder = std::unique_ptr<open_vcdiff::VCDiffEncoder>(new open_vcdiff::VCDiffEncoder(before, before_size));
				encoder->SetFormatFlags(static_cast<open_vcdiff::VCDiffFormatExtensionFlags>(flag));
				if (!encoder->Encode(after, after_size, &encoding)) {
					throw std::runtime_error("VCDiff Encode failed");
				}
				return std::vector<char>(encoding.begin(), encoding.end());
			}

			/**
			 * Decode method
			 * @param before: before buffer
			 * @param before_size: buffer size
			 * @param patch: after buffer
			 * @param patch_size: buffer size
			 * @param flag: encoder flag
			 * @returns: decoded buffer
			*/

			inline static auto decode(
				const char* before,
				size_t before_size,
				const char* patch,
				size_t patch_size
			) -> std::vector<char>
			{
				auto decoded_data = std::string{};
				auto decoder = std::unique_ptr<open_vcdiff::VCDiffStreamingDecoder>(new open_vcdiff::VCDiffStreamingDecoder());
				decoder->StartDecoding(before, before_size);
				if (!decoder->DecodeChunk(patch, patch_size, &decoded_data)) {
					throw std::runtime_error("VCDiff Decode failed");
				}
				return std::vector<char>(decoded_data.data(), decoded_data.data() + decoded_data.size());
			}

			/**
			 * VCDiff encode file method
			 * @param before_file: Before file
			 * @param after_file: After file
			 * @param patch_file: Output patch file
			 * @param flag: flag
			*/

			inline static auto encode_fs(
				const std::string & before_file,
				const std::string & after_file,
				const std::string & patch_file,
				Flag flag
			) -> void
			{
				auto before = FileSystem::readBinary<char>(before_file);
				auto after = FileSystem::readBinary<char>(after_file);
				FileSystem::writeBinary<char>(patch_file, VCDiff::encode(before.data(), before.size(), after.data(), after.size(), flag));
				return;
			}

			/**
			 * VCDiff decode file method
			 * @param before_file: Before file
			 * @param patch_file: Patch file
			 * @param after_file: Output after file
			*/

			inline static auto decode_fs(
				const std::string & before_file,
				const std::string & patch_file,
				const std::string & after_file
			) -> void
			{
				auto before = FileSystem::readBinary<char>(before_file);
				auto patch = FileSystem::readBinary<char>(patch_file);
				FileSystem::writeBinary<char>(after_file, VCDiff::decode(before.data(), before.size(), patch.data(), patch.size()));
				return;
			}
	};
}