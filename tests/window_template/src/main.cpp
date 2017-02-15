#include "window_application.h"
#include "cmd/parser.h"

int main(int argc, char* argv[])
{
    auto application = WindowApplication();

    cmd::g_cmdParser.init(argc, argv);
    cmd::g_cmdParser.setLogger(application.createLogger());

    if (application.init(argc, argv) == false)
    {
        return 1;
    }

    auto result = application.execute();

    application.shutdown();

    return result;
}
