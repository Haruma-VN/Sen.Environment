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
    Interface::StringView* argument,
    Interface::ShellCallback m_callback
)
{
    try{
        auto script = Interface::make_standard_string(argument);
        Interface::Shell::callback = m_callback;
        auto callback = std::unique_ptr<Interface::Callback>(new Interface::Callback(script, m_callback));
        callback->execute();
    }
    catch(const std::exception & ex)
    {
        Interface::Shell::callback(Interface::construct_string_list(std::vector{std::string{"display"}, fmt::format("Runtime Exception found: {}", parse_exception().what()), std::string{""}, std::string{"red"}}));
        return 1;
    }
    catch(int errorCode)
    {
        Interface::Shell::callback(Interface::construct_string_list(std::vector{std::string{"display"}, fmt::format("Exception found: {} with error code {}", parse_exception().what(), errorCode), std::string{""}, std::string{"red"}}));
        return 1;
    }
    catch(...)
    {
        Interface::Shell::callback(Interface::construct_string_list(std::vector{std::string{"display"}, fmt::format("Unknown Exception found: {}", parse_exception().what()), std::string{""}, std::string{"red"}}));
        return 1;
    }
    return 0;
}