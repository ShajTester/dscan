

#include <iostream>
#include <string>


#include "args.hxx"
#include "version.h"

int main(int argc, char const *argv[])
{

	// Примеры разбора командной строки
	// https://taywee.github.io/args/

	args::ArgumentParser parser("R-BD-E5R-V4 i2c devices scanner", "This goes after the options");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Flag ver(parser, "ver", "version ", {'v', "version"});
    args::ValueFlag<std::string> conf(parser, "conf", "Config file name", {'c', "conf"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }


    if(ver)
    {
    	std::cout << "R-BD-E5R-V4 i2c devices scanner\nversion " << VER << std::endl;
    	return 0;
    }

    if(conf)
    {
    	std::cout << "Config file name: " << args::get(conf) << std::endl;
    }

    return 0;
}

