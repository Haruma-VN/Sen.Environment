#include "kernel/definition/utility.hpp"

using namespace Sen::Kernel;
using std::exception;

M_EXPORT_API
int execute(int size, char** argc)
{
    try{
        debug(Encryption::MD5::hashData(String::convertStringToSpan<unsigned char>("Hello World")));
    }
    catch(exception &ex)
    {
        debug(ex.what());
        return 1;
    }
    catch(int errorCode)
    {
        debug(fmt::format("Error caught with error code: {}", errorCode));
        return 1;
    }
    catch(...)
    {
        debug("An error occured during runtime");
        return 1;
    }
    return 0;
}