#include "Shell.h"
#define DEBUG true

inline auto static getLine(

) -> BasicStringView
{
    auto str = std::string{};
    std::getline(std::cin, str);
    auto m = BasicStringView{str.c_str()};
    m.size = str.size();
    return m;
}

inline auto static print(
    char const* title,
    const Sen::Shell::Interactive::Color color
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
            
            case Sen::Shell::Interactive::Color::WHITE: {
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
    return;
}

MAIN_FUNCTION
{
    try {
        try_assert(size >= 3, "argument too few, expected: arg.size >= 3");
    }
    catch (const std::runtime_error& e) {
        print(e.what(), Sen::Shell::Interactive::Color::RED);
        input();
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
        print("Kernel cannot be loaded", Sen::Shell::Interactive::Color::RED);
        return 1;
    }
    #if WIN32
        auto execute_method = (execute)GetProcAddress(hinstLib, "execute");
    #else
        auto execute_method = (execute)dlsym(hinstLib, "execute");
    #endif
    if (execute_method == NULL) {
        print("Method not found", Sen::Shell::Interactive::Color::RED);
        #if WIN32
                FreeLibrary(hinstLib);
        #else
                dlclose(hinstLib);
        #endif
        return 1;
    }
    auto argument = std::unique_ptr<BasicStringView>(new BasicStringView{ argc[2] });
    auto parameter = std::unique_ptr<Parameter>(new Parameter{std::vector<std::string>{}});
    auto result = execute_method(argument.get(), parameter.get(), print, getLine, MShellAPI{Sen::Shell::version, false});
    #if WIN32
        FreeLibrary(hinstLib);
    #else
        dlclose(hinstLib);
    #endif
    return result;
}
