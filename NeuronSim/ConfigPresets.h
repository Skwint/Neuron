#ifndef CONFIG_PRESETS_H
#define CONFIG_PRESETS_H

#include <map>

#include "ConfigSet.h"

class ConfigPresets
{
public:
	ConfigPresets();
	~ConfigPresets();

	void read(const std::string & layerType);

	std::map<std::string, ConfigSet> configs() { return mConfigs; }
	inline bool contains(const std::string & name) const { return mConfigs.count(name); }
	ConfigSet & operator[](const std::string & name) { return mConfigs[name]; }
	const ConfigSet & operator[](const std::string & name) const { return mConfigs.at(name); }
	

private:
	std::map<std::string, ConfigSet> mConfigs;
};

#endif
