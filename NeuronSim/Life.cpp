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

Life * Life::clone()
{
	return new Life(*this);
}

string Life::name()
{
	return "Life";
}

void Life::setConfig(const ConfigSet & config)
{
	mLow = config.items().at(CFG_LOW).mFloat;
	mHigh = config.items().at(CFG_HIGH).mFloat;
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

void Life::preTick()
{
	auto * cell = &mNeurons[0];
	for (int num = (int)mNeurons.size(); num; --num)
	{
		assert(cell->input != neuronLifeCheck);
		cell->input /= cell->shunt;
		cell->shunt = 1.0f;
		cell->firing = cell->input > mLow && cell->input < mHigh;
		++cell;
	}
}

void Life::postTick()
{
	NeuronLife * cell = &mNeurons[0];
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

