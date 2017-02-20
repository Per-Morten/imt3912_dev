#include "parser.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <iterator>
#include <vector>

cmd::Parser cmd::g_cmdParser;


namespace
{
    namespace local
    {
        std::string trimWhitespaces(const std::string& source)
        {
            std::string str = source;

            str.erase(std::remove_if(std::begin(str),
                                     std::end(str),
                                     [](const auto& c)
                                     { return c == '\n' || c == ' '; }), 
                      std::end(str));

            return str;
        }

        std::vector<std::string> stringifyArguments(int argc, char** argv)
        {
            std::vector<std::string> args;
            args.reserve(argc);
            for (int i = 1; i < argc; ++i)
            {

                const std::string arg = trimWhitespaces(argv[i]);
                if (!arg.empty())
                {
                    args.push_back(arg);
                }
            }

            return args;
        }

        std::unordered_map<std::string, std::vector<std::string>> mapArguments(const std::vector<std::string>& args)
        {
            std::unordered_map<std::string, std::vector<std::string>> arguments;
            std::string currentMode = "";
            for (const auto& item : args)
            {
                if (item.front() == '-')
                {
                    currentMode = item;
                }
                else
                {
                    arguments[currentMode].emplace_back(item);
                }
            }
            return arguments;
        }
    }
}

void
cmd::Parser::init(int argc, char** argv)
{
    const auto args = local::stringifyArguments(argc, argv);

    m_arguments = local::mapArguments(args);
}

void
cmd::Parser::setLogger(nox::app::log::Logger log)
{
    log = std::move(log);
    log.setName("cmdParser");
}

const std::string&
cmd::Parser::getStringArgument(const std::string& commandName,
                               const std::string& defaultValue)
{
    const auto& arg = std::find_if(std::cbegin(m_arguments),
                                   std::cend(m_arguments),
                                   [&commandName](const auto& item)
                                   { return item.first == commandName; });

    if (arg != std::cend(m_arguments) && !arg->second.empty())
    {
        return arg->second.front();
    }

    log.warning().output.format("Could not find argument: %s\n", commandName.c_str());
    return defaultValue;
}

int
cmd::Parser::getIntArgument(const std::string& commandName,
                            int defaultValue)
{
    const auto& arg = std::find_if(std::cbegin(m_arguments),
                                   std::cend(m_arguments),
                                   [&commandName](const auto& item)
                                   { return item.first == commandName; });

    if (arg != std::cend(m_arguments))
    {
        try
        {
            return std::stoi(arg->second.front());
        }
        catch (const std::exception& e)
        {
            log.warning().format("Exception encountered: %s\n", e.what());
        }
    }

    log.warning().format("Could not find argument: %s\n", commandName.c_str());
    return defaultValue;
}

float 
cmd::Parser::getFloatArgument(const std::string& commandName, 
                              float defaultValue)
{
    const auto& arg = std::find_if(std::cbegin(m_arguments),
                                   std::cend(m_arguments),
                                   [&commandName](const auto& item)
                                   { return item.first == commandName; });

    if (arg != std::cend(m_arguments))
    {
        try
        {
            return std::stof(arg->second.front());
        }
        catch (const std::exception& e)
        {
            log.warning().format("Exception encountered: %s\n", e.what());
        }
    }

    log.warning().format("Could not find argument: %s\n", commandName.c_str());
    return defaultValue;
}

std::size_t
cmd::Parser::getStringArguments(const std::string& commandName,
                                std::vector<std::string>& buffer) const
{
    const auto& arg = std::find_if(std::cbegin(m_arguments),
                                   std::cend(m_arguments),
                                   [&commandName](const auto& item)
                                   { return item.first == commandName; });
    const auto sizeBeforeCopy = buffer.size();
    if (arg != std::cend(m_arguments))
    {
        std::copy(std::cbegin(arg->second),
                  std::cend(arg->second),
                  std::back_inserter(buffer));
    }
    const auto sizeAfterCopy = buffer.size();
    
    return sizeAfterCopy - sizeBeforeCopy;
}
