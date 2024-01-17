#include "kernel/interface/callback.hpp"

using namespace Sen::Kernel;

M_EXPORT_API
int version(

)
{
    return Sen::Kernel::version;
}

M_EXPORT_API
int execute(
    Interface::StringView* script,
    Interface::StringList* argument,
    Interface::ShellCallback m_callback
)
{
    try{
        auto script_path = Interface::make_standard_string(script);
        Interface::Shell::callback = m_callback;
        auto callback = std::unique_ptr<Interface::Callback>(new Interface::Callback(script_path, m_callback));
        callback->execute();
    }
    catch(...)
    {
        Interface::Shell::callback(Interface::construct_string_list(std::vector{std::string{"display"}, fmt::format("Runtime Exception found: {}", parse_exception().what()), std::string{""}, std::string{"red"}}));
        return 1;
    }
    return 0;
}