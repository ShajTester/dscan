#pragma once

#include <memory>
#include <ostream>

#include "rapidjson/document.h"  
#include "prjconfig.hpp"
#include "i2c_class.hpp"


namespace rikor
{


enum class ReportFormat
{
	rep_plain,
	rep_json_indent,
	rep_json_compact
};

enum class ScanType
{
	scan_all,
	scan_known
};


class board
{

	ReportFormat rep_format = ReportFormat::rep_json_indent;
	ScanType scan_type = ScanType::scan_known;
	std::weak_ptr<prjConfig> config;
	std::list<devdata> devices;

	board();
	board(const std::weak_ptr<prjConfig> &cfg);

public:

	static std::shared_ptr<board> create(const std::weak_ptr<prjConfig> &cfg);

	void set_format(const ReportFormat rf);
	void set_scantype(const ScanType st);

	void scan();
	void print_report(std::ostream &os) const;
	// friend std::ostream& std::operator<<(std::ostream& os, const board& b);


}; // class board


} // namespace rikor



namespace std { // Взято отсюда: https://ru.stackoverflow.com/a/267766

std::ostream & operator<<(std::ostream &os, const rikor::board &b);

}


