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
    #if WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    std::setlocale(LC_ALL, "C");
    auto kernel = std::string{};
    auto script = std::string{};
    if (size >= 2) {
        kernel = argc[1];
    }
    else {
        kernel = std::filesystem::absolute(KERNEL_DEFAULT).string();
    }
    if (size >= 3) {
        script = argc[2];
    }
    else {
        script = std::filesystem::absolute("./Script/main.js").string();
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
        #if WIN32
                FreeLibrary(hinstLib);
        #else
                dlclose(hinstLib);
        #endif
        return 1;
    }
    auto script_pointer = std::unique_ptr<CStringView>(new CStringView{
        .size = script.size(),
        .value = script.data(),
    });
    auto argument_size = static_cast<uint64_t>(size);
    auto argument_list = std::unique_ptr<CStringList>(new CStringList{
         .value = new CStringView[argument_size],
         .size = argument_size,
    });
    for (auto i = 0; i < size; i++) {
       (argument_list.get())->value[i].value = argc[i];
       (argument_list.get())->value[i].size = strlen(argc[i]);
    }
    auto result = execute_method(script_pointer.get(), argument_list.get(), callback);
    delete[] copy;
    delete[] argument_list->value;
    #if WIN32
        FreeLibrary(hinstLib);
    #else
        dlclose(hinstLib);
    #endif
    return result;
}
