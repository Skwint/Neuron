#include "ConfigSet.h"

#include <fstream>
#include <sstream>

#include "Log.h"

using namespace std;

ConfigSet::ConfigSet()
{

}

ConfigSet::~ConfigSet()
{

}

void ConfigSet::read(const filesystem::path & path)
{
	mName = path.stem().string();
	LOG("Reading config [" << mName << "]");

	ifstream is(path);
	while (!is.eof())
	{
		std::string name;
		ConfigItem item;
		is >> name;
		is >> item;
		if (!is.eof())
		{
			mItems[name] = item;
		}
	}
}

void ConfigSet::write(const filesystem::path & path)
{
	ofstream os(path);
	for (auto item : mItems)
	{
		os << item.first;
		os << " ";
		os << item.second;
		os << "\n";
	}
}
