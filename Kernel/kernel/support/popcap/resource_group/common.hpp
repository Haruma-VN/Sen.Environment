#pragma once
#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::ResourceGroup {

	// using string

	using std::string;

	// using nlohmann json

	using namespace nlohmann;

	struct SubContent {
		bool is_composite;
		json subgroup;
	};

	// split

	inline static auto split(
		string infile,
		string output
	) -> void
	{
		auto resource = FileSystem::readJson(infile);
		try_assert(!resource["groups"].is_null(), fmt::format("\"{}\" cannot be null in resources.json", "groups"));
		FileSystem::createDirectory(fmt::format("{}/{}", Path::toPosixStyle(output), "subgroup"));
		auto content = json{};
		for(auto &c : resource["groups"])
		{
			if(!(c["resources"].is_null()))
			{
				for(auto &e : c["resources"])
				{
					e.erase("slot");
				}
			}
			if(!(c["resources"].is_null()) && !(c["parent"].is_null()))
			{
				FileSystem::writeJson(fmt::format("{}/subgroup/{}.json", output, c["id"]), c);
			}
			if((!c["subgroups"].is_null()) || (!c["resources"].is_null() && c["parent"].is_null())){
				if(!c["subgroups"].is_null())
				{
					content[c["id"]]["is_composite"] = true;
					for(auto &e : c["subgroups"]){
						content[c["id"]]["subgroups"][e["id"]] = json{{"type", e["res"]}};
					}
				}
				else{
					content[c["id"]] = json{
						{"is_composite",  false},
						{"subgroups", json{
							{c["id"], json{
								{"type", nullptr}
							}}
						}}
					};
					FileSystem::writeJson(fmt::format("{}/subgroup/{}.json", output, c["id"]), c);
				}
			}
		}
		FileSystem::writeJson(fmt::format("{}/content.json", output), content);
		return;
	}
	


}