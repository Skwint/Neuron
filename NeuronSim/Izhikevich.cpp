#include "Izhikevich.h"

Izhikevich::Izhikevich(int width, int height) :
	Net<NeuronIzhikevich>(width, height)
{
	mConfig = Izhikevich::defaultConfig();
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
	mConfig = config;
}

const ConfigSet & Izhikevich::defaultConfig()
{
	static ConfigSet config;
	if (config.empty())
	{
		ConfigItem item;
		item.name = "v2";
		item.minimum = -1000.0f;
		item.maximum = 1000.0f;
		item.value = item.def = 0.04f;
		config.push_back(item);
		item.name = "v1";
		item.value = item.def = 5.0f;
		config.push_back(item);
		item.name = "v0";
		item.value = item.def = 140.0f;
		config.push_back(item);
		item.name = "a";
		item.value = item.def = 0.02f;
		config.push_back(item);
		item.name = "b";
		item.value = item.def = 0.2f;
		config.push_back(item);
		item.name = "c";
		item.value = item.def = -65.0f;
		config.push_back(item);
		item.name = "d";
		item.value = item.def = 2.0f;
		config.push_back(item);
	}
	return config;
}

void Izhikevich::tick()
{
}
