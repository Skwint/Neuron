#ifndef TEST_CONFIGS_H
#define TEST_CONFIGS_H

#include "Test.h"

class TestConfigs : public Test
{
public:
	TestConfigs();
	~TestConfigs();

	std::string name() { return "Configs"; }
	void run();
};

#endif


