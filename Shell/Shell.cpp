#include "Shell.h"

int main(int size, char** argc) {
    auto hinstLib = LoadLibrary(TEXT(argc[0]));
    if (hinstLib == NULL) {
        std::cout << "Unable to load DLL!" << std::endl;
        return 1;
    }
    auto execute = (ExecuteFunction)GetProcAddress(hinstLib, "execute");
    if (execute == NULL) {
        std::cout << "Unable to load function!" << std::endl;
        FreeLibrary(hinstLib);
        return 1;
    }
    execute(size, argc);
    FreeLibrary(hinstLib);
    return 0;
}
