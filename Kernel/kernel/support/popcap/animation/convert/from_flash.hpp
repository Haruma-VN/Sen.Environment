#pragma once

#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{
	template <auto split_label>
	class FromFlash : public Common
	{
	public:
		inline static auto process_whole(
			SexyAnimation &definition,
			ExtraInfo const &extra,
			std::string_view source
		) -> void
		{
			static_assert(split_label == true || split_label == false, "split_label must be true or false");
			// TODO
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination) -> void
		{
			static_assert(split_label == true || split_label == false, "split_label must be true or false");
			auto definition = SexyAnimation{};
			auto extra = *FileSystem::read_json(fmt::format("{}/extra.json", source));
			process_whole(definition, extra, source);
			FileSystem::write_json(destination, definition);
			return;
		}
	};

	using ConvertFromFlashWithMainSprite = FromFlash<false>;

	using ConvertFromFlashWithLabel = FromFlash<true>;
}