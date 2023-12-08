#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/resource_group/convert.hpp"

namespace Sen::Kernel::Support::PopCap::ResInfo {

	// using namespace resource group

	using namespace ResourceGroup;

	// virtual class inheritit common

	class Virtual : Common {

		public:

			// constructor

			Virtual(

			) = default;

			// destructor

			~Virtual(

			) = default;

			// need override this method

			virtual auto convert_whole(
				const nlohmann::json & res_info
			) -> nlohmann::json = 0;
			
	};

	// convert class

	class Convert : Virtual {

		private:

			// use windows path : new style

			bool use_string_for_style;


		public:

			// constructor

			Convert(

			) = default;

			// destructor

			~Convert(

			) = default;

			auto convert_whole(
				const nlohmann::json & res_info
			) -> nlohmann::json override final
			{
				if()
			}

			static auto convert(
				const nlohmann::json & res_info
			) -> nlohmann::json
			{

				auto* convert_c = new ResInfo::Convert{};
				auto result = convert_c->convert_whole(res_info);
				delete convert_c;
				convert_c = nullptr;
				return result;
			}

			static auto convert_fs(
				const std::string & source,
				const std::string & destination
			) -> void
			{
				FileSystem::writeJson(destination, Convert::convert(FileSystem::readJson(source)));
				return;
			}

	};
};