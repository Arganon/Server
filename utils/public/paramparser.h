/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief The class to parse input params during server start.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef SERVER_PARAMPARSER_H
#define SERVER_PARAMPARSER_H

#include <getopt.h>

#include "../../data/public/types.h"
#include "../../data/public/data_structures.h"

///@brief Enum class to choose next steps after parameters parsing.
enum class COMMANDS {
RUN,
STOP,
DAEMON,
HELP,
UNDEFINED
};

///@brief Class for parsing input parameters, i.e. start, stop, help, port, ip and etc.
class ParamParser {
private:
	///@brief The value to describe a short key types.
	const char *m_short_options { "dshrpef:a:" };

	///@brief The array contain 'option' structure which contain arguments description.
	const struct option m_long_options[9] {
		{ "help", no_argument, NULL, 'h' },
		{ "run", no_argument, NULL, 'r' },
		{ "port", required_argument, NULL, 'p' },
		{ "encrypted_port", required_argument, NULL, 'e' },
		{ "adr", required_argument, NULL, 'a' },
		{ "stop", no_argument, NULL, 's' },
		{ "daemon", no_argument, NULL, 'd' },
		{ "folder", no_argument, NULL, 'f' },
		{ NULL, 0, NULL, 0 }
	};

public:
	/// @brief Default Constructor.
	ParamParser() = default;

	/// @brief Default Destructor.
	~ParamParser() = default;

	/// @brief For parsing parameters reseived from the main(...)
	/// @param[in]  _argc Quantity of the input parameters.
	/// @param[in]  _argv Parameters array.
	/// @param[out] _data ServerConnectInfo structure for port and ip.
	/// @return     Enum value for choosing next steps.
	COMMANDS parse_params(const int _argc,
						  char *_argv[],
						  ServerConnectInfo & _data) const noexcept;
};

#endif // SERVER_PARAMPARSER_H
