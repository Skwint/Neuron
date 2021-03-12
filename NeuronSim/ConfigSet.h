#ifndef CONFIG_SET_H
#define CONFIG_SET_H

#include <map>
#include <filesystem>

#include "ConfigItem.h"

// A config set is a named set of ConfigItem instances.
// These should generally be obtained from an instance of a Layer,
// or from the ConfigPreset instances provided.
class ConfigSet
{
public:
	// default constructor
	ConfigSet();
	// destructor
	~ConfigSet();

	// The name of this config set. By default and empty string.
	const std::string & name() const { return mName; }
	// Read a file as a set of config items. The name of the file
	// becomes the name of the set.
	void read(const std::filesystem::path & path);
	// Write a set of config items to a file
	void write(const std::filesystem::path & path);

	// Get the set of all config items held in this config set
	std::map<std::string, ConfigItem> & items() { return mItems; }
	// Get the set of all config items held in this config set
	const std::map<std::string, ConfigItem> & items() const { return mItems; }
	// Get the config item with a specified name
	// The item will be created with a default value if it does not already exist
	ConfigItem & operator[](const std::string & index) { return mItems[index]; }
	// Get the config item with a specified name
	const ConfigItem & operator[](const std::string & index) const { return mItems.at(index); }

private:
	std::string mName; //< The name of this config set. May be an empty string
	std::map<std::string, ConfigItem> mItems; //< The items in this set
};

#endif
