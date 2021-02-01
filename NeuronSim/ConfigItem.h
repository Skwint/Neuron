#ifndef CONFIG_ITEM_H
#define CONFIG_ITEM_H

#include <string>
#include <vector>

struct ConfigItem
{
	std::string name;
	float minimum;
	float maximum;
	float value;
	float def;
};

typedef std::vector<ConfigItem> ConfigSet;

#endif
