

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <list>

#include "args.hxx"
#include "version.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "log.h"
#include "i2c_class.hpp"
#include "rapidjson/prettywriter.h"

std::shared_ptr<spdlog::logger> my_logger;

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

	if(conf)
	{ // Загружаем конфигурацию из файла
		SPDLOG_LOGGER_DEBUG(my_logger, "main   Config file name: {}", args::get(conf));
	}
	else
	{ // Используем конфигурацию по умолчанию
	}

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
		auto p = std::reinterpret_cast<std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer> > >(writer);
		p->SetFormatOptions(rapidjson::kFormatDefault);
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
			std::cout << fmt::format("   {0:#04x} :  {1}", it.addr, it.state_str()) << std::endl;
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

