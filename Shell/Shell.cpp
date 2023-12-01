#include "Shell.h"
#define DEBUG true

inline auto static getLine(

) -> BasicStringView
{
    auto str = std::string{};
    std::getline(std::cin, str);
    auto m = BasicStringView{};
    strcpy_s(m.data, str.c_str());
    m.size = str.size();
    return m;
}

inline auto static print(
    char const* title,
    char const* message,
    const Sen::Shell::Interactive::Color color
) -> void
{
    #if WIN32
        auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
        std::cout << title;
    #else
        switch (color) {
            case Sen::Shell::Interactive::Color::RED: {
                std::cout << "\033[31m" << title << "\033[0m";
                break;
            }
            case Sen::Shell::Interactive::Color::GREEN: {
                std::cout << "\033[32m" << title << "\033[0m";
                break;
            }
            case Sen::Shell::Interactive::Color::CYAN: {
                std::cout << "\033[36m" << title << "\033[0m";
                break;
            }
            case Sen::Shell::Interactive::Color::YELLOW: {
                std::cout << "\033[33m" << title << "\033[0m";
                break;
            }
            default: {
                throw std::runtime_error("Unknown color");
            }
        }
    #endif
    #if WIN32
       SetConsoleTextAttribute(hConsole, 7);
    #endif
    std::cout << message << std::endl;
    return;
}

MAIN_FUNCTION
{
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
        auto execute = (KernelExecute)GetProcAddress(hinstLib, "execute");
    #else
        auto execute = (KernelExecute)dlsym(hinstLib, "execute");
    #endif
    if (execute == NULL) {
        print("Method not found", "", Sen::Shell::Interactive::Color::RED);
        #if WIN32
                FreeLibrary(hinstLib);
        #else
                dlclose(hinstLib);
        #endif
        return 1;
    }
    auto path1 = (std::string)"D:/test/mRes.bin";
    auto path2 = (std::string)"D:/test/mRes.test.json";
    auto* argument = new BasicStringView{};
    strcpy_s(argument->data, path1.c_str());
    argument->size = path1.size();
    auto* parameter = new Parameter{};
    strcpy_s(parameter->data[0].data, path2.c_str());
    parameter->data[0].size = path2.size();
    auto result = execute(argument, parameter, print, getLine, Sen::Shell::CliCallBack::ZLIB_UNCOMPRESS);
    #if WIN32
        FreeLibrary(hinstLib);
    #else
        dlclose(hinstLib);
    #endif
    delete argument;
    delete parameter;
    return result;
}
