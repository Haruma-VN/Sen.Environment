#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/basic/filesystem.hpp"

namespace Sen::Kernel::Definition::Compression::Zip {

	/**
	 * 
	 * Zip compress struct
	 * 
	*/

	struct Compress {

		protected:


			/**
			 * Chunk Size
			*/

			inline static auto constexpr CHUNK_SIZE = 1024;

			/**
			 * Lambda close zip
			*/

			inline static auto constexpr zip_deleter = [](auto zip) {  
				zip_close(zip);
				return;
			};

			/**
			 * Lambda close directory
			*/

			inline static auto constexpr dir_deleter = [](auto dir) { 
				closedir(dir);
				return;
			};

		public:

			/**
			 * Constructor
			*/

			explicit Compress(

			) = default;

			/**
			 * Destructor
			*/

			~Compress(

			) = default;

			/**
			 * Compress whole directory into a zip
			*/

			inline static auto directory(
				std::string_view source, 
				std::string_view destination
			) -> void
			{
				auto zip = std::unique_ptr<struct zip_t, decltype(zip_deleter)> (zip_open(destination.data(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w'), zip_deleter);
				auto dir = std::unique_ptr<DIR, decltype(dir_deleter)>(opendir(source.data()), dir_deleter);
				auto entry = static_cast<struct dirent*>(nullptr);
				while ((entry = readdir(dir.get())) != NULL) {
					if (entry->d_type == DT_REG) {
						auto file_path = std::make_unique<char[]>(CHUNK_SIZE);
						snprintf(file_path.get(), CHUNK_SIZE, "%s/%s", source.data(), entry->d_name);
						zip_entry_open(zip.get(), entry->d_name);
						zip_entry_fwrite(zip.get(), file_path.get());
						zip_entry_close(zip.get());
					}
				}
				return;
			}

			/**
			 * Compress files into a zip
			*/

			inline static auto file(
				const std::vector<std::string> & source,
				std::string_view destination,
    			std::string_view root = ""_sv
			) -> void
			{
				auto zip = std::unique_ptr<struct zip_t, decltype(zip_deleter)>(zip_open(destination.data(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w'), zip_deleter);
				auto root_is_not_empty = root != "";
				for (auto & file : source) {
					auto zip_path = std::string{file};
					if (root_is_not_empty){
						auto pos = zip_path.find(root);
						if (pos != std::string::npos) {
							zip_path.erase(pos, root.length());
						}
					}
					std::replace(zip_path.begin(), zip_path.end(), '\\', '/');
					zip_entry_open(zip.get(), zip_path.data());
					{
						auto current_data = FileSystem::read_binary<char>(file);
						zip_entry_write(zip.get(), current_data.data(), current_data.size());
					}
					zip_entry_close(zip.get());
				}
				return;
			}

	};


	struct Uncompress {

		public:
			/**
			 * Constructor
			*/

			explicit Uncompress(

			) = default;

			/**
			 * Destructor
			*/

			~Uncompress(

			) = default;

			/**
			 * unzip a zip file to a directory
			*/

			inline static auto process(
				std::string_view zip_path, 
				std::string_view dir_path
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
		};
}

