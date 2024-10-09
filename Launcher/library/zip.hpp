#pragma once
#include "library/utility.hpp"

auto extract_zipfile(
	const std::string & zip_path,
	const std::string& dir_path
) -> void
{
	zip_extract(
		zip_path.data(),
		dir_path.data(),
		[](auto c, auto i) {
			return 0; },
			nullptr
	);
	return;
}