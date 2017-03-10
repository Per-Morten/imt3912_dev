#include <chrono>
#include <thread>

#include <nox/log/Logger.h>
#include <nox/log/OutputManager.h>

#include <cmd/parser.h>


OutputManager outputManager;


void
spamMessages()
{
    Logger logger(*outputManager);

    auto beginTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

    while (std::chrono::duration<std::chrono::milliseconds>(currentTime - beginTime).count() < programDuration)
    {


        currentTime = std::chrono::high_resolution_clock::now();
    }
}

int
main(int argc,
     char* argv[])
{   
    //Initialize command line parser
    cmd::g_cmdParser.init(argc, argv);
    cmd::g_cmdParser.setLogger("", Logger(*outputManager));

    //Fetch arguments from the command line parser
    int programDuration = cmd::g_cmdParser.getIntArgument(cmd::constants::run_duration_ms_cmd,
                                                          cmd::constants::run_duration_ms_default);
    int threadCount = cmd::g_cmdParser.getIntArgument(cmd::constants::thread_count_cmd,
                                                      cmd::constants::thread_count_default);
    
    // Starting threads
    std::vector<std::thread> threads;
    threads.reserve()
    for (std::size_t i = 0; i < threadCount; ++i)
    {
        threads.emplace_back();
    }

    // Joining pushing threads 
    std::for_each(std::begin(threads),
                  std::end(threads),
                  [](auto& thread)
    { 
        thread.join();
    });
}
