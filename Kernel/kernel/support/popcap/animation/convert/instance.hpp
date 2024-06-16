#pragma once


#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"
#include "kernel/support/popcap/animation/convert/from_flash.hpp"
#include "kernel/support/popcap/animation/convert/to_flash.hpp"
#include "kernel/support/popcap/animation/decode.hpp"
#include "kernel/support/popcap/animation/encode.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert {

	struct InstanceConvert {
	public:
		template <auto split_label>
		inline static auto to_flash(
			std::string_view source,
			std::string_view destination,
			int resolution
		) -> void
		{
			static_assert(split_label == true || split_label == false, "split_label must be true or false");
			auto stream = DataStreamView{source};
			auto definition = SexyAnimation{};
			Decode::process_whole(stream, definition);
			if constexpr (split_label) {
				ConvertToFlashWithLabel::process_fs(source, destination, resolution);
			}
			else {
				ConvertToFlashWithMainSprite::process_fs(source, destination, resolution);
			}
			return;
		}

		template <auto split_label>
		inline static auto from_flash(
			std::string_view source,
			std::string_view destination
		) -> void
		{
			static_assert(split_label == true || split_label == false, "split_label must be true or false");
			auto definition = SexyAnimation{};
			auto extra = *FileSystem::read_json(fmt::format("{}/extra.json", source));
			if constexpr (split_label) {
				ConvertFromFlashWithLabel::process_whole(definition, extra, source);
			}
			else {
				ConvertFromFlashWithMainSprite::process_whole(definition, extra, source);
			}
			auto stream = DataStreamView{};
			Encode::process_whole(stream, definition);
			stream.out_file(destination);
			return;
		} 
	};
}