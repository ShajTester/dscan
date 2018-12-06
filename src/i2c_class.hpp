#pragma once

#include <memory>
#include <vector>
#include <list>
#include <string>
#include "rapidjson/prettywriter.h"


namespace rikor
{

enum devstate
{
	DEV_NON = 0,
	DEV_ABSENT = 1,
	DEV_PRESENT,
	DEV_BUSY,
	DEV_ERROR
};

class devdata
{
public:
	unsigned char bus;
	unsigned char addr;
	unsigned char state;
	std::string name;
	std::string descr;

	devdata(){}
	devdata(unsigned char a, unsigned char s): addr(a), state(s) {}
	std::string state_str() const;

	// Отсюда
	// https://github.com/Tencent/rapidjson/blob/30d92a6399b6077006d976b1dc05ee13305bf1c4/example/serialize/serialize.cpp#L86
	template <typename Writer>
	void Serialize(Writer &writer) const
	{
		writer.StartObject();
		writer.String("bus");
		writer.Uint(3);
		writer.String("addr");
		writer.Uint(addr);
		writer.String("state");
		writer.Uint(state);
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

};

} // namespace rikor

