#include "console_application.h"

int main(int argc, char* argv[])
{
    auto application = pt::ConsoleApplication();

    if (application.init(argc, argv) == false)
    {
        return 1;
    }

    auto result = application.execute();

    application.shutdown();


    return result;
}
