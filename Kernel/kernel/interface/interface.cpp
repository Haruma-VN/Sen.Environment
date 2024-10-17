#include "kernel/interface/callback.hpp"

using namespace Sen::Kernel;

M_EXPORT_API
auto execute(
    Interface::StringView* script,
    Interface::StringList* argument,
    Interface::ShellCallback m_callback
) -> int
{
    try
    {
        std::setlocale(LC_ALL, "C");
        auto script_path = Interface::make_standard_string(script);
        auto arguments = Interface::destruct_string_list(argument);
        Interface::Shell::callback = m_callback;
        Interface::Additional::assign(script_path, arguments);
        auto kernel = Interface::Callback{m_callback};
        kernel.prepare();
        kernel.execute();
    }
    catch(...)
    {
        Interface::Shell::callback(Interface::construct_string_list(std::vector{std::string{"display"}, fmt::format("Runtime Exception found: {}", parse_exception().what()), std::string{""}, std::string{"red"}}).get(), nullptr);
        return 1;
    }
    return 0;
}