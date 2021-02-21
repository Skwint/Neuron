#include "TestNet.h"

#include <memory>

#include "NeuronSim/Cell.h"
#include "NeuronSim/ConfigPresets.h"
#include "NeuronSim/Net.h"
#include "NeuronSim/SpikeTrain.h"

using namespace std;

TestNet::TestNet()
{

}

TestNet::~TestNet()
{

}

static const int TEST_INIT(-12345);
static const int FIRE_ONCE(1);
static const int REST(2);

struct NeuronTest : public Cell
{
	NeuronTest() : test(TEST_INIT) {}
	uint32_t color() { return test; }
	int test;
};

class TestNetLayer : public Net<NeuronTest>
{
public:
	TestNetLayer(int width, int height) : Net<NeuronTest>(width, height) {}
	~TestNetLayer() {}

	static std::string name() { return "test"; }
	static const ConfigPresets & presets();
	void tick(SynapseMatrix * synapses, Spiker * spiker);
	std::string typeName() { return name(); }
	void setConfig(const ConfigSet & config) {}
	ConfigSet getConfig() { ConfigSet config; return config; }
	const ConfigPresets & getPresets();

	NeuronTest & operator[](int index) { return mNeurons[index]; }
	NeuronTest & at(int col, int row) { return mNeurons[row * mWidth + col]; }
};

static inline float hashColRow(int col, int row) { return float(col + 10 * row); }

void TestNet::run()
{
	int width = 64;
	int height = 64;
	auto net = make_shared<TestNetLayer>(width, height);
	SpikeTrain spikeTrain(net, net, 0, false);
	TEST_EQUAL(net->width(), width);
	TEST_EQUAL(net->height(), height);
	TEST_EQUAL(net->at(0, 0).test, TEST_INIT);

	width = 16;
	height = 20;
	net->resize(width, height);
	TEST_EQUAL(net->width(), width);
	TEST_EQUAL(net->height(), height);

	const int size = 9;
	SynapseMatrix synapses(this);
	synapses.setSize(size, size);
	for (int rr = 0; rr < size; ++rr)
	{
		for (int cc = 0; cc < size; ++cc)
		{
			// Set the weight to a pattern based on row and column
			synapses.synapse(cc, rr)->delay = 0;
			synapses.synapse(cc, rr)->weight = hashColRow(cc, rr);
		}
	}
	synapses.setTarget(net);
	synapses.setSource(net);

	// Nothing should happen - our state is boring and we have no spikes
	net->tick(&synapses, &spikeTrain);
	spikeTrain.tick();
	TEST_EQUAL(net->at(1, 2).test, TEST_INIT);
	TEST_EQUAL(net->at(10, 12).test, TEST_INIT);
	TEST_EQUAL(net->at(3, 1).input, 0.0f);
	TEST_EQUAL(net->at(10, 12).input, 0.0f);
	TEST_EQUAL(net->at(12, 11).firing, false);
	TEST_EQUAL(net->at(15, 13).firing, false);

	// top left corner fire with wrap around:
	net->at(0, 0).test = FIRE_ONCE;
	net->tick(&synapses, &spikeTrain);
	spikeTrain.tick();
	TEST_EQUAL(net->at(0, 0).test, REST);
	// wrapping up and left
	for (int rr = 0; rr < 4; ++rr)
	{
		int posr = height - 4 + rr;
		for (int cc = 0; cc < 4; ++cc)
		{
			int posc = width - 4 + cc;
			TEST_EQUAL(net->at(posc, posr).input, hashColRow(cc, rr));
		}
	}
	// wrapping up
	for (int rr = 0; rr < 4; ++rr)
	{
		int posr = height - 4 + rr;
		for (int cc = 0; cc < 4; ++cc)
		{
			int posc = cc + 4;
			TEST_EQUAL(net->at(cc, posr).input, hashColRow(posc, rr));
		}
	}
	// wrapping left
	for (int rr = 0; rr < 5; ++rr)
	{
		int posr = rr + 4;
		for (int cc = 0; cc < 4; ++cc)
		{
			int posc = width - 4 + cc;
			TEST_EQUAL(net->at(posc, rr).input, hashColRow(cc, posr));
		}
	}
	// not wrapping
	for (int rr = 0; rr < 5; ++rr)
	{
		int posr = rr + 4;
		for (int cc = 0; cc < 5; ++cc)
		{
			int posc = cc + 4;
			TEST_EQUAL(net->at(cc, rr).input, hashColRow(posc, posr));
		}
	}

	if (mFails > 0)
	{
		TEST_LOG("Logging entire net:\n" << *net);
	}

	// almost bottom right corner fire with wrap around:
	net->clear();
	int sizer = 7;
	int sizec = 6;
	net->at(width - sizec + size / 2, height - sizer + size / 2).test = FIRE_ONCE;
	net->tick(&synapses, &spikeTrain);
	spikeTrain.tick();
	TEST_EQUAL(net->at(width - sizec + size / 2, height - sizer + size / 2).test, REST);
	// not wrapping
	for (int rr = 0; rr < sizer; ++rr)
	{
		int netr = height - sizer + rr;
		for (int cc = 0; cc < sizec; ++cc)
		{
			int netc = width - sizec + cc;
			TEST_EQUAL(net->at(netc, netr).input, hashColRow(cc, rr));
		}
	}
	// wrapping down
	for (int rr = sizer; rr < size; ++rr)
	{
		int netr = rr - sizer;
		for (int cc = 0; cc < 5; ++cc)
		{
			int netc = width - sizec + cc;
			TEST_EQUAL(net->at(netc, netr).input, hashColRow(cc, rr));
		}
	}
	// wrapping right
	for (int rr = 0; rr < sizer; ++rr)
	{
		int netr = height - sizer + rr;
		for (int cc = sizec; cc < size; ++cc)
		{
			int netc = cc - sizec;
			TEST_EQUAL(net->at(netc, netr).input, hashColRow(cc, rr));
		}
	}
	// wrapping down and right
	for (int rr = sizer; rr < size; ++rr)
	{
		int netr = rr - sizer;
		for (int cc = sizec; cc < size; ++cc)
		{
			int netc = cc - sizec;
			TEST_EQUAL(net->at(netc, netr).input, hashColRow(cc, rr));
		}
	}

	if (mFails > 0)
	{
		TEST_LOG("Logging entire net:\n" << *net);
	}
}

const ConfigPresets & TestNetLayer::getPresets()
{
	return TestNetLayer::presets();
}

const ConfigPresets & TestNetLayer::presets()
{
	static ConfigPresets presets;
	if (presets.configs().empty())
	{
		presets.read(name());
	}
	return presets;
}

void TestNetLayer::tick(SynapseMatrix * synapses, Spiker * spiker)
{
	auto neuron = &at(0, 0);
	for (int rr = 0; rr < mHeight; ++rr)
	{
		for (int cc = 0; cc < mWidth; ++cc)
		{
			neuron->firing = (neuron->test == FIRE_ONCE);
			if (neuron->firing)
			{
				neuron->test = REST;
			}
			++neuron;
		}
	}
	Net::tick(synapses, spiker);
}
