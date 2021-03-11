#include "Control.h"

#include "Control.h"

#include <ctime>

#include "Common.h"
#include "NeuronSim/LayerFactory.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/SynapseMatrix.h"

using namespace std;

static const int totalRandomActions = 1000;
static const int WIDTH = 128;
static const int HEIGHT = 128;
static const string NETWORK_TYPE = "Izhikevich";
static const int NUM_LAYERS = 8;
static const int SYNAPSE_WIDTH = 3;
static const int SYNAPSE_HEIGHT = 3;
static const float MAX_SYNAPSE_WEIGHT = 1.0f;
static const float NOISE_WEIGHT = 20.0f;
static const float NOISE_DENSITY = 0.3f;
static const float NOISE_TIME = 3;
static const float TARGET_SPIKE_DENSITY = 0.05f;
static const float LARGE_CHANGE_CHANCE = 0.05f;
#ifdef NDEBUG
static const int NUM_ITERATIONS = 50;
static const int NUM_EXPERIMENTS = 1000;
#else
static const int NUM_ITERATIONS = 40;
static const int NUM_EXPERIMENTS = 100;
#endif
static const int NUM_EVALUATIONS(10);
static const int MAX_CHANGES(5);

enum Action
{
	ACTION_CHANGE_SYNAPSE_WEIGHT = 0,
	ACTION_CHANGE_CONFIG,
	ACTION_CHANGE_SPIKE_LENGTH,
	ACTION_FLIP_WEIGHT,
	ACTION_ATROPHY,

	ACTION_COUNT
};

Control::Control() :
	mZeroToOne(0.0f, 1.0f),
	mMinusOneToOne(-1.0f, 1.0f),
	mSmallMultiplier(0.9f, 1.1f),
	mSmallOffset(-1, 1),
	mLargeMultiplier(0.5f, 1.5f),
	mLargeOffset(-30, 30)
{
	mCurrent.score = 999999.0f;
	mBest.score = 999999.0f;
}

Control::~Control()
{
}

void Control::run()
{
	uint32_t seed = uint32_t(time(0));
	GEN_LOG("seeding with " << seed);
	mRandom.seed(seed);

	initializeSynapseData();
	createAutomaton();
	initializeConfigData();

	keepCurrent();
	mSolved = false;
	// Try a few experiments:
	for (int exp = 0; exp < NUM_EXPERIMENTS; ++exp)
	{
		// Apply DNA
		applyDna();

		// Make random changes
		changeAutomaton();

		// Initialize with random noise to layer 0:
		applyNoise();

		// Run the simulation and score it
		evaluate();

		// If it's an improvement, keep it
		if (mCurrent.score < mBest.score)
		{
			keepCurrent();
			mSolved = true;
			GEN_LOG("New best score: " << mCurrent.score);
		}
		else if (mSolved)
		{
			restoreBest();
		}

		if (!(exp % 100))
		{
			GEN_LOG(" # " << exp);
		}
	}

	// Save the best configuration we got
	mAutomaton->save("genetic.neuron");
}

void Control::applyDna()
{
	for (int lnum = 0; lnum < NUM_LAYERS; ++lnum)
	{
		auto layer = mAutomaton->layers()[lnum];
		layer->setConfig(mCurrent.layers[lnum].config);
		layer->setSpike(Spike::SHAPE_SQUARE, mCurrent.layers[lnum].spikeDuration);
	}

	while (!mAutomaton->synapses().empty())
	{
		mAutomaton->removeSynapse(mAutomaton->synapses().back());
	}

	// Make synapses
	auto * data = &mCurrent.synapses[0];
	for (int l1 = 0; l1 < NUM_LAYERS; ++l1)
	{
		for (int l2 = 0; l2 < NUM_LAYERS; ++l2)
		{
			auto synapses = mAutomaton->createSynapse();
			synapses->setSize(SYNAPSE_WIDTH, SYNAPSE_HEIGHT);
			synapses->setDelay(SynapseMatrix::DELAY_NONE);
			synapses->setSource(mAutomaton->layers()[l1]);
			synapses->setTarget(mAutomaton->layers()[l2]);
			synapses->loadImage(&data->pixels[0], SYNAPSE_WIDTH, SYNAPSE_HEIGHT, data->weight);
			++data;
		}
	}
}

void Control::evaluate()
{
	for (int tt = 0; tt < NUM_ITERATIONS; ++tt)
	{
		mAutomaton->tick();
	}
	mCurrent.score = 0;
	for (int tt = 0; tt < NUM_EVALUATIONS; ++tt)
	{
		mAutomaton->tick();
		score();
	}
	mCurrent.score /= NUM_EVALUATIONS;
}

void Control::applyNoise()
{
	mAutomaton->clearLayers();
	auto layer = mAutomaton->layers()[0];
	for (int num = 0; num < NOISE_TIME; ++num)
	{
		for (int row = 0; row < mAutomaton->height(); ++row)
		{
			for (int col = 0; col < mAutomaton->width(); ++col)
			{
				if (mZeroToOne(mRandom) < NOISE_DENSITY)
				{
					layer->fire(col, row, NOISE_WEIGHT);
				}
			}
		}
		mAutomaton->tick();
	}
}

void Control::initializeConfigData()
{
	for (auto layer : mAutomaton->layers())
	{
		LayerDna data;
		data.spikeDuration = 2;
		data.config = layer->getConfig();
		mCurrent.layers.push_back(data);
	}
}

void Control::initializeSynapseData()
{
	SynapseDna data;
	data.pixels.resize(SYNAPSE_WIDTH * SYNAPSE_HEIGHT);
	for (int l1 = 0; l1 < NUM_LAYERS; ++l1)
	{
		for (int l2 = 0; l2 < NUM_LAYERS; ++l2)
		{
			data.weight = MAX_SYNAPSE_WEIGHT * float(mMinusOneToOne(mRandom));
			auto * pixel = &data.pixels[0];
			for (int row = 0; row < SYNAPSE_HEIGHT; ++row)
			{
				for (int col = 0; col < SYNAPSE_WIDTH; ++col)
				{
					*pixel = uint32_t(mZeroToOne(mRandom) * 0xFF);
					++pixel;
				}
			}
			mCurrent.synapses.push_back(data);
		}
	}
}

void Control::createAutomaton()
{
	mAutomaton = make_unique<Automaton>();
	mAutomaton->setNetworkType(NETWORK_TYPE);
	mAutomaton->setSize(WIDTH, HEIGHT);

	// Make layers
	for (int lnum = 0; lnum < NUM_LAYERS; ++lnum)
	{
		mAutomaton->createLayer();
	}
}

void Control::changeAutomaton()
{
	int numChanges = mRandom() % MAX_CHANGES;
	for (int change = 0; change < numChanges; ++change)
	{
		int action = mRandom() % ACTION_COUNT;
		bool large = mZeroToOne(mRandom) < LARGE_CHANGE_CHANCE;
		float multi;
		int offset;
		if (large)
		{
			multi = float(mLargeMultiplier(mRandom));
			offset = mLargeOffset(mRandom);
		}
		else
		{
			multi = float(mSmallMultiplier(mRandom));
			offset = mSmallOffset(mRandom);
		}
		switch (action)
		{
		case ACTION_CHANGE_SYNAPSE_WEIGHT:
		{
			int synapse_index = mRandom() % mAutomaton->synapses().size();
			mCurrent.synapses[synapse_index].weight *= multi;
			mAutomaton->synapses()[synapse_index]->loadImage(&mCurrent.synapses[synapse_index].pixels[0], SYNAPSE_WIDTH, SYNAPSE_HEIGHT, mCurrent.synapses[synapse_index].weight);
			break;
		}
		case ACTION_CHANGE_CONFIG:
		{
			bool ok = false;
			while (!ok)
			{
				int layer_index = mRandom() % NUM_LAYERS;
				int item_index = mRandom() % mCurrent.layers[layer_index].config.items().size();
				auto item = mCurrent.layers[layer_index].config.items().begin();
				while (item_index != 0)
				{
					++item;
					--item_index;
				}
				if (item->second.mType == ConfigItem::FLOAT)
				{
					item->second.mFloat *= multi;
					ok = true;
				}
			}
			break;
		}
		case ACTION_CHANGE_SPIKE_LENGTH:
		{
			int layer_index = mRandom() % NUM_LAYERS;
			int duration = mAutomaton->layers()[layer_index]->spikeDuration();
			duration = clamp(duration + offset, 1, 8);
			mAutomaton->layers()[layer_index]->setSpike(Spike::SHAPE_SQUARE, duration);
			break;
		}
		case ACTION_FLIP_WEIGHT:
		{
			int synapse_index = mRandom() % mAutomaton->synapses().size();
			mCurrent.synapses[synapse_index].weight *= -1.0f;
			mAutomaton->synapses()[synapse_index]->loadImage(&mCurrent.synapses[synapse_index].pixels[0], SYNAPSE_WIDTH, SYNAPSE_HEIGHT, mCurrent.synapses[synapse_index].weight);
			break;
		}
		case ACTION_ATROPHY:
		{
			int synapse_index = mRandom() % mAutomaton->synapses().size();
			mAutomaton->removeSynapse(mAutomaton->synapses()[synapse_index]);
			break;
		}
		default:
			GEN_LOG("Impossible action " << action);
			break;
		}
	}
}

void Control::score()
{
	float density = mAutomaton->currentSpikeDensity();
	if (density < 0.000001f)
	{
		mCurrent.score += 999999.0f;
	}
	else
	{
		float score = density - TARGET_SPIKE_DENSITY;
		mCurrent.score += score * score;
	}
}

void Control::keepCurrent()
{
	mBest = mCurrent;
}

void Control::restoreBest()
{
	mCurrent = mBest;
}
