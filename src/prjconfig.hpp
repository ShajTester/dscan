#pragma once


#include <string>
#include <memory>

#include <libfastjson/json.h>

namespace rikor
{


class prjConfig
{

	std::string filename;
	rapidjson::Document d;
	char *buffer = nullptr;

	prjConfig();
	prjConfig(std::string fn);

public:

	static std::shared_ptr<prjConfig> create(const std::string &fn);

	~prjConfig();

	const rapidjson::Document &getDoc();
};


} // namespace rikor
