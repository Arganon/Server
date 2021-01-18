#include <string>
#include <stdlib.h>
#include <getopt.h>

#include "../../utils/public/paramparser.h"

COMMANDS ParamParser::parse_params(const int _argc,
                                   char *_argv[],
								   ServerConnectInfo & _data) const noexcept
{
    COMMANDS result = COMMANDS::UNDEFINED;
    int rez;
    bool isDaemonized = false;

    while ((rez = getopt_long(_argc, _argv, m_short_options, m_long_options, 0)) != -1) {

        switch(rez){
		case 'a': {
			_data.m_host = optarg;
			result = COMMANDS::RUN;
			break;
		};

		case 'h': {
			return COMMANDS::HELP;
		};

		case 's': {
			return COMMANDS::STOP;
		};

		case 'r': {
			result = COMMANDS::RUN;
			break;
		};

		case 'f': {
			_data.m_folder = optarg;
			result = COMMANDS::RUN;
			break;
		};

		case 'p': {
			_data.m_port_regular = atoi(optarg);
			result = COMMANDS::RUN;
			break;
		};

		case 'e': {
			_data.m_port_encrypted = atoi(optarg);
			result = COMMANDS::RUN;
			break;
		};

		case 'd': {
			isDaemonized = true;
			break;
		};

		default: {
			return COMMANDS::STOP;
		};
        };
    };

    if (result == COMMANDS::RUN && isDaemonized) {
        result = COMMANDS::DAEMON;
    }

    return result;
}
