#pragma once
#include <string>
#include <unordered_map>

#include "constants.h"

#include <nox/app/log/Logger.h>

namespace cmd
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// \brief Easy parser for getting access to cmd arguments.
    ///
    ///////////////////////////////////////////////////////////////////////////////////////////
    class Parser
    {
    public:
        ///////////////////////////////////////////////////////////////////////////////////////
        /// \brief initializes the parser, maps all arguments to their respective commands.
        ///
        /// \param argc number of arguments.
        /// \param argv ptr to cmd arguments.
        ///////////////////////////////////////////////////////////////////////////////////////
        void init(int argc, char** argv);

        ///////////////////////////////////////////////////////////////////////////////////////
        /// \brief sets the logger
        ///
        /// \param log 
        ///     new logger that is properly set up
        ///////////////////////////////////////////////////////////////////////////////////////
        void setLogger(nox::app::log::Logger log);

        ///////////////////////////////////////////////////////////////////////////////////////
        /// \brief returns the argument respective to the command name or defaultValue
        ///
        /// \param commandName 
        ///     name of the command to retreive value from.
        /// \param defaultValue
        ///     this will be returned if no arguments are mapped to the command name.
        ///
        /// \return argument 
        ///     respective to the commandName if it exists, defaultValue otherwise.
        ///////////////////////////////////////////////////////////////////////////////////////
        const std::string& getStringArgument(const std::string& commandName,
                                             const std::string& defaultValue);

        ///////////////////////////////////////////////////////////////////////////////////////
        /// \brief returns the argument respective to the command name or defaultValue
        ///
        /// \param commandName 
        ///     name of the command to retreive value from.
        /// \param defaultValue
        ///     this will be returned if no arguments are mapped to the command name.
        ///
        /// \return argument 
        ///     respective to the commandName if it exists, defaultValue otherwise.
        ///////////////////////////////////////////////////////////////////////////////////////
        int getIntArgument(const std::string& commandName,
                           int defaultValue);

        ///////////////////////////////////////////////////////////////////////////////////////
        /// \brief returns the argument respective to the command name or defaultValue
        ///
        /// \param commandName 
        ///     name of the command to retreive value from.
        /// \param defaultValue
        ///     this will be returned if no arguments are mapped to the command name.
        ///
        /// \return argument 
        ///     respective to the commandName if it exists, defaultValue otherwise.
        ///////////////////////////////////////////////////////////////////////////////////////
        float getFloatArgument(const std::string& commandName,
                               float defaultValue);

        ///////////////////////////////////////////////////////////////////////////////////////
        /// \brief copies values respective to the command name into a buffer
        ///
        /// \param commandName 
        ///     name of the command to retreive value from.
        /// \param buffer
        ///     buffer to copy the arguments into.
        ///
        /// \return number of arguments copied into buffer.
        ///////////////////////////////////////////////////////////////////////////////////////
        std::size_t getStringArguments(const std::string& commandName,
                                       std::vector<std::string>& buffer) const;

    private:
        std::unordered_map<std::string, std::vector<std::string>> m_arguments;
        nox::app::log::Logger log;

    };

    extern Parser g_cmdParser;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class ftcp::cmd::Parser
/// \ingroup cmd
///
/// Used for access to cmd arguments passed to the program during startup
///////////////////////////////////////////////////////////////////////////////////////////////////
