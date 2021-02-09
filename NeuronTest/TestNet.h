#ifndef TEST_NET_H
#define TEST_NET_H

#include "Test.h"
class TestNet : public Test
{
public:
	TestNet();
	~TestNet();

	std::string name() { return "Net"; }
	void run();
};

#endif
