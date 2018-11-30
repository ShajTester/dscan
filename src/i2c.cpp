

#include <vector>

#include "i2c.h"


i2cscanner::i2cscanner()
{
	dev.reserve(75);
	count = 0;
}


std::shared_ptr<i2cscanner> i2cscanner::create()
{
	return std::shared_ptr<i2cscanner>(new i2cscanner());
}



std::vector<devdata> i2cscanner::scan(int bus)
{
	std::vector<devdata> d(5);
	for(int i=0; i<5; i++)
	{
		d[i].addr = i*7+1;
		d[i].state = i+3;
	}

	return d;
}
