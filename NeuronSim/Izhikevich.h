#ifndef IZHIKEVICH_H
#define IZHIKEVICH_H

#include "Net.h"

#include "NeuronIzhikevich.h"

class ConfigPresets;

class Izhikevich : public Net<NeuronIzhikevich>
{
public:
	Izhikevich(int width, int height);
	Izhikevich(const Izhikevich & other);
	virtual ~Izhikevich();

	Izhikevich * clone() override;
	static std::string name();
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config);
	ConfigSet getConfig();
	static const ConfigPresets & presets();
	const ConfigPresets & getPresets();
	void clear();
	void postTick();

private:
	float mV2; //< The V^2 term
	float mV1; //< The V term
	float mV0; //< The constant term
	float mA;  //< Recovery time scale
	float mB;  //< Recovery sensitivity
	float mC;  //< Post-spike reset value
	float mD;  //< Post-spike recovery reset
};

#endif
