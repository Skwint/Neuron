#ifndef CONFIG_SET_H
#define CONFIG_SET_H

#include <map>
#include <filesystem>

#include "ConfigItem.h"

class ConfigSet
{
public:
	ConfigSet();
	~ConfigSet();

	const std::string & name() const { return mName; }
	void read(const std::filesystem::path & path);
	void write(const std::filesystem::path & path);

	std::map<std::string, ConfigItem> & items() { return mItems; }
	const std::map<std::string, ConfigItem> & items() const { return mItems; }
	ConfigItem & operator[](const std::string & index) { return mItems[index]; }
	const ConfigItem & operator[](const std::string & index) const { return mItems.at(index); }

private:
	std::string mName;
	std::map<std::string, ConfigItem> mItems;
};

#endif
