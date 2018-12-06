

#include <iostream>
#include <memory>
#include <algorithm>
#include <set>
#include <utility>
#include <iterator>

#include "rapidjson/document.h"  
#include "rapidjson/prettywriter.h"

#include "log.h"
#include "board.hpp"
#include "i2c_class.hpp"


namespace rikor
{


int sscanHex(const char* ch)
{
	unsigned int ret;
	if(!std::sscanf(ch, "0x%x", &ret))
		ret = 0;
	return ret;
}



board::board(): 
	rep_format(ReportFormat::rep_json_indent),
	scan_type(ScanType::scan_known)
{
	SPDLOG_LOGGER_TRACE(my_logger, "board::board()");
}

board::board(const std::weak_ptr<prjConfig> &cfg): 
	board()
{
	SPDLOG_LOGGER_TRACE(my_logger, "board::board(const std::weak_ptr<prjConfig> &cfg)");
	config = cfg;
}



std::shared_ptr<board> board::create(const std::weak_ptr<prjConfig> &cfg)
{
	return std::shared_ptr<board>(new board(cfg));
}


void board::set_format(const ReportFormat rf)
{
	rep_format = rf;
}


void board::set_scantype(const ScanType st)
{
	scan_type = st;
}


void board::scan()
{
	devdata new_device;
	new_device.state = devstate::DEV_NON;
	// if(config)
	// {
		const auto &cfg = config.lock()->getDoc();   // Если что-то не сложилось, здесь получаем исключения...
		devices.clear();
		if(scan_type == ScanType::scan_all)
		{
			std::set<int> addr_set;
			int first_addr;   // Первый сканируемый адрес на шине
			int last_addr;    // Последний сканируемый адрес на шине
			for(const auto &bus: cfg.GetArray())
			{
				if(bus.HasMember("bus")) 
				{
					new_device.bus = bus["bus"].GetInt();
					new_device.cfgst = confstate::conf;
					addr_set.clear();

					if(bus.HasMember("devices"))
					{
						for(const auto &dev: bus["devices"].GetArray())
						{
							if(dev.HasMember("addr"))
								new_device.addr = sscanHex(dev["addr"].GetString());
							else
								new_device.addr = 0;

							if(dev.HasMember("name"))
								new_device.name = dev["name"].GetString();
							else
								new_device.name = "NonameDevice";

							if(dev.HasMember("descr"))
								new_device.descr = dev["descr"].GetString();
							else
								new_device.descr = "";

							addr_set.insert(new_device.addr);
							devices.push_back(new_device);
						}
					}

					// Заполняем все остальные адреса
					if(bus.HasMember("first"))
						first_addr = sscanHex(bus["first"].GetString());
					else
						first_addr = 0x03;
					if(bus.HasMember("last"))
						last_addr = sscanHex(bus["last"].GetString());
					else
						last_addr = 0x77;

					new_device.cfgst = confstate::empty;
					new_device.name = "Unknown";
					new_device.descr = "Unknown device";

					for(int i=first_addr; i<last_addr+1; i++)
					{
						auto result_l = addr_set.insert(i);
						if(result_l.second)
						{
							new_device.addr = i;
							devices.push_back(new_device);
						}
					}
				}
			}
		}
		else if(scan_type == ScanType::scan_known)
		{
			for(const auto &bus: cfg.GetArray())
			{
				if(bus.HasMember("bus")) 
				{
					new_device.bus = bus["bus"].GetInt();
					new_device.cfgst = confstate::conf;
					if(bus.HasMember("devices"))
					{
						for(const auto &dev: bus["devices"].GetArray())
						{
							if(dev.HasMember("addr"))
								new_device.addr = sscanHex(dev["addr"].GetString());
							else
								new_device.addr = 0;

							if(dev.HasMember("name"))
								new_device.name = dev["name"].GetString();
							else
								new_device.name = "NoNameDevice";

							if(dev.HasMember("descr"))
								new_device.descr = dev["descr"].GetString();
							else
								new_device.descr = "";

							devices.push_back(new_device);
						}
					}
				}
			}
		}
		else
		{
			throw "Not implemented yet";
		}
	// }
	// else
	// {
	// 	throw "No config data";
	// }

	int devsize = devices.size();
	devices.sort([](const devdata &a, const devdata &b)
		{
			return ((a.bus == b.bus) ? (a.addr < b.addr) : (a.bus < b.bus));
		});
	devices.unique([](const devdata &a, const devdata &b)
		{
			return ((a.bus == b.bus) && (a.addr == b.addr));
		});
	if(devices.size() != devsize)
		SPDLOG_LOGGER_ERROR(my_logger, "There were several nodes with the same address in the configuration");

	auto scanner = rikor::i2cscanner::create();
	scanner->scan(devices);

	for(const auto &it: devices)
	{
		if(it.state == devstate::DEV_BUSY)
		{ // Здесь нужно через драйвер проверить, что устройство отвечает
		}
	}


}



void board::print_report(std::ostream &os) const
{
	// Вывод в json
	// https://github.com/Tencent/rapidjson/blob/master/example/serialize/serialize.cpp


	if(rep_format == ReportFormat::rep_json_compact) 
	{
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartArray();

		for(const auto &it: devices)
		{
			if(!((it.state == devstate::DEV_ABSENT) && (it.cfgst == confstate::empty)))
			{
				it.Serialize(writer);
			}
		}

		writer.EndArray();
		os << sb.GetString() << std::flush;
	}
	else if(rep_format == ReportFormat::rep_json_indent)
	{
		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
		writer.StartArray();

		for(const auto &it: devices)
		{
			if(!((it.state == devstate::DEV_ABSENT) && (it.cfgst == confstate::empty)))
			{
				it.Serialize(writer);
			}
		}

		writer.EndArray();
		os << sb.GetString() << std::flush;
	}
	else
	{
		os << "Plain text writer\n\n";
		int tbus = -1;
		for(const auto &it: devices)
		{
			if(it.bus != tbus)
			{
				tbus = it.bus;
				os << fmt::format("Bus {}\n", it.bus);
			}
			if(!((it.state == devstate::DEV_ABSENT) && (it.cfgst == confstate::empty)))
			{
				os << fmt::format("   {0}-{1:#04x} :  {2}\n", it.bus, it.addr, it.state_str());
			}
		}
		os << std::flush;
	}

	return;
}


} // namespace rikor


std::ostream & std::operator<<(std::ostream &os, const rikor::board &b)  
{  

	b.print_report(os);
    return os;  
} 

