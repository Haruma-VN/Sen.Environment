#include "kernel/definition/utility.hpp"
#include "kernel/interface/data.hpp"

using namespace Sen::Kernel;
using std::exception;

M_EXPORT_API
int execute(
    Interface::Argument* argument, 
    Interface::Parameter* params, 
    Interface::callback sendMessage, 
    Interface::callback sendSubmessage,
    Interface::input input
)
{
    try{
        auto arguments = Interface::convertArgumentToVectorString(argument);
        auto parameters = Interface::convertParameterToVectorString(params);
        for(auto &c : FileSystem::readWholeDirectory("D:/Code/Sen.Environment/Kernel/build/kernel"))
        {
            sendMessage(c.c_str());
        }
    }
    catch(exception &ex)
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