#ifndef LIFE_H
#define LIFE_H

#include "Net.h"

#include "NeuronLife.h"

class ConfigPresets;

class Life : public Net<NeuronLife>
{
public:
	// Constructor
	Life(int width, int height);
	// Destructor
	~Life();

	// The indetifying name for this type of layer
	// This is static for access via LayerFactory
	static std::string name();
	// The identifying name for this type of layer
	std::string typeName() { return name(); }
	// Set the configuration data
	void setConfig(const ConfigSet & config);
	// Get the configuration data
	ConfigSet getConfig();
	// Get the available preset configurations for this layer type
	// This is static for access via LayerFactory
	static const ConfigPresets & presets();
	// Get the available preset configurations for this layer type
	const ConfigPresets & getPresets();
	// Take a pointer to the start of an array of pixels and populate them
	// with the state of the neurons. The pixels should be in ABGR order.
	// Life has no internal state - it exists entirely within the spikes -
	// and consequently paintState here simply calls paintSpikes.
	void paintState(uint32_t * image) override { paintSpikes(image); }
	// Perform the logical processing specific to Izhikevich neurons
	void tick();

private:
	// Low theshold - below this incoming spike density we don't fire
	float mLow;
	// High threshold - above this incoming spike density we don't fire
	float mHigh;
};

#endif
