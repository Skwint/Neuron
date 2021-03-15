#include "Kumar.h"

#include "ConfigPresets.h"
#include "ConfigSet.h"

static const std::string CFG_V("v");
static const std::string CFG_A("a");
static const std::string CFG_B("b");
static const std::string CFG_C("c");
static const std::string CFG_D("d");

Kumar::Kumar(int width, int height) :
	Net<NeuronKumar>(width, height),
	mV(-10.0f),
	mA(0.02f),
	mB(0.2f),
	mC(0.0f),
	mD(2.0f)
{
	clear();
}

Kumar::~Kumar()
{
}

std::string Kumar::name()
{
	return "Kumar";
}

void Kumar::setConfig(const ConfigSet & config)
{
	mV = config.items().at(CFG_V).mFloat;
	mA = config.items().at(CFG_A).mFloat;
	mB = config.items().at(CFG_B).mFloat;
	mC = config.items().at(CFG_C).mFloat;
	mD = config.items().at(CFG_D).mFloat;
}

const ConfigPresets & Kumar::presets()
{
	static ConfigPresets presets;
	if (presets.configs().empty())
	{
		presets.read(name());
	}
	return presets;
}

ConfigSet Kumar::getConfig()
{
	ConfigSet config;
	config[CFG_V] = mV;
	config[CFG_A] = mA;
	config[CFG_B] = mB;
	config[CFG_C] = mC;
	config[CFG_D] = mD;

	return config;
}

const ConfigPresets & Kumar::getPresets()
{
	return Kumar::presets();
}

void Kumar::clear()
{
	Net<NeuronKumar>::clear();
	for (auto neuron = mNeurons.begin(); neuron != mNeurons.end(); ++neuron)
	{
		neuron->v = mC;
		neuron->u = mB * neuron->v;
	}
}

void Kumar::tick()
{
	NeuronKumar * cell = &mNeurons[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			float v = cell->v;
			float u = cell->u;
			cell->v += exp(v * v) + mV * v - u + cell->input;
			cell->u += mA * (mB * v - u);
			cell->input = 0.0f;
			if (cell->v >= 3)
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
