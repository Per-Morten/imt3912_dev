#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include <nox/log/Logger.h>
#include <nox/log/OutputManager.h>
#include <nox/log/OutputStream.h>

#include <cmd/parser.h>


nox::log::OutputManager outputManager;
int programDuration = 0;


void
spamMessages()
{
    nox::log::Logger logger("SPAM", &outputManager);

    auto beginTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

    while (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - beginTime).count() < programDuration)
    {
        logger.info().raw("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

        currentTime = std::chrono::high_resolution_clock::now();
    }
}

int
main(int argc,
     char* argv[])
{   
    //Initialize command line parser
    cmd::g_cmdParser.init(argc, argv);
    cmd::g_cmdParser.setLogger(nox::log::Logger(&outputManager));

    //Add output in the outputmanager so the logs are written to console
    auto outputStream = std::make_unique<nox::log::OutputStream>();
    outputStream->enableLogLevel(nox::log::Message::allLevels());
    outputStream->addOutputStream(&std::cout);
    outputManager.addLogOutput(std::move(outputStream));

    //Fetch arguments from the command line parser
    programDuration = cmd::g_cmdParser.getIntArgument(cmd::constants::run_duration_ms_cmd,
                                                      cmd::constants::run_duration_ms_default);
    const auto threadCount = static_cast<std::size_t>(cmd::g_cmdParser.getIntArgument(cmd::constants::thread_amount_cmd,
                                                                                      cmd::constants::thread_amount_default));
    
    // Starting threads
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i)
    {
        threads.emplace_back(spamMessages);
    }

    // Joining pushing threads 
    std::for_each(std::begin(threads),
                  std::end(threads),
                  [](auto& thread)
    { 
        thread.join();
    });
}
