#include "library/callback.hpp"

using namespace Sen::Shell;

MAIN_FUNCTION
{
    auto result = 1;
    try {
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
                kernel = Sen::Shell::utf16_to_utf8(std::filesystem::absolute(KERNEL_DEFAULT).wstring());
        #else
                kernel = std::filesystem::absolute(KERNEL_DEFAULT).string();
        #endif
        }
        if (size >= 3) {
            script = Sen::Shell::utf16_to_utf8(argc[2]);
        }
        else {
            #if WINDOWS
                    script = Sen::Shell::utf16_to_utf8(std::filesystem::absolute("./Script/main.js").wstring());
            #else
                    script = std::filesystem::absolute("./Script/main.js").string();
            #endif
        }
        #if WIN32
            auto hinstLib = LoadLibrary(TEXT(kernel.c_str()));
        #else
            auto hinstLib = dlopen(kernel.c_str(), RTLD_LAZY);
        #endif
        if (hinstLib == NULL) {
            Console::print("Kernel cannot be loaded", "", Sen::Shell::Interactive::Color::RED);
            return 1;
        }
        #if WIN32
            auto execute_method = (execute)GetProcAddress(hinstLib, "execute");
        #else
            auto execute_method = (execute)dlsym(hinstLib, "execute");
        #endif
        if (execute_method == NULL) {
            Console::print("Method not found", "", Sen::Shell::Interactive::Color::RED);
            #if WINDOWS
                    FreeLibrary(hinstLib);
            #else
                    dlclose(hinstLib);
            #endif
            return 1;
        }
        auto script_pointer = std::make_unique<CStringView>(static_cast<int>(script.size()), script.data());
        auto argument_size = static_cast<int>(size);
        auto argument_list = new CStringList{
            .value = new CStringView[argument_size],
            .size = argument_size,
        };
        for (auto i = 0; i < size; ++i) {
            switch (i) {
                case 1: {
                    argument_list->value[i].value = kernel.c_str();
                    argument_list->value[i].size = kernel.size();
                    break;
                }
                case 2: {
                    argument_list->value[i].value = script.c_str();
                    argument_list->value[i].size = script.size();
                    break;
                }
                default: {
                    #if WINDOWS
                        auto argument_value = Sen::Shell::utf16_to_utf8(reinterpret_cast<char16_t const*>(argc[i]));
                        auto value_copy = new char[argument_value.size() + 1];
                        std::memcpy(value_copy, argument_value.c_str(), argument_value.size());
                        argument_list->value[i].value = value_copy;
                        argument_list->value[i].size = argument_value.size();
                    #else
                        argument_list->value[i].value = argc[i];
                        argument_list->value[i].size = strlen(argc[i]);
                    #endif
                };
            }
        }
        result = execute_method(script_pointer.get(), argument_list, callback);
        delete[] copy;
        #if WINDOWS
            for (auto i = 3; i < size; ++i) {
                delete[] argument_list->value[i].value;
            }
        #endif
        delete[] argument_list->value;
        delete argument_list;
        #if WIN32
            FreeLibrary(hinstLib);
        #else
            dlclose(hinstLib);
        #endif
    }
    catch (std::runtime_error &e) {
        Console::print("exception found", e.what());
    }
    catch (...) {
        Console::print("exception found", "");
    }
    return result;
}
