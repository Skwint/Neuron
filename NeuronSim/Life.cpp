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

void Life::tick()
{
	auto * cell = &mNeurons[0];
	for (int num = (int)mNeurons.size(); num; --num)
	{
		assert(cell->input != neuronLifeCheck);
		cell->input /= cell->shunt;
		cell->shunt = 1.0f;
		cell->firing = cell->input > mLow && cell->input < mHigh;
		cell->input = 0.0f;
		++cell;
	}
}
