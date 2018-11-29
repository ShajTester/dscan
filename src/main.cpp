

#include <iostream>
#include <string>
#include <memory>


#include "args.hxx"
#include "version.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "log.h"

std::shared_ptr<spdlog::logger> my_logger;

int main(int argc, char const *argv[])
{

	my_logger = spdlog::stdout_color_st("console");
	my_logger->set_level(spdlog::level::trace);
	my_logger->trace(" -=- Start");

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
    { // Загружаем конфигурацию из файла
    	SPDLOG_LOGGER_DEBUG(my_logger, "main   Config file name: {}", args::get(conf));
    }
    else
	{ // Используем конфигурацию по умолчанию
	}

	SPDLOG_LOGGER_INFO(my_logger, "main   end");

	std::cout << "END of programm" << std::endl;

    return 0;
}

