#ifndef TEST_SPIKE_PROCESSOR_H
#define TEST_SPIKE_PROCESSOR_H

#include "Test.h"

class TestSpikeProcessor : public Test
{
public:
	TestSpikeProcessor();
	~TestSpikeProcessor();

	std::string name() { return "SpikeProcessor"; }
	void run();

private:
	void testSpike();
	void testClear();
	void testCircularBuffer();
};

#endif
