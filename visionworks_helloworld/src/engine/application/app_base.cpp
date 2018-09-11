#include "app_base.h"
#include <assert.h>
#include <iostream>

namespace app
{

AppBase::AppBase()
{
}

AppBase::AppBase(int argc, const char ** argv)
{
	d_userArgs.resize(argc);
	for (int i = 0; i < argc; ++i)
	{
		d_userArgs[i] = std::string(argv[i]);
	}
}

AppBase::~AppBase()
{
}

int AppBase::nArgs() const
{
	return (int)d_userArgs.size();
}

const std::string & AppBase::arg(int index) const
{
	assert(index >= 0 && index < d_userArgs.size());
	return d_userArgs[index];
}

} // end namespace app