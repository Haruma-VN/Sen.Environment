#pragma once


#include "kernel/support/popcap/re_animation/definition.hpp"
#include "kernel/support/popcap/re_animation/convert/from_flash.hpp"
#include "kernel/support/popcap/re_animation/convert/to_flash.hpp"
#include "kernel/support/popcap/re_animation/decode.hpp"
#include "kernel/support/popcap/re_animation/encode.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation::Convert {

	class InstanceConvert {

	private:
		using ToFlash = ToFlash;

		using FromFlash = FromFlash;

		using Decode = Decode;

		using Encode = Encode;

	public:
	
		InstanceConvert(

		) = default;

		~InstanceConvert(

		) = default;

		auto operator=(
			InstanceConvert &&that
		) = delete;

		InstanceConvert(
			InstanceConvert &&that
		) = delete;

	public:

		inline static auto to_flash(
			std::string_view source,
			std::string_view destination,
            ReanimPlatform platform
		) -> void
		{
			auto decode = Decode{source};
            auto reanim_info = ReanimInfo{};
            decode.process(reanim_info, platform);
			auto to_flash = ToFlash{};
			to_flash.process(reanim_info, destination);
			return;
		}

		inline static auto from_flash (
			std::string_view source,
			std::string_view destination,
            ReanimPlatform platform
		) -> void
		{
			auto from_flash = FromFlash{};
			auto reanim = ReanimInfo{};
			from_flash.process(source, reanim);
			auto encode = Encode{};
			encode.process(reanim, platform);
            encode.stream->out_file(destination);
			return;
		} 


	};
}