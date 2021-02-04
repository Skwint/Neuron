#ifndef LIFE_H
#define LIFE_H

#include "Net.h"

#include "NeuronLife.h"

class ConfigPresets;

class Life : public Net<NeuronLife>
{
public:
	Life(int width, int height);
	~Life();

	static std::string name();
	static const ConfigPresets & presets();
	void tick(SynapseMatrix * synapses);
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config);
	ConfigSet getConfig();
	const ConfigPresets & getPresets();

private:
	inline void tickSegment(int cs, int ce, NeuronLife * cell, NeuronLife * dst, Synapse * synapse);

private:
	float mLow;
	float mHigh;
};

#endif
