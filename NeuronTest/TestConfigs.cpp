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
	TEST_EQUAL(config["one"].mType, ConfigItem::FLOAT);
	TEST_EQUAL(config["one"].mFloat, 1.0f);
	TEST_EQUAL(config["two"].mType, ConfigItem::INT);
	TEST_EQUAL(config["two"].mInt, 2);
	TEST_EQUAL(config["thousand"].mFloat, 1000.0f);
}
