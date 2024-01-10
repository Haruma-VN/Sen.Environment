#include "Shell.h"
#define DEBUG true

inline auto static get_line(

) -> CStringView
{
    auto str = std::string{};
    std::getline(std::cin, str);
    return CStringView{
        .value = str.c_str(), 
        .size = str.size()
    };
}

inline auto static print(
    const std::string & title,
    const std::string & message = std::string{""},
    const Sen::Shell::Interactive::Color color = Sen::Shell::Interactive::Color::DEFAULT
) -> void
{
    #if WIN32
        auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
        std::cout << title << std::endl;
    #else
        switch (color) {
            case Sen::Shell::Interactive::Color::RED: {
                std::cout << "\033[31m" << title << "\033[0m" << std::endl;
                break;
            }
            case Sen::Shell::Interactive::Color::GREEN: {
                std::cout << "\033[32m" << title << "\033[0m" << std::endl;
                break;
            }
            case Sen::Shell::Interactive::Color::CYAN: {
                std::cout << "\033[36m" << title << "\033[0m" << std::endl;
                break;
            }
            case Sen::Shell::Interactive::Color::YELLOW: {
                std::cout << "\033[33m" << title << "\033[0m" << std::endl;
                break;
            case Sen::Shell::Interactive::Color::DEFAULT: {
                std::cout << title << std::endl;
                break;
            }
            default: {
                throw std::runtime_error("Unknown color");
            }
        }
    #endif
    #if WIN32
       SetConsoleTextAttribute(hConsole, Sen::Shell::Interactive::Color::DEFAULT);
    #endif
   if (message != "") {
      std::cout << message << std::endl;
   }
    return;
}

#define EMPTY_STRING_VIEW CStringView{.value = "", .size = 0};

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

inline static auto callback(
    CStringList list
) -> CStringView
{
    auto result = StringList::to_vector(list);
    try_assert(result.size() >= 1, "argument must be greater than 1");
    if (result[0] == "display") {
        try_assert(result.size() >= 2, "argument must be greater than 2");
        switch (result.size()) {
            case 2: {
                print(result[1]);
                break;
            }
            case 3: {
                print(result[1], result[2]);
                break;
            }
            case 4: {
                print(result[1], result[2], convert_color(result[3]));
                break;
            }
        }
        return EMPTY_STRING_VIEW;
    }
    if (result[0] == "input") {
        return get_line();
    }
    if (result[0] == "is_gui") {
        return CStringView{ .value = "0", .size = 1 };
    }
    if (result[0] == "version") {
        auto version = std::to_string(Sen::Shell::version);
        return CStringView{ .value = "1", .size = version.size()};
    }
    return EMPTY_STRING_VIEW;
}

MAIN_FUNCTION
{
    try {
        try_assert(size >= 3, "argument too few, expected: arg.size >= 3");
    }
    catch (const std::runtime_error& e) {
        print(e.what(), "", Sen::Shell::Interactive::Color::RED);
        get_line();
        return 1;
    }
    #if WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    std::setlocale(LC_ALL, "C");
    #if WIN32
        auto hinstLib = LoadLibrary(TEXT(argc[1]));
    #else
        auto hinstLib = dlopen(argc[1], RTLD_LAZY);
    #endif
    if (hinstLib == NULL) {
        print("Kernel cannot be loaded", "", Sen::Shell::Interactive::Color::RED);
        return 1;
    }
    #if WIN32
        auto execute_method = (execute)GetProcAddress(hinstLib, "execute");
    #else
        auto execute_method = (execute)dlsym(hinstLib, "execute");
    #endif
    if (execute_method == NULL) {
        print("Method not found", "", Sen::Shell::Interactive::Color::RED);
        #if WIN32
                FreeLibrary(hinstLib);
        #else
                dlclose(hinstLib);
        #endif
        return 1;
    }
    auto argument = std::string{ argc[2] };
    auto script = std::unique_ptr<CStringView>(new CStringView{
        .value = argument.data(),
        .size = argument.size()
        });
    auto result = execute_method(script.get(), callback);
    #if WIN32
        FreeLibrary(hinstLib);
    #else
        dlclose(hinstLib);
    #endif
    return result;
}
