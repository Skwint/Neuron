#ifndef IZHIKEVICH_H
#define IZHIKEVICH_H

#include "Net.h"

#include "NeuronIzhikevich.h"

class Izhikevich : public Net<NeuronIzhikevich>
{
public:
	Izhikevich(int width = 512, int height = 512);
	virtual ~Izhikevich();

	static std::string name();
	static const ConfigSet & defaultConfig();
	virtual void tick(SynapseMatrix * synapses);
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config);
	ConfigSet getConfig();

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
