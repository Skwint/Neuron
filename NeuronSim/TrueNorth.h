#ifndef TRUENORTH_H
#define TRUENORTH_H

#include "Net.h"

#include "NeuronTrueNorth.h"

class ConfigPresets;

// This TrueNorth implementation is adapted for our own synapse model.
// It is also missing all PRN (pseudo random number) variables.
class TrueNorth : public Net<NeuronTrueNorth>
{
public:
	TrueNorth(int width, int height);
	virtual ~TrueNorth();

	static std::string name();
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config) override;
	ConfigSet getConfig() override;
	static const ConfigPresets & presets();
	const ConfigPresets & getPresets() override;
	void clear() override;
	void preTick() override;

private:
	int32_t mLeakReversal;     //< 0, 1
	float mLeakWeight;
	float mPositiveThreshold;
	float mNegativeThreshold;
	float mResetVoltage;
	int32_t mResetOrSaturate;  //< 0, 1
	int32_t mResetMode;        //< 0, 1, 2
};

#endif
