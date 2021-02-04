#include "TestConfigs.h"

#include "NeuronSim/ConfigPresets.h"

TestConfigs::TestConfigs()
{

}

TestConfigs::~TestConfigs()
{

}

void TestConfigs::run()
{
	ConfigPresets presets;
	presets.read("test");

	TEST_EQUAL(int(presets.configs().size()), 1);
	auto & config = presets["test1"];
	TEST_EQUAL(int(config.items().size()), 3);
	TEST_EQUAL(config.name(), std::string("test1"));
	TEST_EQUAL(config["one"].value, 1.0f);
	TEST_EQUAL(config["two"].value, 2.0f);
	TEST_EQUAL(config["thousand"].value, 1000.0f);
}
