#include "TestStability.h"

#include <ctime>

#include "NeuronSim/Layer.h"
#include "NeuronSim/SynapseMatrix.h"

using namespace std;

static const int totalRandomActions = 1000;

TestStability::TestStability()
{
	mAutomaton = make_unique<Automaton>();
	mAutomaton->addListener(this);
}

TestStability::~TestStability()
{
	mAutomaton->removeListener(this);
}

void TestStability::run()
{
	Test::run();
	uint32_t seed = uint32_t(time(0));
	TEST_LOG("seeding with " << seed);
	mRandom.seed(seed);

	for (int num = 0; num < totalRandomActions; ++num)
	{
		int action = mRandom() % ACTION_COUNT;
		switch (action)
		{
		case CREATE_LAYER:
		{
			int expect = int(mLayers.size()) + 1;
			mAutomaton->createLayer();
			TEST_EQUAL(int(mLayers.size()), expect);
			break;
		}
		case REMOVE_LAYER:
		{
			if (!mLayers.empty())
			{
				int expect = int(mLayers.size()) - 1;
				int index = mRandom() % mLayers.size();
				mAutomaton->removeLayer(mLayers[index]);
				TEST_EQUAL(int(mLayers.size()), expect);
			}
			break;
		}
		case CREATE_SYNAPSES:
		{
			if (!mLayers.empty())
			{
				int expect = int(mSynapses.size() + 1);
				auto synapses = mAutomaton->createSynapse();
				TEST_EQUAL(int(mSynapses.size()), expect);
				int l1 = mRandom() % mLayers.size();
				int l2 = mRandom() % mLayers.size();
				synapses->setSource(mLayers[l1]);
				synapses->setTarget(mLayers[l2]);
			}
			break;
		}
		case REMOVE_SYNAPSES:
		{
			if (!mSynapses.empty())
			{
				int expect = int(mSynapses.size()) - 1;
				int index = mRandom() % mSynapses.size();
				mAutomaton->removeSynapse(mSynapses[index]);
				TEST_EQUAL(int(mSynapses.size()), expect);
			}
			break;
		}
		case RESIZE:
		{
			int bias = max(32, num * 128 / totalRandomActions);
			int width = bias + mRandom() % bias;
			int height = bias + mRandom() % bias;
			mAutomaton->setSize(width, height);
			TEST_EQUAL(mWidth, width);
			TEST_EQUAL(mHeight, height);
			break;
		}
		case STEP:
		{
			mAutomaton->tick();
			mAutomaton->tick();
			mAutomaton->tick();
			break;
		}
		default:
			TEST_FAIL("Action not recognised");
			break;
		}
	}
}

void TestStability::automatonTypeChanged()
{
}

void TestStability::automatonSizeChanged(int width, int height)
{
	mWidth = width;
	mHeight = height;
}

void TestStability::automatonLayerCreated(std::shared_ptr<Layer> layer)
{
	mLayers.push_back(layer);
}

void TestStability::automatonLayerRemoved(std::shared_ptr<Layer> layer)
{
	mLayers.erase(remove(mLayers.begin(), mLayers.end(), layer), mLayers.end());
}

void TestStability::automatonSynapsesCreated(std::shared_ptr<SynapseMatrix> synapses)
{
	mSynapses.push_back(synapses);
}

void TestStability::automatonSynapsesRemoved(std::shared_ptr<SynapseMatrix> synapses)
{
	mSynapses.erase(remove(mSynapses.begin(), mSynapses.end(), synapses), mSynapses.end());
}

