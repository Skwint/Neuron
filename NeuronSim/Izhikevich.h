#ifndef IZHIKEVICH_H
#define IZHIKEVICH_H

#include "Net.h"

#include "NeuronIzhikevich.h"

class ConfigPresets;

class Izhikevich : public Net<NeuronIzhikevich>
{
public:
	Izhikevich(int width, int height);
	virtual ~Izhikevich();

	static std::string name();
	static const ConfigSet & defaultConfig();
	virtual void tick(SynapseMatrix * synapses);
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config);
	ConfigSet getConfig();
	static const ConfigPresets & presets();
	const ConfigPresets & getPresets();

private:
	float mV2;
	float mV1;
	float mV0;
	float mA;
	float mB;
	float mC;
	float mD;
};

#endif
