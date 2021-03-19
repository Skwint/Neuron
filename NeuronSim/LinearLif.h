#ifndef LINEAR_LIF_H
#define LINEAR_LIF_H

#include "Net.h"

#include "NeuronLif.h"

// Linear LIF is a Leaky Integrate and Fire network with an exponential
// decay of potential (leak).
class LinearLif : public Net<NeuronLif>
{
public:
	// Constructor
	LinearLif(int width, int height);
	// Destructor
	virtual ~LinearLif();

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
	// Pixels will be greyscale shaded proportionally to their potential,
	// with black for the lower limit and white for the threshold.
	void paintState(uint32_t * image) override;
	// Perform the logical processing specific to Izhikevich neurons
	void tick();

private:
	// The amount of potential that persists between iterations.
	// 1.0 is a perfect integrator, and 0.0 is entirely leaky and
	// a pure coincidence detector.
	float mLeak;
	// The potential at which a cell fires and resets
	float mThreshold;
	// The potential to which a cell resets after firing
	float mReset;
	// The minimum allowed potential (due to inhibitory spikes)
	float mLowerLimit;
};

#endif

