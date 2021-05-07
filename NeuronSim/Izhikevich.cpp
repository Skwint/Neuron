#include "Izhikevich.h"

#include "ConfigPresets.h"
#include "ConfigSet.h"

using namespace std;

static const string CFG_A("a");
static const string CFG_B("b");
static const string CFG_C("c");
static const string CFG_D("d");

static const float V2(0.04f);
static const float V1(5.0f);
static const float V0(140.0f);

Izhikevich::Izhikevich(int width, int height) :
	Net<NeuronIzhikevich>(width, height),
	mA(0.02f),
	mB(0.2f),
	mC(-65.0f),
	mD(2.0f)
{
	clear();
}

Izhikevich::Izhikevich(const Izhikevich & other) :
	Net<NeuronIzhikevich>(other),
	mA(other.mA),
	mB(other.mB),
	mC(other.mC),
	mD(other.mD)
{
}

Izhikevich::~Izhikevich()
{
}

string Izhikevich::name()
{
	return "Izhikevich";
}

void Izhikevich::setConfig(const ConfigSet & config)
{
	mA = config.items().at(CFG_A).mFloat;
	mB = config.items().at(CFG_B).mFloat;
	mC = config.items().at(CFG_C).mFloat;
	mD = config.items().at(CFG_D).mFloat;
}

const ConfigPresets & Izhikevich::presets()
{
	static ConfigPresets presets;
	if (presets.configs().empty())
	{
		presets.read(name());
	}
	return presets;
}

ConfigSet Izhikevich::getConfig()
{
	ConfigSet config;
	config[CFG_A] = mA;
	config[CFG_B] = mB;
	config[CFG_C] = mC;
	config[CFG_D] = mD;

	return config;
}

const ConfigPresets & Izhikevich::getPresets()
{
	return Izhikevich::presets();
}

void Izhikevich::clear()
{
	Net<NeuronIzhikevich>::clear();
	for (auto neuron = mNeurons.begin(); neuron != mNeurons.end(); ++neuron)
	{
		// This is one of the two solutions for a stable state
		// The alternative is +sqrtf(val)
		// We expect the lower value to be the properly stable point, and the
		// higher value to be an unstable excited state
		// It doesn't work very well and we have abandoned it for now.
		//float val = fabs((mV1 + mB) * (mV1 + mB) - 4.0f * mV2 * mV0);
		//neuron->v = mB - mV1 - sqrtf(val);

		neuron->v = mC;
		neuron->u = mB * mC;
	}
}

void Izhikevich::tick()
{
	processDendrites();

	NeuronIzhikevich * cell = &mNeurons[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			float v = cell->v;
			float u = cell->u;
			cell->v += V2 * v * v + V1 * v + V0 - u + cell->input;
			cell->u += mA * (mB * cell->v - u); // use of new version of V intentional
			cell->input = 0.0f;
			if (cell->v >= 30)
			{
				cell->v = mC;
				cell->u = cell->u + mD;
				cell->firing = true;
			}
			else
			{
				cell->firing = false;
			}

			++cell;
		}
	}
}

void Izhikevich::paintState(uint32_t * image)
{
	// We draw the reset variable instead of the potential here.
	// The potential tends to a lot more short lived and less indicative of
	// a contiuously changing state than u.
	for (auto cell = begin(); cell != end(); ++cell)
	{
		uint32_t col = uint32_t(clamp((128.0f + 2.0f * cell->v), 0.0f, 255.0f));
		*image++ = 0xFF000000 | col | (col << 8) | (col << 16);
	}
}

