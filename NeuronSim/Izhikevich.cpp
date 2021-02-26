#include "Izhikevich.h"

#include "ConfigPresets.h"
#include "ConfigSet.h"

static const std::string CFG_V2("v2");
static const std::string CFG_V1("v1");
static const std::string CFG_V0("v0");
static const std::string CFG_A("a");
static const std::string CFG_B("b");
static const std::string CFG_C("c");
static const std::string CFG_D("d");

Izhikevich::Izhikevich(int width, int height) :
	Net<NeuronIzhikevich>(width, height),
	mV2(0.04f),
	mV1(5.0f),
	mV0(140.0f),
	mA(0.02f),
	mB(0.2f),
	mC(-65.0f),
	mD(2.0f)
{
	clear();
}

Izhikevich::~Izhikevich()
{
}

std::string Izhikevich::name()
{
	return "Izhikevich";
}

void Izhikevich::setConfig(const ConfigSet & config)
{
	mV2 = config.items().at(CFG_V2).mFloat;
	mV1 = config.items().at(CFG_V1).mFloat;
	mV0 = config.items().at(CFG_V0).mFloat;
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
	config[CFG_V2] = mV2;
	config[CFG_V1] = mV1;
	config[CFG_V0] = mV0;
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
		float val = fabs((mV1 + mB) * (mV1 + mB) - 4.0f * mV2 * mV0);
		neuron->v = mB - mV1 - sqrtf(val);
		neuron->u = mB * neuron->v;
	}
}

void Izhikevich::postTick()
{
	NeuronIzhikevich * cell = &mNeurons[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			float v = cell->v;
			float u = cell->u;
			cell->v += mV2 * v * v + mV1 * v + mV0 - u + cell->input;
			cell->u += mA * (mB * v - u);
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
