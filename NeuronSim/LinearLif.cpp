#include "LinearLif.h"

#include <algorithm>

#include "ConfigPresets.h"
#include "ConfigSet.h"
#include "NeuronLif.h"

using namespace std;

static const string CFG_LEAK("leakage");
static const string CFG_THRESHOLD("threshold");
static const string CFG_RESET("reset");
static const string CFG_REFRACTORY("refractory");

LinearLif::LinearLif(int width, int height) :
	Net<NeuronLif>(width, height),
	mLeak(0.99f),
	mThreshold(3.0f),
	mReset(0.0f),
	mRefractory(2)
{
}

LinearLif::~LinearLif()
{
}

std::string LinearLif::name()
{
	return "LIF (linear)";
}

void LinearLif::setConfig(const ConfigSet & config)
{
	mLeak = config.items().at(CFG_LEAK).mFloat;
	mThreshold = config.items().at(CFG_THRESHOLD).mFloat;
	mReset = config.items().at(CFG_RESET).mFloat;
	mRefractory = config.items().at(CFG_REFRACTORY).mInt;
}

ConfigSet LinearLif::getConfig()
{
	ConfigSet config;
	config[CFG_LEAK] = mLeak;
	config[CFG_THRESHOLD] = mThreshold;
	config[CFG_RESET] = mReset;
	config[CFG_RESET] = mRefractory;

	return config;
}

const ConfigPresets & LinearLif::getPresets()
{
	return LinearLif::presets();
}

const ConfigPresets & LinearLif::presets()
{
	static ConfigPresets presets;
	if (presets.configs().empty())
	{
		presets.read(name());
	}
	return presets;
}

void LinearLif::tick(SynapseMatrix * synapses)
{
	Net::tick(synapses);

	NeuronLif * cell = &mNeurons[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			cell->potential *= mLeak;
			if (cell->refractory > 0)
			{
				--cell->refractory;
			}
			else
			{
				cell->potential += cell->input;
				cell->input = 0.0f;
				cell->firing = (cell->potential > mThreshold);
				if (cell->firing)
				{
					cell->potential = mReset;
					cell->refractory = mRefractory;
				}
			}
			++cell;
		}
	}
}

// We overload the Net implementation because we really want
// to colour proportionally to the threshold.
void LinearLif::paint(uint32_t * image)
{
	NeuronLif * neuron = &mNeurons[0];
	uint32_t * pixel = image;
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			uint32_t lum = min(uint32_t(0xFF), uint32_t(255.0f * neuron->potential / mThreshold));
			*pixel = 0xFF000000 | (neuron->firing?0xFF0000:0x00) | lum << 8 | lum;
			++pixel;
			++neuron;
		}
	}
}
