

#include <iostream>
#include <memory>
#include <algorithm>
#include <set>
#include <utility>
#include <iterator>

#include "rapidjson/document.h"  

#include "log.h"
#include "board.hpp"


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
					SPDLOG_LOGGER_DEBUG(my_logger, "Found bus #{}", new_device.bus);
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
					SPDLOG_LOGGER_DEBUG(my_logger, "Found bus #{}", new_device.bus);

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


	SPDLOG_LOGGER_DEBUG(my_logger, "Scan complete");
	SPDLOG_LOGGER_DEBUG(my_logger, "   Devices scaned {}", devices.size());
	for(const auto &it: devices)
	{
		SPDLOG_LOGGER_DEBUG(my_logger, "{0}-{1:04x}  -  {2}",
			it.bus, it.addr, it.name);
	}


}



void board::print_report(std::ostream &os)
{
	os << *this;
}


} // namespace rikor


std::ostream & std::operator<<(std::ostream &os, const rikor::board &b)  
{  
    os << "This is a test report" << std::flush;
    return os;  
} 

