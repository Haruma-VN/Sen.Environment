#include "kernel/interface/callback.hpp"
#include "kernel/interface/version.hpp"

using namespace Sen::Kernel;

M_EXPORT_API
unsigned long long version(

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
    Interface::CliCallBack command
)
{
    try{
        auto process = Interface::convertBasicStringViewToString(argument);
        auto parameters = Interface::convertParameterToVectorString(params);
        auto* callback = new Interface::Callback(command, process, parameters, input, sendMessage);
        callback->execute();
        delete callback;
	    callback = nullptr;
    }
    catch(std::exception &ex)
    {
        sendMessage("Runtime Exception found:\n  ", ex.what(), Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    catch(int errorCode)
    {
        sendMessage("Exception found:\n  ", fmt::format("Error caught with error code: {}", errorCode).c_str(), Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    catch(...)
    {
        sendMessage("Assertation Error:\n  ", "An error occured during runtime", Sen::Kernel::Interface::Color::RED);
        return 1;
    }
    return 0;
}