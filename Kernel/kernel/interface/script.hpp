#pragma once

#include "kernel/interface/shell.hpp"

namespace Sen::Kernel::Interface::Script {

	/**
	 * ----------------------------------------
	 * JS namespace is used
	 * ----------------------------------------
	*/

	namespace JS = Sen::Kernel::Definition::JavaScript;

	/**
	 * ----------------------------------------
	 * JS Method Declaration
	 * ----------------------------------------
	*/

	typedef JSValue (*JavaScriptNativeMethod)(JSContext *, JSValueConst, int, JSValueConst *);

	/**
	 * JavaScript Thread
	*/

	namespace Thread {

		/**
		 * ----------------------------------------
		 * JavaScript Sleep
		 * @param argv[0]: time to sleep
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto sleep(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto time = JS::Converter::get_int64(context, argv[0]);
			Sen::Kernel::Definition::Timer::sleep(time);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JavaScript Date now
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto now(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 0, fmt::format("argument expected {} but received {}", 0, argc));
			auto current = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
			return JS::Converter::to_number(context, current);
		}

	}

	/**
	 * Diff namespace
	*/

	namespace Diff {

		/**
		 * VCDiff interface
		*/

		namespace VCDiff {
			

			inline static auto encode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 4, fmt::format("argument expected {} but received {}", 4, argc));
				auto before_file = JS_ToCString(context, argv[0]);
				auto after_file = JS_ToCString(context, argv[1]);
				auto patch_file = JS_ToCString(context, argv[2]);
				auto flag = JS::Converter::get_int32(context, argv[3]);
				Sen::Kernel::Definition::Diff::VCDiff::encode_fs(before_file, after_file, patch_file, static_cast<Sen::Kernel::Definition::Diff::VCDiff::Flag>(flag));
				JS_FreeCString(context, before_file);
				JS_FreeCString(context, after_file);
				JS_FreeCString(context, patch_file);
				return JS::Converter::get_undefined();
			}

			inline static auto decode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto before_file = JS_ToCString(context, argv[0]);
				auto patch_file = JS_ToCString(context, argv[1]);
				auto after_file = JS_ToCString(context, argv[2]);
				Sen::Kernel::Definition::Diff::VCDiff::decode_fs(before_file, after_file, patch_file);
				JS_FreeCString(context, before_file);
				JS_FreeCString(context, patch_file);
				JS_FreeCString(context, after_file);
				return JS::Converter::get_undefined();
			}

		}

	}

	/**
	 * Operating System
	*/

	namespace OperatingSystem {

		/**
		 * ----------------------------------------
		 * JavaScript Get current OS
		 * @return: current OS
		 * ----------------------------------------
		*/

		inline static auto current(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 0, fmt::format("argument expected {} but received {}", 0, argc));
			#if WINDOWS
				return JS::Converter::to_string(context, std::string{"Windows"});
			#elif LINUX
				return JS::Converter::to_string(context, std::string{"Linux"});
			#elif APPLE
				return JS::Converter::to_string(context, std::string{"Macintosh"});
			#elif ANDROID
				return JS::Converter::to_string(context, std::string{"Android"});
			#elif IPHONE
				return JS::Converter::to_string(context, std::string{"iPhone"});
			#else
				return JS::Converter::to_string(context, std::string{"Unknown OS"});
			#endif
		}	

		#if WINDOWS
		struct FileDeleter {
			inline auto operator()(
				FILE* file
			) const -> void
			{
				if (file) {
					fclose(file);
				}
				return;
			}
		};
		#else
		struct FileDeleter {
			inline auto operator()(
				FILE* file
			) const -> void
			{
				if (file) {
					pclose(file);
				}
				return;
			}
		};
		#endif

		/**
		 * ----------------------------------------
		 * JavaScript Get current Architecture
		 * @return: current Architecture
		 * ----------------------------------------
		*/

		inline static auto architecture(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 0, fmt::format("argument expected {} but received {}", 0, argc));
			#if WINDOWS
				auto si = SYSTEM_INFO{};
				GetNativeSystemInfo(&si);
				switch (si.wProcessorArchitecture)
					{
						case PROCESSOR_ARCHITECTURE_AMD64:{
							return JS::Converter::to_string(context, std::string{"x64"});
						}
						case PROCESSOR_ARCHITECTURE_ARM:{
							return JS::Converter::to_string(context, std::string{"x86"});
						}
						case PROCESSOR_ARCHITECTURE_IA64:{
							return JS::Converter::to_string(context, std::string{"arm64"});
						}
						case PROCESSOR_ARCHITECTURE_INTEL:{
							return JS::Converter::to_string(context, std::string{"arm"});
						}
						default:{
							return JS::Converter::to_string(context, std::string{"Unknown"});
						}
					}
			#else
				auto buffer = std::array<char, 128>();
				auto result = std::string();
				auto pipe = std::unique_ptr<FILE, FileDeleter>(popen("uname -m", "r"));
				if (!pipe) {
					throw std::runtime_error("cannot open pipe");
				}
				while (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
					result += buffer.data();
				}
				result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
				if (result == "x86_64"){
					return JS::Converter::to_string(context, std::string{"x64"});
				}
				else if (result == "i686"){
					return JS::Converter::to_string(context, std::string{"x86"});
				}
				else if (result == "aarch64"){
					return JS::Converter::to_string(context, std::string{"arm64"});
				}
				else if (result == "armv7l"){
					return JS::Converter::to_string(context, std::string{"arm"});
				}
				else{
					return JS::Converter::to_string(context, std::string{"Unknown"});
				}
			#endif
		}	

	}

	namespace ArrayBuffer {

		/**
		 * ----------------------------------------
		 * JavaScript Array Buffer open file
		 * @param argv[0]: source file
		 * @return: Array Buffer
		 * ----------------------------------------
		*/

		inline static auto open(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = JS::Converter::read_file_as_js_arraybuffer(context, source);
			JS_FreeCString(context, source);
			return result;
		}

		/**
		 * ----------------------------------------
		 * JavaScript Array Buffer open file
		 * @param argv[0]: destination file
		 * @param argv[1]: Array Buffer
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto out(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
			auto destination = JS_ToCString(context, argv[0]);
			auto array_buffer = argv[1];
			JS::Converter::write_file_as_arraybuffer(context, destination, array_buffer);
			JS_FreeCString(context, destination);
			return JS::Converter::get_undefined();
		}

	}

	/**
	 * Sub-Process call
	*/

	namespace Process {

		/**
		 * ----------------------------------------
		 * JavaScript Spawn Process
		 * @param argv[0]: process command
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto run(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			Sen::Kernel::Process::run(source);
			JS_FreeCString(context, source);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JavaScript Check Environment
		 * @param argv[0]: process
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto is_exists_in_path_environment(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::Process::is_exists_in_path_environment(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_bool(context, result);
		}

		/**
		 * ----------------------------------------
		 * JavaScript Spawn Process
		 * @param argv[0]: process command
		 * @return: string return
		 * ----------------------------------------
		*/

		inline static auto execute(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::Process::execute(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_string(context, result);
		}

	}

	/**
	 * JavaScript Console Interactive
	*/

	namespace Console {
		/**
		 * ----------------------------------------
		 * JS Print method
		 * @param argv[0]: title
		 * @param argv[1]: color
		 * ----------------------------------------
		*/

		inline static auto print(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 2 or argc == 1, fmt::format("argument expected {} but received {}", "2 or 1", argc));
			auto str = JS_ToCString(context, argv[0]);
			if(argc == 2){
				Shell::print(JS::Converter::get_string(context, argv[0]).c_str(),static_cast<Sen::Kernel::Interface::Color>(JS::Converter::get_int32(context, argv[1])));
			}
			else{
				Shell::print(JS::Converter::get_string(context, argv[0]).c_str(), Sen::Kernel::Interface::Color::DEFAULT);
			}
			JS_FreeCString(context, str);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JS Readline method
		 * @returns: input data
		 * ----------------------------------------
		*/

		inline static auto readline(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 0, fmt::format("argument expected {} but received {}", 0, argc));
			auto result = Shell{}.input();
			auto m_result = std::string{result.data, result.size};
			return JS::Converter::to_string(context, m_result);
		}
	}

	namespace Path {

		/**
		 * ----------------------------------------
		 * JS join path method
		 * @return: joined file
		 * ----------------------------------------
		*/

		inline static auto join(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			auto v = std::vector<std::string>{};
			for(auto i : Range<int>(argc)){
				auto source = JS_ToCString(context, argv[i]);
				v.push_back(std::string{source});
				JS_FreeCString(context, source);
			}
			return JS::Converter::to_string(context, Sen::Kernel::Path::Script::join(v));
		}

		/**
		 * ----------------------------------------
		 * JS Basename for file
		 * @param argv[0]: source file
		 * @return: base name
		 * ----------------------------------------
		*/

		inline static auto basename(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::Path::Script::basename(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS Delimiter of current OS
		 * @param argv[0]: source file
		 * @return: Delimiter of current OS
		 * ----------------------------------------
		*/

		inline static auto delimiter(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			auto result = Sen::Kernel::Path::Script::delimiter();
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS Dirname
		 * @param argv[0]: source file
		 * @return: dirname
		 * ----------------------------------------
		*/

		inline static auto dirname(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::Path::Script::dirname(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS Format
		 * @param argv[0]: source
		 * @return: formatted
		 * ----------------------------------------
		*/

		inline static auto format(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto dir = JS_GetPropertyStr(context, argv[0], "dir");
			auto base = JS_GetPropertyStr(context, argv[0], "base");
			auto source_1 = JS_ToCString(context, dir);
			auto source_2 = JS_ToCString(context, base);
			auto result = Sen::Kernel::Path::Script::format(Sen::Kernel::Path::Format{source_1,source_2});
			JS_FreeCString(context, source_1);
			JS_FreeCString(context, source_2);
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS Normalize
		 * @param argv[0]: source file
		 * @return: normalize
		 * ----------------------------------------
		*/

		inline static auto normalize(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::Path::Script::normalize(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS Resolve
		 * @param argv[0]: source file
		 * @return: resolve
		 * ----------------------------------------
		*/

		inline static auto resolve(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::Path::Script::resolve(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS Extension name
		 * @param argv[0]: source file
		 * @return: extname
		 * ----------------------------------------
		*/

		inline static auto extname(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::Path::Script::extname(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS Is absolute
		 * @param argv[0]: source file
		 * @return: is absolute path
		 * ----------------------------------------
		*/

		inline static auto is_absolute(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::Path::Script::is_absolute(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_bool(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS Relative
		 * @param argv[0]: source file
		 * @param argv[0]: source file
		 * @return: Relative
		 * ----------------------------------------
		*/

		inline static auto relative(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
			auto from = JS_ToCString(context, argv[0]);
			auto to = JS_ToCString(context, argv[1]);
			auto result = Sen::Kernel::Path::Script::relative(from, to);
			JS_FreeCString(context, from);
			JS_FreeCString(context, to);
			return JS::Converter::to_string(context, result);
		}

	}

	/**
	 * JavaScript File System
	*/

	namespace FileSystem {

		/**
		 * ----------------------------------------
		 * JS read file method
		 * @param argv[0]: source file
		 * @return: readed file
		 * ----------------------------------------
		*/

		inline static auto read_file(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::FileSystem::read_file(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS read file method
		 * @param argv[0]: source file
		 * @return: readed file
		 * ----------------------------------------
		*/

		inline static auto read_file_encode_with_utf16le(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::FileSystem::readFileByUtf16LE(source);
			JS_FreeCString(context, source);
			auto converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
			auto utf8_string = std::string{converter.to_bytes(result)};
			return JS::Converter::to_string(context, utf8_string);
		}

		/**
		 * ----------------------------------------
		 * JS write file method
		 * @param argv[0]: destination file
		 * @param argv[1]: data to write
		 * @return: written file
		 * ----------------------------------------
		*/

		inline static auto write_file(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
			auto destination = JS_ToCString(context, argv[0]);
			auto data = JS_ToCString(context, argv[1]);
			Sen::Kernel::FileSystem::write_file(destination, data);
			JS_FreeCString(context, destination);
			JS_FreeCString(context, data);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JS write file method
		 * @param argv[0]: destination file
		 * @param argv[1]: data to write
		 * @return: written file
		 * ----------------------------------------
		*/

		inline static auto write_file_encode_with_utf16le(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
			auto destination = JS_ToCString(context, argv[0]);
			auto data = JS_ToCString(context, argv[1]);
			auto converter = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{};
			auto result = std::wstring{converter.from_bytes(data)};
			Sen::Kernel::FileSystem::write_fileByUtf16LE(destination, result);
			JS_FreeCString(context, destination);
			JS_FreeCString(context, data);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JS read directory method
		 * @param argv[0]: source directory
		 * @return: JS Array of String
		 * ----------------------------------------
		*/

		inline static auto read_current_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::FileSystem::readDirectory(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_array(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS read directory method
		 * @param argv[0]: source directory
		 * @return: JS Array of String
		 * ----------------------------------------
		*/

		inline static auto read_directory_only_file(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::FileSystem::readDirectoryOnlyFile(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_array(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS read directory method
		 * @param argv[0]: source directory
		 * @return: JS Array of String
		 * ----------------------------------------
		*/

		inline static auto read_directory_only_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::FileSystem::readDirectoryOnlyDirectory(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_array(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS read directory method
		 * @param argv[0]: source directory
		 * @return: JS Array of String
		 * ----------------------------------------
		*/

		inline static auto read_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::FileSystem::readWholeDirectory(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_array(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS create directory
		 * @param argv[0]: destination directory
		 * @return: created directory
		 * ----------------------------------------
		*/

		inline static auto create_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto destination = JS_ToCString(context, argv[0]);
			Sen::Kernel::FileSystem::createDirectory(destination);
			JS_FreeCString(context, destination);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JS check file
		 * @param argv[0]: source file
		 * @return: if its file true else false
		 * ----------------------------------------
		*/

		inline static auto is_file(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = std::filesystem::is_regular_file(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_bool(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS check directory
		 * @param argv[0]: source directory
		 * @return: if its directory true
		 * ----------------------------------------
		*/

		inline static auto is_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = std::filesystem::is_directory(source);
			JS_FreeCString(context, source);
			return JS::Converter::to_bool(context, result);
		}

		// Basic Operation for JS

		namespace Operation {

			/**
			 * ----------------------------------------
			 * JS rename file
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @return: undefined
			 * ----------------------------------------
			*/

			inline static auto rename(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				std::filesystem::rename(std::filesystem::path{source}, std::filesystem::path{destination});
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JS copy file
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @return: undefined
			 * ----------------------------------------
			*/

			inline static auto copy(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				std::filesystem::copy(std::filesystem::path{source}, std::filesystem::path{destination});
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JS copy file
			 * @param argv[0]: source file
			 * @return: undefined
			 * ----------------------------------------
			*/

			inline static auto remove(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				std::filesystem::remove(std::filesystem::path{source});
				JS_FreeCString(context, source);
				return JS::Converter::get_undefined();
			}

		}
	}

	/**
	 * JavaScript Image Support
	*/

	namespace Image {

		/**
		 * ----------------------------------------
		 * JavaScript scale image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto scale_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto destination = JS_ToCString(context, argv[1]);
			auto percentage = JS::Converter::get_float32(context, argv[2]);
			Sen::Kernel::Definition::ImageIO::scale_png(source, destination, percentage);
			JS_FreeCString(context, source);
			JS_FreeCString(context, destination);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JavaScript transparent image
		 * @param argv[0]: destination file
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto transparent_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
			auto destination = JS_ToCString(context, argv[0]);
			auto width = JS::Converter::get_int32(context, argv[1]);
			auto height = JS::Converter::get_int32(context, argv[2]);
			Sen::Kernel::Definition::ImageIO::transparent_png(destination, width, height);
			JS_FreeCString(context, destination);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JavaScript resize image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto resize_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto destination = JS_ToCString(context, argv[1]);
			auto percentage = JS::Converter::get_float32(context, argv[2]);
			Sen::Kernel::Definition::ImageIO::resize_png(source, destination, percentage);
			JS_FreeCString(context, source);
			JS_FreeCString(context, destination);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JavaScript rotate image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto rotate_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto destination = JS_ToCString(context, argv[1]);
			auto percentage = JS::Converter::get_float64(context, argv[2]);
			Sen::Kernel::Definition::ImageIO::rotate_png(source, destination, percentage);
			JS_FreeCString(context, source);
			JS_FreeCString(context, destination);
			return JS::Converter::get_undefined();
		}


	}

	/**
	 * JavaScript Engine
	*/

	namespace JavaScript {
		/**
		 * ----------------------------------------
		 * JavaScript evaluate method
		 * @param argv[0]: JS code to execute
		 * @return: JS value after evaluate
		 * ----------------------------------------
		*/

		inline static auto evaluate(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto m_value = JS_Eval(context, source, strlen(source), "unknown", JS_EVAL_TYPE_GLOBAL);
			JS_FreeCString(context, source);
			return m_value;
		}

		/**
		 * ----------------------------------------
		 * JavaScript evaluate as file
		 * @param argv[0]: source file
		 * @return: JS Value after evaluate
		 * ----------------------------------------
		*/

		inline static auto evaluate_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto js_source = Sen::Kernel::FileSystem::read_file(source);
			auto m_value = JS_Eval(context, js_source.c_str(), js_source.size(), source, JS_EVAL_TYPE_GLOBAL);
			JS_FreeCString(context, source);
			return m_value;
		}
	}

	/**
	 * JavaScript Encryption System
	*/

	namespace Encryption {

		/**
		 * JavaScript Encryption System Supportive of MD5
		*/

		namespace MD5 {
			/**
			 * ----------------------------------------
			 * JavaScript MD5 Hash method
			 * @param argv[0]: source hash
			 * @returns: hashed string
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto str = std::string{source};
				auto result = Kernel::Definition::Encryption::MD5::hash(static_cast<std::span<unsigned char>>(String::convertStringToSpan<unsigned char>(str)));
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript MD5 Hash file method
			 * @param argv[0]: source file
			 * @returns: hashed string
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::MD5::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}
		}

		/**
		 * JavaScript Base64 Encode/Decode System
		*/

		namespace Base64 {

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded string
			 * ----------------------------------------
			*/

			inline static auto encode(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::Base64::encode(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Decode method
			 * @param argv[0]: source to decode
			 * @returns: Decoded string
			 * ----------------------------------------
			*/

			inline static auto decode(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::Base64::decode(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto encode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Encryption::Base64::encode_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto decode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Encryption::Base64::decode_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}
		}

		/**
		 * JavaScript SHA224 Hash 
		*/

		namespace SHA224 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA224 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::Sha224::hash(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA224 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::Sha224::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

		}

		/**
		 * JavaScript SHA256 Support
		*/

		namespace SHA256 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA256 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA256::hash(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA256 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA256::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}
		}

		/**
		 * JavaScript SHA384 hash Support
		*/

		namespace SHA384 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA384 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA384::hash(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA384 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA384::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}
		}

		/**
		 * JavaScript SHA512 hash Support
		*/

		namespace SHA512 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA512 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA512::hash(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA512 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA512::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}
		}

		/**
		 * JavaScript XOR Encryption
		*/

		namespace XOR {
			/**
			 * ----------------------------------------
			 * JavaScript XOR Encryption
			 * @param argv[0]: key
			 * @param argv[1]: plain
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto encrypt(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto plain = JS_ToCString(context, argv[0]);
				auto key = JS_ToCString(context, argv[1]);
				auto result = Sen::Kernel::Definition::Encryption::XOR::encrypt(plain, key);
				JS_FreeCString(context, plain);
				JS_FreeCString(context, key);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript XOR Encryption File System
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: key
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto encrypt_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				auto key = JS_ToCString(context, argv[2]);
				Sen::Kernel::Definition::Encryption::XOR::encrypt_fs(source, destination, key);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				JS_FreeCString(context, key);
				return JS::Converter::get_undefined();
			}

		}
	}

	/**
	 * JavaScript Compression Support
	*/

	namespace Compression {

		/**
		 * JavaScript Zlib Support
		*/

		namespace Zlib {
			/**
			 * ----------------------------------------
			 * JavaScript Zlib Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: level
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				auto level = JS::Converter::get_int32(context, argv[2]);
				if(!(Sen::Kernel::Definition::Compression::Zlib::Level::DEFAULT <= level or level <= Sen::Kernel::Definition::Compression::Zlib::Level::LEVEL_9)){
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					throw std::invalid_argument(fmt::format("Invalid zlib level, expected level from 0 to 9, received {}", level));
				}
				Sen::Kernel::Definition::Compression::Zlib::compress_fs(source, destination, static_cast<Sen::Kernel::Definition::Compression::Zlib::Level>(level));
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JavaScript Zlib Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Zlib::uncompress_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}
			
		}

		/**
		 * JavaScript Bzip2 Support
		*/

		namespace Bzip2 {
			/**
			 * ----------------------------------------
			 * JavaScript Bzip2 Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: level
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Bzip2::compress_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JavaScript Bzip2 Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Bzip2::uncompress_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}
			
		}

		/**
		 * JavaScript Lzma Support
		*/

		namespace Lzma {
			/**
			 * ----------------------------------------
			 * JavaScript Lzma Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: level
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Lzma::compress_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JavaScript Lzma Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: actual size
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				auto actual_size = JS::Converter::get_uint64(context, argv[2]);
				Sen::Kernel::Definition::Compression::Lzma::uncompress_fs(source, destination, actual_size);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}
			
		}

		/**
		 * JavaScript Gzip Compression Support
		*/

		namespace Gzip {
			/**
			 * ----------------------------------------
			 * JavaScript Gzip Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Zlib::compress_gzip_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JavaScript Gzip Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Zlib::uncompress_gzip_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}
		}
	}

	/**
	 * JavaScript Support
	*/

	namespace Support {

		/**
		 * JavaScript Texture Encode & Decode
		*/

		namespace Texture {

				/**
				 * ----------------------------------------
				 * JavaScript Texture Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: width
				 * @param argv[3]: height
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 5, fmt::format("argument expected {} but received {}", 5, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto width = JS::Converter::get_int32(context, argv[2]);
					auto height = JS::Converter::get_int32(context, argv[3]);
					auto format = JS::Converter::get_int32(context, argv[4]);
					Sen::Kernel::Support::Texture::InvokeMethod::decode_fs(source, destination, width, height, static_cast<Sen::Kernel::Support::Texture::Format>(format));
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript Texture Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto format = JS::Converter::get_int32(context, argv[2]);
					Sen::Kernel::Support::Texture::InvokeMethod::encode_fs(source, destination, static_cast<Sen::Kernel::Support::Texture::Format>(format));
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

		}

		/**
		 * JavaScript PopCap Support
		*/

		namespace PopCap {

				/**
			 * JavaScript Zlib Support
			*/

			namespace Zlib {
				/**
				 * ----------------------------------------
				 * JavaScript Zlib Compression File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: use_64_bit_variant
				 * @returns: Compressed file
				 * ----------------------------------------
				*/

				inline static auto compress_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto use_64_bit_variant = JS::Converter::get_bool(context, argv[2]);
					Sen::Kernel::Support::PopCap::Zlib::Compress::compress_fs(source, destination, use_64_bit_variant);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript Zlib Uncompression File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto uncompress_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto use_64_bit_variant = JS::Converter::get_bool(context, argv[2]);
					Sen::Kernel::Support::PopCap::Zlib::Compress::compress_fs(source, destination, use_64_bit_variant);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
				
			}

			/**
			 * JavaScript Newton Support
			*/

			namespace CompiledText {

				/**
				 * ----------------------------------------
				 * JavaScript Compiled Text Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: key
				 * @param argv[3]: iv
				 * @param argv[4]: use 64bit variant
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 5, fmt::format("argument expected {} but received {}", 5, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto key = JS_ToCString(context, argv[2]);
					auto iv = JS_ToCString(context, argv[3]);
					auto use_64_bit_variant = JS::Converter::get_bool(context, argv[4]);
					Sen::Kernel::Support::PopCap::CompiledText::Decode::process_fs(source, destination, key, iv, use_64_bit_variant);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					JS_FreeCString(context, key);
					JS_FreeCString(context, iv);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript Compiled Text Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 5, fmt::format("argument expected {} but received {}", 5, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto key = JS_ToCString(context, argv[2]);
					auto iv = JS_ToCString(context, argv[3]);
					auto use_64_bit_variant = JS::Converter::get_bool(context, argv[4]);
					Sen::Kernel::Support::PopCap::CompiledText::Encode::process_fs(source, destination, key, iv, use_64_bit_variant);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

			}
			

			/**
			 * JavaScript Resource Group Support
			*/

			namespace ResourceGroup {

				/**
				 * ----------------------------------------
				 * JavaScript Resource Group Split File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Splitted file
				 * ----------------------------------------
				*/

				inline static auto split_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::ResourceGroup::BasicConversion::split(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript Resource Group Merge File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Merged file
				 * ----------------------------------------
				*/

				inline static auto merge_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::ResourceGroup::BasicConversion::merge(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript Resource Group Convert File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: path style
				 * @returns: Converted file
				 * ----------------------------------------
				*/

				inline static auto convert_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto path_style = JS::Converter::get_int32(context, argv[2]);
					Sen::Kernel::Support::PopCap::ResourceGroup::Convert::convert_fs(source, destination, static_cast<Sen::Kernel::Support::PopCap::ResourceGroup::PathStyle>(path_style));
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}


			}

			/**
			 * JavaScript Res Info Support
			*/

			namespace ResInfo {

				/**
				 * ----------------------------------------
				 * JavaScript ResInfo Split File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Splitted file
				 * ----------------------------------------
				*/

				inline static auto split_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::ResInfo::BasicConversion::split_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript ResInfo Merge File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Merged file
				 * ----------------------------------------
				*/

				inline static auto merge_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::ResInfo::BasicConversion::merge_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript ResInfo Convert File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Converted file
				 * ----------------------------------------
				*/

				inline static auto convert_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::ResInfo::Convert::convert_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}


			}

			/**
			 * JavaScript RenderEffects Support
			*/

			namespace RenderEffects {

				/**
				 * ----------------------------------------
				 * JavaScript RenderEffects Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::RenderEffects::Decode::process_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript RenderEffects Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::RenderEffects::Encode::process_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}


			}

			/**
			 * JavaScript CharacterFontWidget2 Support
			*/

			namespace CharacterFontWidget2 {

				/**
				 * ----------------------------------------
				 * CFW2 Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::CFW2::Decode::process_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * CFW2 Encode
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encrypted file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::CFW2::Encode::process_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}


			}

			/**
			 * JavaScript Crypt-Data Support
			*/

			namespace CryptData {

				/**
				 * ----------------------------------------
				 * JavaScript RenderEffects Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decrypt_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto key = JS_ToCString(context, argv[2]);
					Sen::Kernel::Support::PopCap::CryptData::Decrypt::process_fs(source, destination, key);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					JS_FreeCString(context, key);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript Crypt-Data Encrypt
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: key
				 * @returns: Encrypted file
				 * ----------------------------------------
				*/

				inline static auto encrypt_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					auto key = JS_ToCString(context, argv[2]);
					Sen::Kernel::Support::PopCap::CryptData::Encrypt::process_fs(source, destination, key);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					JS_FreeCString(context, key);
					return JS::Converter::get_undefined();
				}


			}

			/**
			 * JavaScript Newton Support
			*/

			namespace Newton {

				/**
				 * ----------------------------------------
				 * JavaScript Newton Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::Newton::Decode::process_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript Newton Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::Newton::Encode::process_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

			}

			/**
			 * JavaScript RTON Support
			*/
			
			namespace RTON {
				/**
				 * ----------------------------------------
				 * JavaScript RTON Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::RTON::Decode::decode_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Decode File as Threads
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs_as_multiple_threads(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					auto paths = std::vector<std::vector<std::string>>{};
					for (const auto & i : Range<int>(argc)) {
						const auto & data = JS::Converter::get_vector<std::string>(context, argv[i]);
						try_assert(data.size() == 2, fmt::format("argument expected {} but received {}", 2, data.size()));
						paths.emplace_back(data);
					}
					Sen::Kernel::Support::PopCap::RTON::Decode::decode_fs_as_multiple_threads(paths);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::RTON::Encode::encode_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}

			/**
			 * JavaScript RSB Support
			*/

			namespace RSB {
				/**
				 * ----------------------------------------
				 * JavaScript RSB Unpack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Unpacked file
				 * ----------------------------------------
				*/

				inline static auto unpack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSB Pack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Packed file
				 * ----------------------------------------
				*/

				inline static auto pack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}

			/**
			 * JavaScript RSG Support
			*/

			namespace RSG {
				/**
				 * ----------------------------------------
				 * JavaScript RSG Unpack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Unpacked file
				 * ----------------------------------------
				*/

				inline static auto unpack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSG Pack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Packed
				 *  file
				 * ----------------------------------------
				*/

				inline static auto pack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}

			/**
			 * JavaScript PAM Supportive
			*/

			namespace Animation {
				/**
				 * ----------------------------------------
				 * JavaScript PAM Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript PAM Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}
		}

		/**
		 * JavaScript WWise Support
		*/

		namespace WWise {

			/**
			 * JavaScript SoundBank Support
			*/

			namespace SoundBank {
				/**
				 * ----------------------------------------
				 * JavaScript WWise Soundbank Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript WWise Soundbank Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}
		}
	}

	/**
	 * JavaScript Custom JSON Serializer
	*/

	namespace JSON {

		/**
		 * Convert nlohmann::ordered_json to JSValue for quickjs
		*/

		inline static auto json_to_js_value(
			JSContext *context, 
			const nlohmann::ordered_json &json
		) -> JSValue
		{
			switch (json.type()) {
				case nlohmann::ordered_json::value_t::null:{
					return JS::Converter::get_null();
				}
				case nlohmann::ordered_json::value_t::object: {
					auto js_obj = JS_NewObject(context);
					for (auto & [key, value] : json.items()) {
						JS_DefinePropertyValueStr(context, js_obj, key.c_str(), json_to_js_value(context, value), JS_PROP_C_W_E);
					}
					return js_obj;
				}
				case nlohmann::ordered_json::value_t::array: {
					auto js_arr = JS_NewArray(context);
					for (auto i : Range<size_t>(json.size())) {
						JS_DefinePropertyValueUint32(context, js_arr, i, json_to_js_value(context, json[i]), JS_PROP_C_W_E);
					}
					return js_arr;
				}
				case nlohmann::ordered_json::value_t::string:{
					return JS_NewString(context, json.get<std::string>().c_str());
				}
				case nlohmann::ordered_json::value_t::boolean:{
					return JS_NewBool(context, json.get<bool>());
				}
				case nlohmann::ordered_json::value_t::number_integer:{
					return JS_NewBigInt64(context, json.get<int64_t>());
				}
				case nlohmann::ordered_json::value_t::number_unsigned:{
					return JS_NewBigInt64(context, json.get<uint64_t>());
				}
				case nlohmann::ordered_json::value_t::number_float:{
					return JS_NewFloat64(context, json.get<double>());
				}
				default:{
					return JS::Converter::get_undefined();
				}
			}
		}
		
		/**
		 * ----------------------------------------
		 * JavaScript JSON Deserializer
		 * @param argv[0]: JS String
		 * @returns: Deserialized object
		 * ----------------------------------------
		*/

		inline static auto deserialize(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto json = nlohmann::ordered_json::parse(source);
			auto js_obj = json_to_js_value(context, json);
			JS_FreeCString(context, source);
			return js_obj;
		}

		/**
		 * ----------------------------------------
		 * JavaScript JSON Deserializer
		 * @param argv[0]: JS String
		 * @returns: Deserialized object
		 * ----------------------------------------
		*/

		inline static auto deserialize_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto json = nlohmann::ordered_json::parse(Sen::Kernel::FileSystem::read_file(source));
			auto js_obj = json_to_js_value(context, json);
			JS_FreeCString(context, source);
			return js_obj;
		}

		/**
		 * QuickJS JSON Value to nlohmann::ordered_json and then to string
		*/

		inline static auto js_object_to_json(
			JSContext *context, 
			JSValueConst value
		) -> nlohmann::ordered_json
		{
			switch (JS_VALUE_GET_TAG(value)) {
				case JS_TAG_UNDEFINED:{
					return nullptr;
				}
				case JS_TAG_NULL:{
					return nlohmann::ordered_json(nullptr);
				}
				case JS_TAG_OBJECT: {
					if (JS_IsArray(context, value)) {
						auto json = nlohmann::ordered_json::array();
						auto length = uint32_t{};
						JS_ToUint32(context, &length, JS_GetPropertyStr(context, value, "length"));
						for (auto i : Range<uint32_t>(length)) {
							auto val = JS_GetPropertyUint32(context, value, i);
							json.push_back(js_object_to_json(context, val));
							JS_FreeValue(context, val);
						}
						return json;
					} 
					else if (JS_IsObject(value)) {
						auto json = nlohmann::ordered_json::object();
						auto *tab = static_cast<JSPropertyEnum*>(nullptr);
						auto tab_size = uint32_t{};
						if (JS_GetOwnPropertyNames(context, &tab, &tab_size, value, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) == 0) {
							for (auto i : Range<uint32_t>(tab_size)) {
								auto key = JS_AtomToCString(context, tab[i].atom);
								auto val = JS_GetProperty(context, value, tab[i].atom);
								if(JS_VALUE_GET_TAG(val) != JS_TAG_UNDEFINED){	
									json[key] = js_object_to_json(context, val);
								}
								JS_FreeAtom(context, tab[i].atom);
								JS_FreeValue(context, val);
							}
							js_free(context, tab);
						}
						return json;
					}
					else{
						throw std::runtime_error("Unknown type");
					}
				}
				case JS_TAG_STRING: {
					auto str = JS_ToCString(context, value);
					auto json = nlohmann::ordered_json(str);
					JS_FreeCString(context, str);
					return json;
				}
				case JS_TAG_BOOL:{
					return nlohmann::ordered_json(JS_VALUE_GET_BOOL(value));
				}
				case JS_TAG_INT:{
					return nlohmann::ordered_json((double) JS_VALUE_GET_INT(value));
				}
				case JS_TAG_FLOAT64:{
					return nlohmann::ordered_json(JS_VALUE_GET_FLOAT64(value));
				}
				case JS_TAG_BIG_INT: {
					auto val = int64_t{};
					JS_ToBigInt64(context, &val, value);
					return nlohmann::ordered_json(val);
				}
			}
		}

		/**
		 * ----------------------------------------
		 * JavaScript JSON Serializer
		 * @param argv[0]: JSValue
		 * @param argv[1]: indent
		 * @param argv[2]: char
		 * @param argv[3]: ensure ascii
		 * @returns: Serialized object
		 * ----------------------------------------
		*/

		inline static auto serialize(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
			auto json = js_object_to_json(context, argv[0]);
			auto indent = JS::Converter::get_int32(context, argv[1]);
			auto ensure_ascii = JS::Converter::get_bool(context, argv[2]);;
			auto source = json.dump(indent, '\t', ensure_ascii);
			auto js_val = JS_NewString(context, source.c_str());
			return js_val;
		}

		/**
		 * ----------------------------------------
		 * JavaScript JSON Serializer
		 * @param argv[0]: Destination
		 * @param argv[1]: json
		 * @param argv[2]: indent
		 * @param argv[3]: ensure ascii
		 * @returns: Serialized object
		 * ----------------------------------------
		*/

		inline static auto serialize_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 4, fmt::format("argument expected {} but received {}", 4, argc));
			auto destination = JS_ToCString(context, argv[0]);
			auto json = js_object_to_json(context, argv[1]);
			auto indent = JS::Converter::get_int32(context, argv[2]);
			auto ensure_ascii = JS::Converter::get_bool(context, argv[3]);;
			auto result = json.dump(indent, '\t', ensure_ascii);
			Sen::Kernel::FileSystem::write_file(destination, result);
			JS_FreeCString(context, destination);
			return JS::Converter::get_undefined();
		}
	}

}