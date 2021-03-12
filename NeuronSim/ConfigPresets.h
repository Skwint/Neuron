#ifndef CONFIG_PRESETS_H
#define CONFIG_PRESETS_H

#include <map>

#include "ConfigSet.h"

// ConfigPresets is a set of ConfigSet instances, each containing a named set
// of configuration values. These are loaded from disk at startup and make
// quickly configuring layers much easier.
class ConfigPresets
{
public:
	// Default constructor
	ConfigPresets();
	// Destructor
	~ConfigPresets();

	// Read all the presets for a specfied type of neuron
	void read(const std::string & layerType);

	// Get a mapping of configuration names to config sets
	std::map<std::string, ConfigSet> configs() { return mConfigs; }
	// Check to see if a preset with a particular name exists
	inline bool contains(const std::string & name) const { return mConfigs.count(name); }
	// Get a preset with the given name. Note that this will create a
	// new, default configuration with this name if it does not already
	// exist.
	ConfigSet & operator[](const std::string & name) { return mConfigs[name]; }
	// Returns a configset with the given name, but only if it already
	// exists.
	const ConfigSet & operator[](const std::string & name) const { return mConfigs.at(name); }
	

private:
	std::map<std::string, ConfigSet> mConfigs; //< Available presets and names
};

#endif
