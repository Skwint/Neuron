#ifndef KUMAR_H
#define KUMAR_H

#include "Net.h"

#include "NeuronKumar.h"

class ConfigPresets;

class Kumar : public Net<NeuronKumar>
{
public:
	Kumar(int width, int height);
	virtual ~Kumar();

	static std::string name();
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config);
	ConfigSet getConfig();
	static const ConfigPresets & presets();
	const ConfigPresets & getPresets();
	void clear();
	void preTick();

private:
	float mV;  //< The V term
	float mA;  //< Recovery time scale
	float mB;  //< Recovery sensitivity
	float mC;  //< Post-spike reset value
	float mD;  //< Post-spike recovery reset
};

#endif
