#include "Life.h"

#include "ConfigItem.h"
#include "Log.h"

static const std::string CFG_LOW("low threshold");
static const std::string CFG_HIGH("high threshold");

Life::Life(int width, int height) :
	Net(width, height)
{
	setConfig(Life::defaultConfig());
}

Life::~Life()
{

}

std::string Life::name()
{
	return "Life";
}

void Life::setConfig(const ConfigSet & config)
{
	mLow = config.at(CFG_LOW).value;
	mHigh = config.at(CFG_HIGH).value;
}

ConfigSet Life::getConfig()
{
	ConfigSet config;
	config[CFG_LOW] = mLow;
	config[CFG_HIGH] = mHigh;
	return config;
}

const ConfigSet & Life::defaultConfig()
{
	static ConfigSet config;
	if (config.empty())
	{
		config[CFG_LOW] = ConfigItem(2.25f);
		config[CFG_HIGH] = ConfigItem(3.75f);
	}
	return config;
}

inline void Life::tickSegment(int cs, int ce, NeuronLife * cell, NeuronLife * dst)
{
	dst += cs;
	for (int tc = cs; tc < ce; ++tc)
	{
		// This is taking weights from the rules of life, instead of from the synapse matrix - TODO
		if (cell == dst)
			mSpikeProcessor->fire(&dst->input, 0.5f, 0);
		else
			mSpikeProcessor->fire(&dst->input, 1.0f, 0);
		++dst;
	}
}

void Life::tick()
{
	for (int rr = 0; rr < mHeight; ++rr)
	{
		NeuronLife * cell = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			// Life is an extremely leaky integrator - it leaks 100%
			// on every time step.
			cell->potential = cell->input;
			cell->input = 0.0f;
			++cell;
		}
	}

	// TODO - is there any way we can generalize this without resorting to a
	// polymorphic function call on every cell?
	for (int rr = 0; rr < mHeight; ++rr)
	{
		int normRowBegin = synapseNormRowBegin(rr);
		int normRowEnd = synapseNormRowEnd(rr);
		int lowRowBegin = synapseLowWrapRowBegin(rr);
		int lowRowEnd = synapseLowWrapRowEnd(rr);
		int highRowBegin = synapseHighWrapRowBegin(rr);
		int highRowEnd = synapseHighWrapRowEnd(rr);

		NeuronLife * cell = row(rr);
		NeuronLife * dst;
		for (int cc = 0; cc < mWidth; ++cc)
		{
			// Life was designed in terms of "alive" and "dead" but
			// here we equate "alive" to "firing".
			if (cell->potential > mLow && cell->potential < mHigh)
			{
				int normColBegin = synapseNormColBegin(cc);
				int normColEnd = synapseNormColEnd(cc);
				int lowColBegin = synapseLowWrapColBegin(cc);
				int lowColEnd = synapseLowWrapColEnd(cc);
				int highColBegin = synapseHighWrapColBegin(cc);
				int highColEnd = synapseHighWrapColEnd(cc);
				for (int tr = lowRowBegin; tr < lowRowEnd; ++tr)
				{
					dst = cell + tr * rowStep();
					tickSegment(lowColBegin, lowColEnd, cell, dst);
					tickSegment(normColBegin, normColEnd, cell, dst);
					tickSegment(highColBegin, highColEnd, cell, dst);
				}
				for (int tr = normRowBegin; tr < normRowEnd; ++tr)
				{
					dst = cell + tr * rowStep();
					tickSegment(lowColBegin, lowColEnd, cell, dst);
					tickSegment(normColBegin, normColEnd, cell, dst);
					tickSegment(highColBegin, highColEnd, cell, dst);
				}
				for (int tr = highRowBegin; tr < highRowEnd; ++tr)
				{
					dst = cell + tr * rowStep();
					tickSegment(lowColBegin, lowColEnd, cell, dst);
					tickSegment(normColBegin, normColEnd, cell, dst);
					tickSegment(highColBegin, highColEnd, cell, dst);
				}
			}
			++cell;
		}
	}
}
