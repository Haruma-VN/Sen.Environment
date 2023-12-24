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
    Interface::BasicStringView* argument, 
    Interface::Parameter* params, 
    Interface::callback sendMessage,
    Interface::input input,
    Interface::MShellAPI shell
)
{
    try{
        auto process = Interface::convertBasicStringViewToString(argument);
        auto parameters = Interface::convertParameterToVectorString(params);
        Interface::Shell::input = input;
        Interface::Shell::print = sendMessage;
        auto callback = std::make_shared<Interface::Callback>(process, parameters, shell);
        callback->execute();
    }
    catch(std::exception &ex)
    {
        sendMessage(fmt::format("Runtime Exception found:\n  {}", ex.what()).c_str(), Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    catch(int errorCode)
    {
        // 
        sendMessage(fmt::format("Exception found:\n  {}", fmt::format("Error caught with error code: {}", errorCode)).c_str(), Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    catch(...)
    {
        sendMessage(fmt::format("Assertation Error:\n  {}", "An error occured during runtime").c_str(), Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    return 0;
}