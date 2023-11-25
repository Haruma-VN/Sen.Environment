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
    char const* message
) -> void
{
    std::cout << message << std::endl;
}

inline auto static printSubmessage(
    char const* message
) -> void
{
    std::cout << message;
}

MAIN_FUNCTION
{
    #if WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    std::setlocale(LC_ALL, "C");
    auto hinstLib = LoadLibrary(TEXT("D:/Code/Sen.Environment/Kernel/build/kernel/Release/kernel.dll"));
    if (hinstLib == NULL) {
        print("Unable to load DLL!");
        return 1;
    }
    auto execute = (KernelExecute)GetProcAddress(hinstLib, "execute");
    if (execute == NULL) {
        print("Unable to load function!");
        FreeLibrary(hinstLib);
        return 1;
    }
    auto* argument = new Argument{};
    auto* parameter = new Parameter{};
    execute(argument, parameter, print, printSubmessage, getLine);
    FreeLibrary(hinstLib);
    delete argument;
    delete parameter;
    return 0;
}
