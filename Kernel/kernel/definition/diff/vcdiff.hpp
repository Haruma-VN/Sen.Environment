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
				VCD_FORMAT_JSON = 0x04,
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
			template <typename T, auto flag> requires std::is_integral<T>::value
			inline static auto encode(
				const char* before,
				T before_size,
				const char* after,
				T after_size
			) -> std::vector<unsigned char>
			{
				static_assert(flag == open_vcdiff::VCDiffFormatExtensionFlagValues::VCD_FORMAT_CHECKSUM or flag == open_vcdiff::VCDiffFormatExtensionFlagValues::VCD_FORMAT_INTERLEAVED or open_vcdiff::VCDiffFormatExtensionFlagValues::VCD_FORMAT_JSON or open_vcdiff::VCDiffFormatExtensionFlagValues::VCD_STANDARD_FORMAT, "flag must be one of vcdiff supported format");
				auto encoding = std::string{};
				auto encoder = std::make_unique<open_vcdiff::VCDiffEncoder>(before, before_size);
				encoder->SetFormatFlags(static_cast<open_vcdiff::VCDiffFormatExtensionFlags>(flag));
				if (!encoder->Encode(after, after_size, &encoding)) {
					throw Exception(fmt::format("{}", Language::get("vcdiff.encode.failed")), std::source_location::current(), "encode");
				}
				return std::vector<unsigned char>(encoding.begin(), encoding.end());
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

			template <typename T> requires std::is_integral<T>::value
			inline static auto decode(
				const char* before,
				T before_size,
				const char* patch,
				T patch_size
			) -> std::vector<unsigned char>
			{
				auto decoded_data = std::string{};
				auto decoder = std::make_unique<open_vcdiff::VCDiffStreamingDecoder>();
				decoder->StartDecoding(before, before_size);
				if (!decoder->DecodeChunk(patch, patch_size, &decoded_data)) {
					throw Exception(fmt::format("{}", Language::get("vcdiff.decode.failed")), std::source_location::current(), "decode");
				}
				return std::vector<unsigned char>(decoded_data.data(), decoded_data.data() + decoded_data.size());
			}

			/**
			 * VCDiff encode file method
			 * @param before_file: Before file
			 * @param after_file: After file
			 * @param patch_file: Output patch file
			 * @param flag: flag
			*/

			inline static auto encode_fs(
				std::string_view before_file,
				std::string_view after_file,
				std::string_view patch_file,
				Flag flag
			) -> void
			{
				auto before = FileSystem::read_binary<char>(before_file);
				auto after = FileSystem::read_binary<char>(after_file);
				switch (flag) {
					case Flag::VCD_FORMAT_CHECKSUM:{
						FileSystem::write_binary<unsigned char>(patch_file, VCDiff::encode<std::size_t, Flag::VCD_FORMAT_CHECKSUM>(before.data(), before.size(), after.data(), after.size()));
						break;
					}
					case Flag::VCD_FORMAT_INTERLEAVED:{
						FileSystem::write_binary<unsigned char>(patch_file, VCDiff::encode<std::size_t,Flag::VCD_FORMAT_INTERLEAVED>(before.data(), before.size(), after.data(), after.size()));
						break;
					}
					case Flag::VCD_FORMAT_JSON:{
						FileSystem::write_binary<unsigned char>(patch_file, VCDiff::encode<std::size_t, Flag::VCD_FORMAT_JSON>(before.data(), before.size(), after.data(), after.size()));
						break;
					}
					case Flag::VCD_STANDARD_FORMAT:{
						FileSystem::write_binary<unsigned char>(patch_file, VCDiff::encode<std::size_t, Flag::VCD_STANDARD_FORMAT>(before.data(), before.size(), after.data(), after.size()));
						break;
					}
					default:{
						throw Exception(fmt::format("{}", Kernel::Language::get("vcdiff.encode.flag_not_found")), std::source_location::current(), "encode_fs");
					}
				}
				return;
			}

			/**
			 * VCDiff decode file method
			 * @param before_file: Before file
			 * @param patch_file: Patch file
			 * @param after_file: Output after file
			*/

			inline static auto decode_fs(
				std::string_view before_file,
				std::string_view patch_file,
				std::string_view after_file
			) -> void
			{
				auto before = FileSystem::read_binary<char>(before_file);
				auto patch = FileSystem::read_binary<char>(patch_file);
				FileSystem::write_binary<unsigned char>(after_file, VCDiff::decode(before.data(), before.size(), patch.data(), patch.size()));
				return;
			}
	};
}