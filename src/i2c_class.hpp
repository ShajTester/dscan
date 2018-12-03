#pragma once

#include <memory>
#include <vector>
#include <list>


enum devstate
{
	DEV_NON = 0,
	DEV_ABSENT = 1,
	DEV_PRESENT,
	DEV_BUSY,
	DEV_ERROR
};

struct devdata
{
	unsigned char addr;
	unsigned char state;

	devdata(unsigned char a, unsigned char s): addr(a), state(s) {}
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

