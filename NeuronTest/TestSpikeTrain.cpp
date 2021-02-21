#include "TestSpikeTrain.h"

#include "NeuronSim/SpikeTrain.h"
#include "NeuronSim/Life.h"

using namespace std;

TestSpikeTrain::TestSpikeTrain()
{
	mLayer = make_shared<Life>(1,1);
}

TestSpikeTrain::~TestSpikeTrain()
{
}

void TestSpikeTrain::run()
{
	Test::run();

	testSpike();
	testClear();
	testCircularBuffer();
}

// Fires a spike and verifies that the spike is received by the target
// in the expected manner and time.
void TestSpikeTrain::testSpike()
{
	mLayer->first()->input = 0.0f;
	float expected[] = { 0.0f, 0.5f / 1.5f, 2.0f / 1.5f, 2.5f / 1.5f, 2.5f / 1.5f };

	SpikeTrain proc(mLayer, mLayer, 4, false);
	Spike spike;
	spike.setSpike(Spike::SHAPE_TRIANGLE, 3);
	proc.fire(spike, 0, 1.0f, 0);

	for (auto expect : expected)
	{
		TEST_APPROX_EQUAL(mLayer->first()->input, expect);
		proc.tick();
	}
}

// Checks that spikes are removed when the processor is cleared
void TestSpikeTrain::testClear()
{
	mLayer->first()->input = 0.0f;
	float expected[] = { 0.0f, 0.0f, 0.0f };

	SpikeTrain proc(mLayer, mLayer, 4, false);
	Spike spike;
	spike.setSpike(Spike::SHAPE_SQUARE, 1);
	proc.fire(spike, 0, 1.0f, 1);
	proc.clear();

	for (auto expect : expected)
	{
		TEST_APPROX_EQUAL(mLayer->first()->input, expect);
		proc.tick();
	}
}

// Run for enough ticks that the buffer has to wrap around and
// verify that spikes are still working.
// Verify that old spikes do not "resurrect".
void TestSpikeTrain::testCircularBuffer()
{
	mLayer->first()->input = 0.0f;
	float expect[] = { 0.0f, 0.25f, 1.0f, 1.75f, 2.0f, 2.0f };

	const int frameSize(10);
	SpikeTrain proc(mLayer, mLayer, frameSize, false);
	Spike spike;
	spike.setSpike(Spike::SHAPE_TRIANGLE, 4);

	// Fire a spike right at the start (which we don't care about)
	// to make sure it isn't still there when we loop
	proc.fire(spike, 0, 1.0f, 1);

	// Loop till nearly the end of the buffer
	for (int tt = 0; tt < frameSize - 2; ++tt)
	{
		proc.tick();
	}

	// Fire a spike that wraps around the buffer
	mLayer->first()->input = 0.0f;
	proc.fire(spike, 0, 1.0f, 0);
	for (int tt = 0; tt < sizeof(expect) / sizeof(float); ++tt)
	{
		TEST_APPROX_EQUAL(mLayer->first()->input, expect[tt]);
		proc.tick();
	}
}
