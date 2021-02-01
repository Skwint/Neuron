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
	void tick();
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config);

private:
	float mLow;
	float mHigh;
};

#endif
