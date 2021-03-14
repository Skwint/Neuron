#ifndef LINEAR_LIF_H
#define LINEAR_LIF_H

#include "Net.h"

#include "NeuronLif.h"

class LinearLif : public Net<NeuronLif>
{
public:
	LinearLif(int width, int height);
	virtual ~LinearLif();

	static std::string name();
	void postTick();
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config);
	ConfigSet getConfig();
	static const ConfigPresets & presets();
	const ConfigPresets & getPresets();
	void paint(uint32_t * image);

private:
	float mLeak;
	float mThreshold;
	float mReset;
	float mLowerLimit;
};

#endif

