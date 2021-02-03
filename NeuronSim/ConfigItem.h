#ifndef CONFIG_ITEM_H
#define CONFIG_ITEM_H

#include <string>
#include <map>

struct ConfigItem
{
	ConfigItem() :
		value(0.0f)
	{}
	ConfigItem(float value) :
		value(value)
	{}
	float value;
};

typedef std::map<std::string, ConfigItem> ConfigSet;

#endif
