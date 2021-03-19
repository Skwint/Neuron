#ifndef KUMAR_H
#define KUMAR_H

#include "Net.h"

#include "NeuronKumar.h"

class ConfigPresets;

class Kumar : public Net<NeuronKumar>
{
public:
	// Constructor
	Kumar(int width, int height);
	// Destructor
	virtual ~Kumar();

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
	// Each specialization of Net will interpret its state in its own colour
	// scheme.
	void paintState(uint32_t * image) override;
	// Reset all cells in this layer to a state which is hopefully
	// queiscent and stable (v = mC, u = mC * mB)
	void clear();
	// Perform the logical processing specific to Izhikevich neurons
	void tick();

private:
	float mV;  //< The V term
	float mA;  //< Recovery time scale
	float mB;  //< Recovery sensitivity
	float mC;  //< Post-spike reset value
	float mD;  //< Post-spike recovery reset
};

#endif
