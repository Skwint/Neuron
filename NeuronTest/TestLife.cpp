#include "TestLife.h"

#include "NeuronSim/Layer.h"
#include "NeuronSim/Life.h"
#include "NeuronSim/SynapseMatrix.h"

using namespace std;

TestLife::TestLife()
{
	mAutomaton = make_unique<Automaton>();
	mAutomaton->addListener(this);
}

TestLife::~TestLife()
{
	mAutomaton->removeListener(this);
}

void TestLife::run()
{
	Test::run();

	testBasicLife();
	testInterleavedLife();
	testSaveLoad();
}

// Uses the Life network type to run a completely normal
// version of conways game of life.
void TestLife::testBasicLife()
{
	TEST_SUB;

	// Initialize a 5x5 life layer
	mAutomaton->reset();
	mAutomaton->setNetworkType("Life");
	mAutomaton->setSize(5, 5);
	auto layer = mAutomaton->createLayer();
	auto synapse = mAutomaton->createSynapse();
	synapse->setSource(layer);
	synapse->setTarget(layer);
	uint32_t syn[] =
	{
		0xFF, 0xFF, 0xFF,
		0xFF, 0x80, 0xFF,
		0xFF, 0xFF, 0xFF,
	};
	synapse->loadImage(syn, 3, 3, 1.0f);

	// Create a glider in the middle of the layer
	float weight = 3.0f;
	layer->inject(1, 1, weight);
	layer->inject(2, 1, weight);
	layer->inject(3, 1, weight);
	layer->inject(3, 2, weight);
	layer->inject(2, 3, weight);

	const uint32_t live(0xFFFFFFFF);
	const uint32_t dead(0xFF000000);
	std::vector<uint32_t> image;
	image.resize(25);

	// Run for one step so that the spikes arrive
	mAutomaton->tick();
	std::vector<uint32_t> gliderStart =
	{
		dead, dead, dead, dead, dead,
		dead, live, live, live, dead,
		dead, dead, dead, live, dead,
		dead, dead, live, dead, dead,
		dead, dead, dead, dead, dead
	};
	layer->paintState(&image[0]);

	TEST(image == gliderStart);

	// Run for one step to reach the next state
	mAutomaton->tick();
	std::vector<uint32_t> gliderNext =
	{
		dead, dead, live, dead, dead,
		dead, dead, live, live, dead,
		dead, live, dead, live, dead,
		dead, dead, dead, dead, dead,
		dead, dead, dead, dead, dead
	};
	layer->paintState(&image[0]);

	TEST(image == gliderNext);

	// A glider repeats itself every 4 steps, but moves.
	// After 5x4 steps it should be back at the start if
	// wrap around is working:
	for (int ii = 0; ii < 19; ++ii)
	{
		mAutomaton->tick();
	}
	layer->paintState(&image[0]);
	TEST(image == gliderStart);

}

// Uses two layers with two synapse matrices linking them in 
// both directions but no synapses within the layers
// If everything is working it should be able to run two 
// games of life at the same time, but with each one hopping
// back and forth between the two layers.
// This test excercises a lot of the program logic.
void TestLife::testInterleavedLife()
{
	TEST_SUB;

	// Initialize a 5x5 life layer
	mAutomaton->reset();
	mAutomaton->setNetworkType("Life");
	mAutomaton->setSize(5, 5);
	auto layer1 = mAutomaton->createLayer();
	auto layer2 = mAutomaton->createLayer();
	auto synapses1 = mAutomaton->createSynapse();
	auto synapses2 = mAutomaton->createSynapse();
	synapses1->setSource(layer1);
	synapses1->setTarget(layer2);
	synapses2->setSource(layer2);
	synapses2->setTarget(layer1);
	uint32_t syn[] =
	{
		0xFF, 0xFF, 0xFF,
		0xFF, 0x80, 0xFF,
		0xFF, 0xFF, 0xFF,
	};
	synapses1->loadImage(syn, 3, 3, 1.0f);
	synapses2->loadImage(syn, 3, 3, 1.0f);

	layer1->setSpike(Spike::SHAPE_SQUARE, 1);
	layer2->setSpike(Spike::SHAPE_SQUARE, 1);

	// Create a glider in the middle of the first layer
	float weight = 3.0f;
	layer1->inject(1, 1, weight);
	layer1->inject(2, 1, weight);
	layer1->inject(3, 1, weight);
	layer1->inject(3, 2, weight);
	layer1->inject(2, 3, weight);

	// Create a windmill in the second layer
	layer2->inject(2, 1, weight);
	layer2->inject(2, 2, weight);
	layer2->inject(2, 3, weight);

	const uint32_t live(0xFFFFFFFF);
	const uint32_t dead(0xFF000000);
	std::vector<uint32_t> image;
	image.resize(25);

	// Run for one step so that the spikes arrive
	mAutomaton->tick();
	std::vector<uint32_t> gliderStart =
	{
		dead, dead, dead, dead, dead,
		dead, live, live, live, dead,
		dead, dead, dead, live, dead,
		dead, dead, live, dead, dead,
		dead, dead, dead, dead, dead
	};
	std::vector<uint32_t> windmillStart =
	{
		dead, dead, dead, dead, dead,
		dead, dead, live, dead, dead,
		dead, dead, live, dead, dead,
		dead, dead, live, dead, dead,
		dead, dead, dead, dead, dead
	};
	layer1->paintState(&image[0]);
	TEST(image == gliderStart);
	layer2->paintState(&image[0]);
	TEST(image == windmillStart);

	// Run for one step to reach the next state
	mAutomaton->tick();
	std::vector<uint32_t> gliderNext =
	{
		dead, dead, live, dead, dead,
		dead, dead, live, live, dead,
		dead, live, dead, live, dead,
		dead, dead, dead, dead, dead,
		dead, dead, dead, dead, dead
	};
	std::vector<uint32_t> windmillNext =
	{
		dead, dead, dead, dead, dead,
		dead, dead, dead, dead, dead,
		dead, live, live, live, dead,
		dead, dead, dead, dead, dead,
		dead, dead, dead, dead, dead
	};
	layer1->paintState(&image[0]);
	TEST(image == windmillNext);
	layer2->paintState(&image[0]);
	TEST(image == gliderNext);

	// One more step for good measure, and just check the windmill
	mAutomaton->tick();
	layer2->paintState(&image[0]);
	TEST(image == windmillStart);
}

void TestLife::testSaveLoad()
{
	TEST_SUB;

	// Initialize a 5x5 life layer
	mAutomaton->reset();
	mAutomaton->setNetworkType("Life");
	mAutomaton->setSize(5, 5);
	auto layer = mAutomaton->createLayer();
	auto synapse = mAutomaton->createSynapse();
	synapse->setSource(layer);
	synapse->setTarget(layer);
	uint32_t syn[] =
	{
		0xFF, 0xFF, 0xFF,
		0xFF, 0x80, 0xFF,
		0xFF, 0xFF, 0xFF,
	};
	synapse->loadImage(syn, 3, 3, 1.0f);

	// Create a glider in the middle of the layer
	float weight = 3.0f;
	layer->inject(1, 1, weight);
	layer->inject(2, 1, weight);
	layer->inject(3, 1, weight);
	layer->inject(3, 2, weight);
	layer->inject(2, 3, weight);

	const uint32_t live(0xFFFFFFFF);
	const uint32_t dead(0xFF000000);
	std::vector<uint32_t> image;
	image.resize(25);

	// Run for one step so that the spikes arrive
	mAutomaton->tick();
	std::vector<uint32_t> gliderStart =
	{
		dead, dead, dead, dead, dead,
		dead, live, live, live, dead,
		dead, dead, dead, live, dead,
		dead, dead, live, dead, dead,
		dead, dead, dead, dead, dead
	};
	layer->paintState(&image[0]);
	TEST(image == gliderStart);

	// Save state to file
	mAutomaton->save("Data/Test/Life.neuron");

	// Load it
	mAutomaton->clearLayers();
	mAutomaton->load("Data/Test/Life.neuron");
	layer = mAutomaton->layers()[0];

	// Tick to the next state
	mAutomaton->tick();
	std::vector<uint32_t> gliderNext =
	{
		dead, dead, live, dead, dead,
		dead, dead, live, live, dead,
		dead, live, dead, live, dead,
		dead, dead, dead, dead, dead,
		dead, dead, dead, dead, dead
	};
	layer->paintState(&image[0]);
	TEST(image == gliderNext);

	// Destroy a layer which was loaded (regression test for a crash)
	mAutomaton->removeLayer(layer);
}