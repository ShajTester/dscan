#pragma once

#include <memory>
#include <vector>

struct devdata
{
	unsigned char addr;
	unsigned char state;
};


class i2cscanner
{
private:

	int count;
	std::vector<devdata> dev;   /// Буфер для хранения информации по найденым адресам

	i2cscanner();

public:

	static std::shared_ptr<i2cscanner> create();

	std::vector<devdata> scan(int bus);

};

