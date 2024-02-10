#include "Shell.h"
#define DEBUG true

inline auto static get_line(

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
    #if WINDOWS
       SetConsoleTextAttribute(hConsole, Sen::Shell::Interactive::Color::DEFAULT);
    #endif
   if (message != "") {
      std::cout << message << std::endl;
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

inline static auto callback(
    CStringList list
) -> CStringView
{
    auto result = StringList::to_vector(list);
    assert_conditional(result.size() >= 1, "argument must be greater than 1");
    if (result[0] == "display") {
        assert_conditional(result.size() >= 2, "argument must be greater than 2");
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
        delete[] copy;
        copy = nullptr;
        auto c = get_line();
        copy = new char[c.size() + 1];
        std::strcpy(copy, c.c_str());
        return CStringView{
            .size = c.size(),
            .value = copy
        };
    }
    if (result[0] == "is_gui") {
        return CStringView{ .size = 1, .value = "0" };
    }
    if (result[0] == "wait") {
        #if WINDOWS
                auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, Sen::Shell::Interactive::Color::CYAN);
                std::cout << "● ";
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
        std::strcpy(copy, version.c_str());
        return CStringView{ .size = version.size(), .value = copy };
    }
    return EMPTY_STRING_VIEW;
}

MAIN_FUNCTION
{
    #if WINDOWS
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    std::setlocale(LC_ALL, "C");
    auto kernel = std::string{};
    auto script = std::string{};
    if (size >= 2) {
        kernel = Sen::Shell::utf16_to_utf8(argc[1]);
    }
    else {
    #if WINDOWS
            kernel = Sen::Shell::utf16_to_utf8(std::filesystem::absolute(KERNEL_DEFAULT).generic_wstring());
    #else
            kernel = std::filesystem::absolute(KERNEL_DEFAULT).generic_string();
    #endif
    }
    if (size >= 3) {
        script = Sen::Shell::utf16_to_utf8(argc[2]);
    }
    else {
        #if WINDOWS
                script = Sen::Shell::utf16_to_utf8(std::filesystem::absolute("./Script/main.js").generic_wstring());
        #else
                script = std::filesystem::absolute("./Script/main.js").generic_string();
        #endif
    }
    #if WIN32
        auto hinstLib = LoadLibrary(TEXT(kernel.c_str()));
    #else
        auto hinstLib = dlopen(kernel.c_str(), RTLD_LAZY);
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
        #if WINDOWS
                FreeLibrary(hinstLib);
        #else
                dlclose(hinstLib);
        #endif
        return 1;
    }
    auto script_pointer = std::make_unique<CStringView>(script.size(), script.data());
    auto argument_size = static_cast<uint64_t>(size);
    auto argument_list = new CStringList{ .value = new CStringView[argument_size], .size = argument_size };
    for (auto i = 0; i < size; ++i) {
        #if WINDOWS
                auto argument_value = Sen::Shell::utf16_to_utf8(reinterpret_cast<char16_t const*>(argc[i]));
                auto value_copy = new char[argument_value.size() + 1];
                std::memcpy(value_copy, argument_value.c_str(), argument_value.size());
                (argument_list)->value[i].value = value_copy;
                (argument_list)->value[i].size = argument_value.size();
        #else
                (argument_list)->value[i].value = argc[i];
                (argument_list)->value[i].size = strlen(argc[i]);
        #endif
    }
    auto result = execute_method(script_pointer.get(), argument_list, callback);
    delete[] copy;
    for (auto i = 0; i < size; ++i) {
        delete[] argument_list->value[i].value;
    }
    delete[] argument_list->value;
    delete argument_list;
    #if WIN32
        FreeLibrary(hinstLib);
    #else
        dlclose(hinstLib);
    #endif
    return result;
}
