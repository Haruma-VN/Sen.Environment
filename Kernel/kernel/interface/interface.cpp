#include "kernel/interface/callback.h"
using namespace Sen::Kernel;

M_EXPORT_API
int execute(
    Interface::BasicStringView* argument, 
    Interface::Parameter* params, 
    Interface::callback sendMessage,
    Interface::input input,
    Interface::CliCallBack command
)
{
    try{
        auto process = Interface::convertBasicStringViewToString(argument);
        auto parameters = Interface::convertParameterToVectorString(params);
        auto* callback = new Interface::Callback(command, process, parameters, input, sendMessage);
        callback->execute();
        delete callback;
    }
    catch(std::exception &ex)
    {
        sendMessage("Runtime Exception found:\n  ", ex.what(), Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    catch(int errorCode)
    {
        sendMessage("Exception found:\n  ", fmt::format("Error caught with error code: {}", parse_exception()).c_str(), Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    catch(...)
    {
        sendMessage("Assertation Error:\n  ", "An error occured during runtime", Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    return 0;
}