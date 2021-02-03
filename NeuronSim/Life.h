#ifndef LIFE_H
#define LIFE_H

#include "Net.h"

#include "NeuronLife.h"

class Life : public Net<NeuronLife>
{
public:
	Life(int width = 512, int height = 512);
	~Life();

	static std::string name();
	static const ConfigSet & defaultConfig();
	void tick(SynapseMatrix * synapses);
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config);
	ConfigSet getConfig();

private:
	inline void tickSegment(int cs, int ce, NeuronLife * cell, NeuronLife * dst);

private:
	float mLow;
	float mHigh;
};

#endif
