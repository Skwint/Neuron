#include "Izhikevich.h"

#include "ConfigItem.h"

static const std::string CFG_V2("v2");
static const std::string CFG_V1("v1");
static const std::string CFG_V0("v0");
static const std::string CFG_A("a");
static const std::string CFG_B("b");
static const std::string CFG_C("c");
static const std::string CFG_D("d");

Izhikevich::Izhikevich(int width, int height) :
	Net<NeuronIzhikevich>(width, height)
{
	setConfig(Izhikevich::defaultConfig());
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
	mV2 = config.at(CFG_V2).value;
	mV1 = config.at(CFG_V1).value;
	mV0 = config.at(CFG_V0).value;
	mA = config.at(CFG_A).value;
	mB = config.at(CFG_B).value;
	mC = config.at(CFG_C).value;
	mD = config.at(CFG_D).value;
}

const ConfigSet & Izhikevich::defaultConfig()
{
	static ConfigSet config;
	if (config.empty())
	{
		config[CFG_V2] = 0.04f;
		config[CFG_V1] = 5.0f;
		config[CFG_V0] = 140.0f;
		config[CFG_A] = 0.02f;
		config[CFG_B] = 0.2f;
		config[CFG_C] = -65.0f;
		config[CFG_D] = 2.0f;
	}
	return config;
}

// Should these config item names be defined as constants somewhere?
// Yes, probably! TODO
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

void Izhikevich::tick(SynapseMatrix * synapses)
{
}
