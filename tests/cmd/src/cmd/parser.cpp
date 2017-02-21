#include "parser.h"

#include <vector>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <iterator>

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
                                     {return c == '\n' || c == ' '; }), 
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

        std::vector<std::string> readArgumentsFromFile(const std::string& filepath)
        {
            std::ifstream file(filepath);
            if (!file.is_open())
            {
                //log.warning().format("Could not open file \"%s\", arguments from this file won't be loaded\n", filepath.c_str());
                return{};
            }

            std::vector<std::string> args;
            for (std::string line; std::getline(file, line);)
            {
                const std::string arg = trimWhitespaces(line);
                if (!arg.empty())
                {
                    args.push_back(arg);
                }
            }
            return args;
        }

        void mergeArguments(std::unordered_map<std::string, std::vector<std::string>>& currentArgs,
                            const std::unordered_map<std::string, std::vector<std::string>>& source)
        {
            for (const auto& item : source)
            {
                const auto& result = std::find_if(std::begin(currentArgs),
                                                  std::end(currentArgs),
                                                  [&item](const auto& str)
                                                  { return str.first == item.first; });

                // Since the custom arguments are read first and take presedence, 
                // we only check for arguments not in the currentArgs.
                if (result == std::end(currentArgs))
                {
                    currentArgs[item.first] = item.second;
                }
            }
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

    auto fileArg = m_arguments.find(constants::args_from_file_cmd);
    if (fileArg != m_arguments.end())
    {
        const auto fileArguments = local::readArgumentsFromFile(fileArg->second.front());
        const auto fileArgumentsMap = local::mapArguments(fileArguments);

        local::mergeArguments(m_arguments, fileArgumentsMap);
    }
}

void
cmd::Parser::setLogger(nox::log::Logger log)
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

    auto output = log.warning();
    output.format("Could not find argument: %s\n", commandName.c_str());
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
cmd::Parser::getFloatArgument(const std::string& commandName, float defaultValue)
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
