#include "kernel/interface/callback.h"

using namespace Sen::Kernel;

M_EXPORT_API
int execute(
    Interface::BasicStringView* argument, 
    Interface::Parameter* params, 
    Interface::callback sendMessage, 
    Interface::callback sendSubmessage,
    Interface::input input,
    Interface::CliCallBack command
)
{
    try{
        auto process = Interface::convertBasicStringViewToString(argument);
        auto parameters = Interface::convertParameterToVectorString(params);
        auto* callback = new Interface::Callback(command, process, parameters, input, sendMessage, sendSubmessage);
        callback->execute();
        delete callback;
    }
    catch(std::exception &ex)
    {
        sendMessage(ex.what());
        return 1;
    }
    catch(int errorCode)
    {
        sendMessage(fmt::format("Error caught with error code: {}", errorCode).c_str());
        return 1;
    }
    catch(...)
    {
        sendMessage("An error occured during runtime");
        return 1;
    }
    return 0;
}