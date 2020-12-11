#ifndef SERVER_PARAMPARSER_H
#define SERVER_PARAMPARSER_H

#include <getopt.h>

#include "types.h"

///@brief Enum class to choose next steps after parameters parsing.
enum class COMMANDS {
    RUN,
    STOP,
    DAEMON,
    HELP,
    UNDEFINED
};

///@brief Class for parsing input parameters, i.e. start, stop, help, port, ip and etc.
class ParamParser
{
    private:
        ///@brief The value to describe a short key types.
        const char* m_short_options { "dshrp:a:" };

        ///@brief The array contain 'option' structure which contain arguments description.
        const struct option m_long_options[7] {
            { "help", no_argument, NULL, 'h' },
            { "run", no_argument, NULL, 'r' },
            { "port", required_argument, NULL, 'p' },
            { "adr", required_argument, NULL, 'a' },
            { "stop", no_argument, NULL, 's' },
            { "daemon", no_argument, NULL, 'd' },
            { NULL, 0, NULL, 0 }
        };

    public:
        ParamParser() = default;
        ~ParamParser() = default;

        /// @brief For parsing parameters reseived from the main(...)
        /// @param[in]  _argc Quantity of the input parameters.
        /// @param[in]  _argv Parameters array.
        /// @param[out] _data Data structure for port and ip.
        /// @return     Enum value for choosing next steps.
        COMMANDS parse_params(const int _argc, char* _argv[], Data &_data) const noexcept;
};

#endif // SERVER_PARAMPARSER_H
