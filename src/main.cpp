

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <list>

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "args.hxx"
#include "version.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "log.h"
#include "prjconfig.hpp"
#include "board.hpp"

std::shared_ptr<spdlog::logger> my_logger;


// Отсюда
// https://stackoverflow.com/a/12774387
inline bool is_file_exists(const std::string &name) 
{
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}

std::string GetCurrentWorkingDir(void) 
{
	char buff[FILENAME_MAX];
	getcwd(buff, FILENAME_MAX);
	return std::string(buff);
}


int main(int argc, char const *argv[])
{

	my_logger = spdlog::stdout_color_st("A");
	my_logger->set_level(spdlog::level::trace);
	// my_logger->trace(" -=- Start");

	// Примеры разбора командной строки
	// https://taywee.github.io/args/

	args::ArgumentParser parser("R-BD-E5R-V4 i2c devices scanner", "Rikor IMT 2018.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag ver(parser, "ver", "version ", {'v', "version"});
	args::ValueFlag<std::string> conf(parser, "conf", "Config file name", {'c', "conf"});
    args::Group jsongroup(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
	args::Flag json(jsongroup, "json", "json in human readable format", {"json"});
	args::Flag json1(jsongroup, "json1", "json single line format", {"json1"});
	args::Flag outtext(jsongroup, "text", "plain text format (default)", {"text"});
    args::Group scangroup(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
	args::Flag fullscan(scangroup, "fullscan", "scan all devices in all busses", {"full"});
	args::Flag knownscan(scangroup, "knownscan", "scan only known devices (default)", {"known"});

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


	std::string configFileName;

	if(conf)
	{ // Загружаем конфигурацию из файла
		SPDLOG_LOGGER_DEBUG(my_logger, "main   Config file name: {}", args::get(conf));
		configFileName = args::get(conf);
		if(!is_file_exists(configFileName))
		{
			std::cerr << "\nNot found config file\n" << std::endl;
			return 1;
		}
	}
	else
	{ // Используем конфигурацию по умолчанию
		configFileName = "/etc/dscan/dscan.conf";
		if(!is_file_exists(configFileName))
		{
			configFileName = GetCurrentWorkingDir();
			configFileName.append("/dscan.conf");
			if(!is_file_exists(configFileName))
			{
				std::cerr << fmt::format("\nNot found config file: {}\n", configFileName) << std::endl;
				return 1;
			}
		}
	}


	SPDLOG_LOGGER_INFO(my_logger, "Config file name: {}", configFileName);
	auto config_doc = rikor::prjConfig::create(configFileName);
	auto brd = rikor::board::create(config_doc);

	if(json)
		brd->set_format(rikor::ReportFormat::rep_json_indent);
	else if(json1)
		brd->set_format(rikor::ReportFormat::rep_json_compact);
	else
		brd->set_format(rikor::ReportFormat::rep_plain);

	if(fullscan)
		brd->set_scantype(rikor::ScanType::scan_all);
	else
		brd->set_scantype(rikor::ScanType::scan_known);

	brd->scan();

	// brd->print_report(std::cout);
	std::cout << "\n" << *brd << std::endl;

	// Release and close all loggers
	spdlog::drop_all();
	return 0;
}

