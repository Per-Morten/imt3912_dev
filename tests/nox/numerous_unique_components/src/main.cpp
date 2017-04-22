#include "console_application.h"
#include <cmd/parser.h>
#include <nox/util/cycle_count.h>

int main(int argc, char* argv[])
{
    auto application = ConsoleApplication();

    cmd::g_cmdParser.init(argc, argv);
    cmd::g_cmdParser.setLogger(application.createLogger());

    if (application.init(argc, argv) == false)
    {
        return 1;
    }

    auto result = application.execute();

    application.shutdown();

    const auto cycleCount = nox::util::getCpuCycleCount();
    printf("Cyclecount: %u\n", cycleCount);

    return result;
}
