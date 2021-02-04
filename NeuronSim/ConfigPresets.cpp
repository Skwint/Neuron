#include "ConfigPresets.h"

#include <iostream>
#include <filesystem>

#include "Log.h"

using namespace std;

ConfigPresets::ConfigPresets()
{

}

ConfigPresets::~ConfigPresets()
{

}

void ConfigPresets::read(const std::string & layerType)
{
	filesystem::path configPath(filesystem::current_path() / "Data/Config" / layerType);
	LOG("Reading configs from " << configPath.string());
	for (const auto & entry : filesystem::directory_iterator(configPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".cfg")
		{
			std::string name = entry.path().stem().string();
			mConfigs[name].read(entry.path());
		}
	}
}
