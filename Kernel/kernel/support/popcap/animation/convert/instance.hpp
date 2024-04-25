#pragma once


#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"
#include "kernel/support/popcap/animation/convert/from_flash.hpp"
#include "kernel/support/popcap/animation/convert/to_flash.hpp"
#include "kernel/support/popcap/animation/decode.hpp"
#include "kernel/support/popcap/animation/encode.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert {

	class InstanceConvert {

	private:
		using ToFlash = ToFlash;

		using FromFlash = FromFlash;

		using Decode = Animation::Decode<int>;

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
			int resolution
		) -> void
		{
			auto decode = Decode{source};
			decode.process();
			auto to_flash = ToFlash{decode.json};
			auto record = RecordInfo{};
			to_flash.process(destination, record, resolution);
			FileSystem::write_json(fmt::format("{}/record.json", destination), record);
			return;
		}

		inline static auto from_flash (
			std::string_view source,
			std::string_view destination
		) -> void
		{
			auto from_flash = FromFlash{};
			auto record_info = *FileSystem::read_json(fmt::format("{}/record.json", source));
			from_flash.process(source, record_info);
			auto encode = Encode{from_flash.animation};
			encode.process();
			encode.sen->out_file(destination);
			return;
		} 


	};
}