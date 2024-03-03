#pragma once

#include "library/macro.hpp"
#include "library/dialog.hpp"

namespace Sen::Shell {
	
	namespace Console {

		inline auto static input(

		) -> std::string
		{
			auto str = std::string{};
			std::getline(std::cin, str);
			return str;
		}

		inline auto static print(
			const std::string & title,
			const std::string & message = std::string{""},
			const Sen::Shell::Interactive::Color color = Sen::Shell::Interactive::Color::DEFAULT
		) -> void
		{
			#if WINDOWS
				auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, color);
				std::cout << title << std::endl << std::flush;
			#else
				switch (color) {
					case Sen::Shell::Interactive::Color::RED: {
						std::cout << "\033[31m" << title << "\033[0m" << std::endl << std::flush;
						break;
					}
					case Sen::Shell::Interactive::Color::GREEN: {
						std::cout << "\033[32m" << title << "\033[0m" << std::endl << std::flush;
						break;
					}
					case Sen::Shell::Interactive::Color::CYAN: {
						std::cout << "\033[36m" << title << "\033[0m" << std::endl << std::flush;
						break;
					}
					case Sen::Shell::Interactive::Color::YELLOW: {
						std::cout << "\033[33m" << title << "\033[0m" << std::endl << std::flush;
						break;
					case Sen::Shell::Interactive::Color::DEFAULT: {
						std::cout << title << std::endl << std::flush;
						break;
					}
					default: {
						throw std::runtime_error("Unknown color");
					}
				}
			#endif
			#if WINDOWS
				SetConsoleTextAttribute(hConsole, Sen::Shell::Interactive::Color::DEFAULT);
			#endif
			if (message != "") {
				std::cout << message << '\n' << std::flush;
			}
				return;
			}

		#define EMPTY_STRING_VIEW CStringView{.size = 0, .value = ""};

		inline static auto convert_color(
			const std::string& that
		) -> Sen::Shell::Interactive::Color
		{
			if (that == "red") {
				return Sen::Shell::Interactive::Color::RED;
			}
			if (that == "green") {
				return Sen::Shell::Interactive::Color::GREEN;
			}
			if (that == "cyan") {
				return Sen::Shell::Interactive::Color::CYAN;
			}
			if (that == "yellow") {
				return Sen::Shell::Interactive::Color::YELLOW;
			}
			return Sen::Shell::Interactive::Color::DEFAULT;
		}
	}

	inline static auto callback(
		CStringList list,
		void* proxy
	) -> CStringView
	{
		auto result = StringList::to_vector(list);
		assert_conditional(result.size() >= 1, "argument must be greater than 1");
		if (result[0] == "display") {
			assert_conditional(result.size() >= 2, "argument must be greater than 2");
			switch (result.size()) {
				case 2: {
					Console::print(result[1]);
					break;
				}
				case 3: {
					Console::print(result[1], result[2]);
					break;
				}
				case 4: {
					Console::print(result[1], result[2], Console::convert_color(result[3]));
					break;
				}
			}
			return EMPTY_STRING_VIEW;
		}
		if (result[0] == "input") {
			delete[] copy;
			copy = nullptr;
			auto c = Console::input();
			copy = new char[c.size() + 1];
			std::memcpy(copy, c.c_str(), c.size());
			return CStringView{
				.size = c.size(),
				.value = copy,
			};
		}
		if (result[0] == "is_gui") {
			return CStringView{ 
				.size = 1, 
				.value = "0" ,
			};
		}
		if (result[0] == "wait") {
			#if WINDOWS
					auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(hConsole, Sen::Shell::Interactive::Color::CYAN);
					std::cout << "● " << std::flush;
			#else
					std::cout << "\033[36m● \033[0m";
			#endif
			return EMPTY_STRING_VIEW;
		}
		if (result[0] == "clear") {
			#if WINDOWS
					auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(hConsole, Sen::Shell::Interactive::Color::DEFAULT);
			#endif
			return EMPTY_STRING_VIEW;
		}
		if (result[0] == "version") {
			delete[] copy;
			copy = nullptr;
			auto version = std::to_string(Sen::Shell::version);
			copy = new char[version.size() + 1];
			std::memcpy(copy, version.c_str(), version.size());
			return CStringView{ 
				.size = version.size(), 
				.value = copy,
			};
		}
		if (result[0] == "host") {
			assert_conditional(result.size() >= 5, "argument must be greater than 5");
			auto svr = Server{};
			svr.Get(std::format("/{}", result[1]), [&](const Request&, Response& res) {
				res.set_content(result[2], "text/html");
			});
			svr.listen(result[3], std::stoi(result[4]));
			return EMPTY_STRING_VIEW;
		}
		if (result[0] == "pick_file") {
			delete[] copy;
			copy = nullptr;
			#if WINDOWS
				auto raw_selection = Dialog::pick_path(false, false);
				if (!raw_selection.empty()) {
					copy = new char[raw_selection[0].size() + 1];
					std::memcpy(copy, raw_selection[0].data(), raw_selection[0].size());
				}
			#elif LINUX || MACINTOSH
				auto raw_selection = tinyfd_openFileDialog("", nullptr, 0, nullptr, nullptr, false);
				if (raw_selection != nullptr) {
					copy = new char[strlen(raw_selection) + 1];
					std::memcpy(copy, raw_selection, strlen(raw_selection));
				}
				return CStringView{
					.size = strlen(raw_selection),
					.value = copy,
				}
			#endif
		}
		if (result[0] == "pick_directory") {
			delete[] copy;
			copy = nullptr;
			#if WINDOWS
			auto raw_selection = Dialog::pick_path(true, false);
			if (!raw_selection.empty()) {
				copy = new char[raw_selection[0].size() + 1];
				std::memcpy(copy, raw_selection[0].data(), raw_selection[0].size());
			}
			#elif LINUX || MACINTOSH
			auto raw_selection = tinyfd_selectFolderDialog("", nullptr);
			if (raw_selection != nullptr) {
				copy = new char[strlen(raw_selection) + 1];
				std::memcpy(copy, raw_selection, strlen(raw_selection));
			}
			return CStringView{
				.size = strlen(raw_selection),
				.value = copy,
			}
			#endif
		}
		if (result[0] == "push_notification") {
			assert_conditional(result.size() >= 3, "argument must be greater than 3");
			tinyfd_notifyPopup(result[1].data(), result[2].data(), "info");
		}
		return EMPTY_STRING_VIEW;
	}

}