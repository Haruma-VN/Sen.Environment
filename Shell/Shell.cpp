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
    return;
}

inline auto static printSubmessage(
    char const* message
) -> void
{
    std::cout << message;
    return;
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
        print("Kernel cannot be loaded");
        return 1;
    }
    auto execute = (KernelExecute)GetProcAddress(hinstLib, "execute");
    if (execute == NULL) {
        print("Method not found");
        FreeLibrary(hinstLib);
        return 1;
    }
    auto path1 = (std::string)"D:/test/m.bin";
    auto path2 = (std::string)"D:/test/m.data";
    auto* argument = new BasicStringView{};
    strcpy_s(argument->data, path1.c_str());
    argument->size = path1.size();
    auto* parameter = new Parameter{};
    strcpy_s(parameter->data[0].data, path2.c_str());
    parameter->data[0].size = path2.size();
    auto result = execute(argument, parameter, print, printSubmessage, getLine, Sen::Shell::CliCallBack::BASE64_DECODE);
    FreeLibrary(hinstLib);
    delete argument;
    delete parameter;
    return result;
}
