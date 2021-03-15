#include "TestPerformance.h"

#include <chrono>
#include <ctime>

#include "NeuronSim/Layer.h"
#include "NeuronSim/Life.h"
#include "NeuronSim/SynapseMatrix.h"

using namespace std;

static const int numTicks(1000);

TestPerformance::TestPerformance()
{
	mAutomaton = make_unique<Automaton>();
	mAutomaton->addListener(this);
}

TestPerformance::~TestPerformance()
{
	mAutomaton->removeListener(this);
}

void TestPerformance::run()
{
	Test::run();

	// Load the performance automaton
	// Currently this is 8 layers, 6 excitatory, 1 inhibitory and 1 shunting,
	// connected by 18 3x3 synapse matrices.
	// This can always be changed by replacing the files
	mAutomaton->load("Data/Saves/performance.neuron");

	// Run 10 frames to get going
	for (int startup = 0; startup < 10; ++startup)
	{
		mAutomaton->tick();
	}

	uint64_t numLayers = mAutomaton->layers().size();
	uint64_t layerSize = mAutomaton->width() * mAutomaton->height();
	uint64_t numNeurons = numLayers * layerSize;

	uint64_t numSynapses = 0;
	for (auto & synapse : mAutomaton->synapses())
	{
		numSynapses += synapse->width() * synapse->height();
	}
	numSynapses *= layerSize;

	// Time a lot of frames and report the result
	// We time allocated cpu time rather than wall clock time, which will
	// make the results more stable under varying system load.
	//
	// For optimization purposes this test case should still be used, but
	// the results of a profiler should be taken in preference to the time
	// reported by this loop.
	auto clockStart = clock();

	for (int tick = 0; tick < numTicks; ++tick)
	{
		mAutomaton->tick();
	}

	auto clockEnd = clock();
	auto cpuTime = 1000.0 * (clockEnd - clockStart) / CLOCKS_PER_SEC;

	TEST_LOG(numTicks << " performance ticks");
	TEST_LOG(numNeurons << " neurons");
	TEST_LOG(numSynapses << " synapses");
	TEST_LOG("  CPU time           : " << cpuTime << " ms");
	TEST_LOG("  CPU time per tick  : " << (cpuTime / numTicks) << " ms");
	TEST_LOG("  Neurons per second : " << numNeurons * numTicks * 1000u / cpuTime);
	TEST_LOG("  Synapses per second: " << numSynapses * numTicks * 1000u / cpuTime);

	TEST(true); // Give the tester logic something to keep it happy.
}
