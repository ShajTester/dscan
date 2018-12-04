

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
#include "rapidjson/prettywriter.h"

#include "log.h"
#include "i2c_class.hpp"
#include "prjconfig.hpp"

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
	std::string current_working_dir(buff);
	return current_working_dir;
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
	args::Flag json(jsongroup, "json", "json ", {"json"});
	args::Flag json1(jsongroup, "json1", "json single line format ", {"json1"});

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


	SPDLOG_LOGGER_DEBUG(my_logger, "Config file name: {}", configFileName);
	auto config_doc = rikor::prjConfig::create(configFileName);

	SPDLOG_LOGGER_DEBUG(my_logger, "Bus count {}", config_doc->getDoc().GetArray().Size());

	// auto brd = rikor::board::create(config_doc);
	// brd->set_ostream(std::cout);
	// brd->set_format(rikor::board::json_f);
	// brd->set_scantype(rikor::board::scan_all);
	// brd->scan();
	// brd->print_report();








	// Вывод в json
	// https://github.com/Tencent/rapidjson/blob/master/example/serialize/serialize.cpp

	rapidjson::StringBuffer sb;

	std::unique_ptr<rapidjson::Writer<rapidjson::StringBuffer>> writer;
	// rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	if(json1) 
	{
		SPDLOG_LOGGER_DEBUG(my_logger, "rapidjson::Writer");
		writer = std::make_unique<rapidjson::Writer<rapidjson::StringBuffer>>(sb);
		// writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
	}
	else
	{
		SPDLOG_LOGGER_DEBUG(my_logger, "rapidjson::PrettyWriter");
		writer = std::make_unique<rapidjson::PrettyWriter<rapidjson::StringBuffer>>(sb);
		// auto p = std::reinterpret_cast<std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer> > >(writer);
		// p->SetFormatOptions(rapidjson::kFormatDefault);
	}
	writer->StartArray();

	auto scan = i2cscanner::create();
	for(int i=0; i<8; i++)
	{
		std::cout << "\nBus " << i << std::endl;
		auto dev = scan->scan(i);
		for(const auto &it: dev)
		{
			// SPDLOG_LOGGER_DEBUG(my_logger, "i2c-3   {0:#04X} is {1}", it.addr, it.state);
			// std::cout << fmt::format("   {0:#04x} :  {1}", it.addr, it.state_str()) << std::endl;
			it.Serialize(*writer);
		}
	}

	writer->EndArray();

	if(json || json1)
	{
		std::cout << "\n\n\n" << sb.GetString() << std::endl;
	}

	// Release and close all loggers
	spdlog::drop_all();
	return 0;
}

