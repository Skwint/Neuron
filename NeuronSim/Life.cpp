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

inline void Life::tickSegment(int cs, int ce, NeuronLife * cell, NeuronLife * dst, Synapse * synapse)
{
	dst += cs;
	for (int tc = cs; tc < ce; ++tc)
	{
		mSpikeProcessor->fire(&dst->input, synapse->weight, synapse->delay);
		++dst;
		++synapse;
	}
}

void Life::tick(SynapseMatrix * synapses)
{
	for (int rr = 0; rr < mHeight; ++rr)
	{
		NeuronLife * cell = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			assert(cell->potential != neuronLifeCheck);
			assert(cell->input != neuronLifeCheck);
			// Life is an extremely leaky integrator - it leaks 100%
			// on every time step.
			cell->potential = cell->input;
			cell->input = 0.0f;
			++cell;
		}
	}

	// ** TODO **
	// is there any way we can generalize this without resorting to a
	// polymorphic function call on every cell?
	// Maybe? The only bits that change are the integration step above, the
	// decision to fire, and the cell state changes that occur if it fires.
	// The state changes would be OK virtual, but the test needs to be fast.
	// ** TODO **
	for (int rr = 0; rr < mHeight; ++rr)
	{
		int lowRowBegin = synapses->lowWrapRowBegin(rr, mHeight);
		int lowRowEnd = synapses->lowWrapRowEnd(rr, mHeight);
		int normRowBegin = synapses->normRowBegin(rr, mHeight);
		int normRowEnd = synapses->normRowEnd(rr, mHeight);
		int highRowBegin = synapses->highWrapRowBegin(rr, mHeight);
		int highRowEnd = synapses->highWrapRowEnd(rr, mHeight);

		NeuronLife * cell = row(rr);
		NeuronLife * dst;
		for (int cc = 0; cc < mWidth; ++cc)
		{
			// Life was designed in terms of "alive" and "dead" but
			// here we equate "alive" to "firing".
			if (cell->potential > mLow && cell->potential < mHigh)
			{
				int lowColBegin = synapses->lowWrapColBegin(cc, mWidth);
				int lowColEnd = synapses->lowWrapColEnd(cc, mWidth);
				int normColBegin = synapses->normColBegin(cc, mWidth);
				int normColEnd = synapses->normColEnd(cc, mWidth);
				int highColBegin = synapses->highWrapColBegin(cc, mWidth);
				int highColEnd = synapses->highWrapColEnd(cc, mWidth);
				// TODO - these steps are a waste of time we are incrementing the synapse
				// in the tickSegment function and then forgetting and recalculating where
				// it ends up - we should probably inline that function manually.
				int lowStep = max(0, lowColEnd - lowColBegin);
				int normStep = max(0, normColEnd - normColBegin);
				int highStep = max(0, highColEnd - highColBegin);
				Synapse * synapse = synapses->begin();
				for (int tr = lowRowBegin; tr < lowRowEnd; ++tr)
				{
					dst = cell + tr * rowStep();
					tickSegment(lowColBegin, lowColEnd, cell, dst, synapse);
					synapse += lowStep;
					tickSegment(normColBegin, normColEnd, cell, dst, synapse);
					synapse += normStep;
					tickSegment(highColBegin, highColEnd, cell, dst, synapse);
					synapse += highStep;
				}
				for (int tr = normRowBegin; tr < normRowEnd; ++tr)
				{
					dst = cell + tr * rowStep();
					tickSegment(lowColBegin, lowColEnd, cell, dst, synapse);
					synapse += lowStep;
					tickSegment(normColBegin, normColEnd, cell, dst, synapse);
					synapse += normStep;
					tickSegment(highColBegin, highColEnd, cell, dst, synapse);
					synapse += highStep;
				}
				for (int tr = highRowBegin; tr < highRowEnd; ++tr)
				{
					dst = cell + tr * rowStep();
					tickSegment(lowColBegin, lowColEnd, cell, dst, synapse);
					synapse += lowStep;
					tickSegment(normColBegin, normColEnd, cell, dst, synapse);
					synapse += normStep;
					tickSegment(highColBegin, highColEnd, cell, dst, synapse);
					synapse += highStep;
				}
			}
			++cell;
		}
	}
}
