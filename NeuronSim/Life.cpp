#include "Life.h"

#include "Log.h"

Life::Life(int width, int height) :
	Net(width, height),
	mLow(2.25),
	mHigh(3.75)
{
	mConfig = Life::defaultConfig();
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
	mConfig = config;
	for (auto & item : config)
	{
		if (item.name == "low threshold")
		{
			mLow = item.value;
		}
		else if (item.name == "high threshold")
		{
			mHigh = item.value;
		}
		else
		{
			LOG("Unexpected config item [" << item.name << "]");
		}
	}
}

const ConfigSet & Life::defaultConfig()
{
	static ConfigSet config;
	if (config.empty())
	{
		ConfigItem item;
		item.name = "low threshold";
		item.minimum = -1.0f;
		item.maximum = 100.0f;
		item.value = item.def = 2.25;
		config.push_back(item);
		item.name = "high threshold";
		item.value = item.def = 3.75;
		config.push_back(item);
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
	mSpikeProcessor->tick();

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
