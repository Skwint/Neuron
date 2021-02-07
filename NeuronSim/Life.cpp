#include "Life.h"

#include <algorithm>
#include <cassert>

#include "ConfigPresets.h"
#include "ConfigSet.h"
#include "Log.h"

using namespace std;

static const string CFG_LOW("low_threshold");
static const string CFG_HIGH("high_threshold");

Life::Life(int width, int height) :
	Net(width, height),
	mLow(2.25f),
	mHigh(3.75f)
{
}

Life::~Life()
{

}

string Life::name()
{
	return "Life";
}

void Life::setConfig(const ConfigSet & config)
{
	mLow = config.items().at(CFG_LOW).value;
	mHigh = config.items().at(CFG_HIGH).value;
}

ConfigSet Life::getConfig()
{
	ConfigSet config;
	config[CFG_LOW] = mLow;
	config[CFG_HIGH] = mHigh;
	return config;
}

const ConfigPresets & Life::getPresets()
{
	return Life::presets();
}

const ConfigPresets & Life::presets()
{
	static ConfigPresets presets;
	if (presets.configs().empty())
	{
		presets.read(name());
	}
	return presets;
}

void Life::tick(SynapseMatrix * synapses)
{
	NeuronLife * cell = &mNeurons[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			assert(cell->input != neuronLifeCheck);
			cell->firing = cell->input > mLow && cell->input < mHigh;
			++cell;
		}
	}

	Net::tick(synapses);

	cell = &mNeurons[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			// Life is an extremely leaky integrator - it leaks 100%
			// on every time step.
			cell->input = 0.0f;
			++cell;
		}
	}
}
