#pragma once
#include <vector>
#include <string>

namespace app
{

class AppBase
{
public:
	AppBase();
	AppBase(int argc, const char** argv);
	virtual ~AppBase();

	virtual int exec() = 0;

protected:

	int nArgs() const;
	const std::string& arg(int index) const;

private:
	std::vector<std::string> d_userArgs;

	AppBase(const AppBase&) = delete;
	AppBase(AppBase&&) = delete;
	void operator=(const AppBase&) = delete;
	void operator=(AppBase&&) = delete;

};


} // end namespace app