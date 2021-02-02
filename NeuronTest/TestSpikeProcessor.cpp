#include "TestSpikeProcessor.h"

#include "NeuronSim/SpikeProcessor.h"

TestSpikeProcessor::TestSpikeProcessor()
{
}

TestSpikeProcessor::~TestSpikeProcessor()
{
}

void TestSpikeProcessor::run()
{
	Test::run();

	testSpike();
	testClear();
	testCircularBuffer();
}

// Fires a spike and verifies that the spike is received by the target
// in the expected manner and time.
void TestSpikeProcessor::testSpike()
{
	float target = 0.0f;
	float expected[] = { 0.0f, 0.0f, 1.0f, 3.0f, 3.5f, 3.5f };
	SpikeProcessor::Spike spike;
	spike = { 1.0, 2.0, 0.5 };

	SpikeProcessor proc;
	proc.setSpike(spike);
	proc.fire(&target, 1.0f, 1);

	for (auto expect : expected)
	{
		TEST_APPROX_EQUAL(target, expect);
		proc.tick();
	}
}

// Checks that spikes are removed when the processor is cleared
void TestSpikeProcessor::testClear()
{
	float target = 0.0f;
	float expected[] = { 0.0f, 0.0f, 0.0f };
	SpikeProcessor::Spike spike;
	spike = { 1.0 };

	SpikeProcessor proc;
	proc.setSpike(spike);
	proc.fire(&target, 1.0f, 1);
	proc.clear();

	for (auto expect : expected)
	{
		TEST_APPROX_EQUAL(target, expect);
		proc.tick();
	}
}

// Run for enough ticks that the buffer has to wrap around and
// verify that spikes are still working.
// Verify that old spikes do not "resurrect".
void TestSpikeProcessor::testCircularBuffer()
{
	float target = 0.0f;

	SpikeProcessor::Spike spike;
	spike = { 0.1f, 0.2f, 0.3f, 0.4f };
	float expect[] = { 0.0f, 0.1f, 0.3f, 0.6f, 1.0f };

	SpikeProcessor proc;
	proc.setSpike(spike);

	// Fire a spike right at the start (which we don't care about)
	// to make sure it isn't still there when we loop
	proc.fire(&target, 1.0f, 1);

	// Loop till nearly the end of the buffer
	for (int tt = 0; tt < MAX_SPIKE_DELAY - 2; ++tt)
	{
		proc.tick();
	}

	// Fire a spike that wraps around the buffer
	target = 0;
	proc.fire(&target, 1.0f, 0);
	for (int tt = 0; tt < sizeof(expect) / sizeof(float); ++tt)
	{
		TEST_APPROX_EQUAL(target, expect[tt]);
		proc.tick();
	}
}
