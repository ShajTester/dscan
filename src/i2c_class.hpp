#pragma once

#include <memory>
#include <vector>
#include <list>
#include <string>

#include "log.h"


namespace rikor
{

enum class devstate
{
	DEV_NON = 0,       // Не сканировалось
	DEV_ABSENT = 1,    // Не ответил на запросы
	DEV_PRESENT,       // Ответил на запросы / Присутствует в конфигурации
	DEV_BUSY,          // К этому адресу подключен драйвер
	DEV_ERROR
};

enum class confstate
{
	empty,             // Устройства нет в конфигурации
	conf,              // Устройство есть в конфигурации
	conf_driver        // Устройство есть в конфигурации, к нему подключен драйвер
};

class devdata
{
public:
	int bus;              // Номер шины
	int addr;             // Адрес устройства на шине
	devstate state;       // Состояние сканирования
	confstate cfgst;      // Состояние устройства в конфигурации
	std::string name;     // Короткое описание устройства
	std::string descr;    // Развернутое описание устройства

	devdata(){}
	std::string state_str() const;

	// Отсюда
	// https://github.com/Tencent/rapidjson/blob/30d92a6399b6077006d976b1dc05ee13305bf1c4/example/serialize/serialize.cpp#L86
	template <typename Writer>
	void Serialize(Writer &writer) const
	{
		writer.StartObject();
		writer.String("bus");
		writer.Uint(bus);
		writer.String("addr");
		writer.String(fmt::format("{0:#04x}", addr).c_str());
		writer.String("name");
		writer.String(name.c_str());
		writer.String("descr");
		writer.String(descr.c_str());
		writer.String("state");
		writer.Uint(static_cast<int>(state) + static_cast<int>(cfgst) * 100);
		// int a = state;
		// int b = cfgst;
		// writer.Uint(a + b * 100);
		writer.String("state_descr");
		writer.String(state_str().c_str());
		writer.EndObject();
	}
};


class i2cscanner
{
private:

	int count;
	std::vector<devdata> dev;   /// Буфер для хранения информации по найденым адресам

	i2cscanner();

public:

	static std::shared_ptr<i2cscanner> create();

	std::list<devdata> scan(int bus_num);
	void scan(std::list<devdata> &devices);
};

} // namespace rikor

