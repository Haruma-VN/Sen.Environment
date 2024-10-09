#include "library/download.hpp"
#include "library/string.hpp"
#include "library/zip.hpp"

#if _WIN32
#define MAIN int wmain(int argc, wchar_t* argv[])
#else
#define MAIN int main(int argc, char* argv[])
#endif

#ifdef _WIN32
auto get_executable_path(
) -> std::wstring 
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    auto execPath = std::wstring(path);
    auto pos = execPath.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        execPath = execPath.substr(0, pos);
    }
    return execPath;
}


auto execute_windows_command(
    auto& command,
    std::wstring& arguments
) -> void
{
    auto processInfo = PROCESS_INFORMATION{};
    ZeroMemory(&processInfo, sizeof(processInfo));
    auto startupInfo = STARTUPINFOW{};
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    auto fullCommand = command + L" " + arguments;

    if (!CreateProcessW(
        command.c_str(),
        const_cast<LPWSTR>(fullCommand.c_str()),
        nullptr,
        nullptr,
        TRUE,
        0,
        nullptr,
        nullptr,
        &startupInfo,
        &processInfo)
        ) {
        throw std::runtime_error{ "Failed to execute command via CreateProcessW" };
    }
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    return;
}

auto get_download_directory(
) -> std::string
{
    auto path = PWSTR{};
    auto hr = SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &path);
    if (SUCCEEDED(hr)) {
        char mbstr[MAX_PATH];
        auto convertedChars = size_t{ 0 };
        wcstombs_s(&convertedChars, mbstr, path, sizeof(mbstr));
        CoTaskMemFree(path); 
        return std::string(mbstr);
    }
    return "";
}
#else
auto get_executable_path(
) -> std::string {
    char path[PATH_MAX];
    auto count = readlink("/proc/self/exe", path, sizeof(path));
    if (count == -1) {
        throw std::runtime_error{ "Error retrieving executable path" };
        return "";
    }
    path[count] = '\0';
    auto execPath = std::string(path);
    auto pos = execPath.find_last_of("/\\");
    if (pos != std::string::npos) {
        execPath = execPath.substr(0, pos);
    }
    return execPath;
}
#endif


MAIN{
    try {
        #ifdef _WIN32
            auto home = get_executable_path();
            if (home.empty()) {
                throw std::runtime_error{"Failed to get the executable path."};
            }
            /*auto need_shell = true;
            auto need_script = true;
            auto need_kernel = true;
            auto shell_file = home + L"\\Shell.exe";
            if (std::filesystem::exists(shell_file)) {
                if (!std::filesystem::is_regular_file(shell_file)) {
                    std::filesystem::remove_all(shell_file);
                }
                else {
                    need_shell = false;
                }
            }
            auto script_file = home + L"\\Script\\main.js";
            if (std::filesystem::exists(script_file)) {
                if (!std::filesystem::is_regular_file(script_file)) {
                    std::filesystem::remove_all(script_file);
                }
                else {
                    need_script = false;
                }
            }
            auto kernel_file = home + L"\\Kernel.dll";
            if (std::filesystem::exists(kernel_file)) {
                if (!std::filesystem::is_regular_file(kernel_file)) {
                    std::filesystem::remove_all(kernel_file);
                }
                else {
                    need_kernel = false;
                }
            }
            if (need_kernel || need_script || need_shell) {
                std::wcout << L"The essential file does not exists, require to reinstall the tool" << std::endl;
                [&]() {
                    auto result = std::string{};
                    Download::fetch_github(result);
                    auto response = nlohmann::ordered_json::parse(result);
                    for (auto& e : response["assets"]) {
                        if (e["name"] == "win-x64.zip") {
                            auto result = utf16_to_utf8(home) + "\\win-x64.zip";
                            Download::download(e["browser_download_url"], result);
                            auto dest = get_download_directory() + "\\win-x64.zip";
                            std::filesystem::copy(utf8_to_utf16(result), utf8_to_utf16(dest));
                            auto directory = get_download_directory() + "\\sen.temporary";
                            if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
                                std::filesystem::remove_all(directory);
                            }
                            std::filesystem::create_directory(directory);
                            extract_zipfile(dest, directory);
                            std::filesystem::copy(utf8_to_utf16(directory), home, std::filesystem::copy_options::recursive |
                                std::filesystem::copy_options::overwrite_existing);
                            std::filesystem::remove_all(utf8_to_utf16(dest));
                            std::filesystem::remove_all(result);
                            std::filesystem::remove_all(directory);
                            break;
                        }
                    }
                }();
            }*/
            auto shell_path = home + L"\\Shell.exe";
            auto arguments = L"\"" + home + L"\\Kernel.dll\" \"" + home + L"\\Script\\main.js\"";
            for (auto i = 1; i < argc; ++i) {
                auto arg = std::wstring(argv[i], argv[i] + wcslen(argv[i]));
                arguments += L" \"" + arg + L"\"";
            }
            execute_windows_command(shell_path, arguments);
        #else
            auto home = get_executable_path();
            if (home.empty()) {
                throw std::runtime_error{"Failed to get the executable path."};
                return 1;
            }

            auto command = "\"" + home + "/Shell\" \"" + home + "/Kernel\" \"" + home + "/Script/main.js\"";
            for (auto i = 1; i < argc; ++i) {
                command += " \"" + std::string(argv[i]) + "\"";
            }
            system(command.c_str());
        #endif
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::system("pause");
    return -1;
}