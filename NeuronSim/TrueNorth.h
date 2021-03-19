#ifndef TRUENORTH_H
#define TRUENORTH_H

#include "Net.h"

#include "NeuronTrueNorth.h"

class ConfigPresets;

// This TrueNorth implementation is adapted for our own synapse model.
// It is also using floating point arithmetic instead of integers, and
// is missing all PRN (pseudo random number) variables.
class TrueNorth : public Net<NeuronTrueNorth>
{
public:
	// Constructor
	TrueNorth(int width, int height);
	// Destructor
	virtual ~TrueNorth();

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
	void paintState(uint32_t * image) override;
	// Perform the logical processing specific to Izhikevich neurons
	void tick();

private:
	int32_t mLeakReversal;     //< 0, 1
	float mLeakWeight;
	float mPositiveThreshold;
	float mNegativeThreshold;
	float mResetVoltage;
	int32_t mResetOrSaturate;  //< 0, 1
	int32_t mResetMode;        //< 0, 1, 2
};

#endif
