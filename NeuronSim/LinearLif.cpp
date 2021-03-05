#include "LinearLif.h"

#include <algorithm>

#include "ConfigPresets.h"
#include "ConfigSet.h"
#include "NeuronLif.h"

using namespace std;

static const string CFG_LEAK("leakage");
static const string CFG_THRESHOLD("threshold");
static const string CFG_RESET("reset");
static const string CFG_LOWER_LIMIT("lower_limit");

LinearLif::LinearLif(int width, int height) :
	Net<NeuronLif>(width, height),
	mLeak(0.99f),
	mThreshold(3.0f),
	mReset(0.0f),
	mLowerLimit(0.0f)
{
}

LinearLif::~LinearLif()
{
}

LinearLif * LinearLif::clone()
{
	return new LinearLif(*this);
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
	mLowerLimit = config.items().at(CFG_LOWER_LIMIT).mFloat;
}

ConfigSet LinearLif::getConfig()
{
	ConfigSet config;
	config[CFG_LEAK] = mLeak;
	config[CFG_THRESHOLD] = mThreshold;
	config[CFG_RESET] = mReset;
	config[CFG_LOWER_LIMIT] = mLowerLimit;

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

void LinearLif::postTick()
{
	NeuronLif * cell = &mNeurons[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			cell->potential *= mLeak;
			cell->potential += cell->input;
			cell->potential = max(cell->potential, mLowerLimit);
			cell->firing = (cell->potential > mThreshold);
			if (cell->firing)
			{
				cell->potential = mReset;
			}
			cell->input = 0.0f;

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
			float lum = 255.0f * neuron->potential / mThreshold;
			uint32_t val = min(uint32_t(0xFF), uint32_t(fabs(lum)));
			if (lum > 0)
			{
				// negative potential in red, positive in green:
				val = val << 8;
			}
			if (neuron->firing)
			{
				// firing in blue
				val |= 0xFF0000;
			}
			*pixel = 0xFF000000 | val;
			++pixel;
			++neuron;
		}
	}
}
