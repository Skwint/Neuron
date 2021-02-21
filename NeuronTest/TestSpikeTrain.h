#ifndef TEST_SPIKE_TRAIN_H
#define TEST_SPIKE_TRAIN_H

#include "Test.h"

class Life;

class TestSpikeTrain : public Test
{
public:
	TestSpikeTrain();
	~TestSpikeTrain();

	std::string name() { return "SpikeTrain"; }
	void run();

private:
	void testSpike();
	void testClear();
	void testCircularBuffer();

private:
	std::shared_ptr<Life> mLayer;
};

#endif
