#include "Automaton.h"

#include <cassert>

#include "Constants.h"
#include "Exception.h"
#include "Layer.h"
#include "Log.h"
#include "LayerFactory.h"
#include "SpikeProcessor.h"
#include "SynapseMatrix.h"

using namespace std;

bool Automaton::Lock::mLocked(false);

Automaton::Automaton() :
	mType("Life"),
	mWidth(DEFAULT_NET_SIZE),
	mHeight(DEFAULT_NET_SIZE)
{
	mLayerFactory = std::make_unique<LayerFactory>();
	mSpikeProcessor = std::make_shared<SpikeProcessor>();
}

Automaton::~Automaton()
{

}

void Automaton::tick()
{
	mSpikeProcessor->tick();

	for (auto synapses : mSynapses)
	{
		synapses->target()->tick(synapses.get());
	}
}

void Automaton::addListener(Listener * listener)
{
	Lock lock;
	mListeners.push_back(listener);
}

void Automaton::removeListener(Listener * listener)
{
	Lock lock;
	mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
}

void Automaton::setNetworkType(const std::string & type)
{
	if (type != mType)
	{
		LOG("Changing network type to [" << type << "]");
		mSpikeProcessor->clear();
		mSynapses.clear();
		Lock lock;
		for (auto layer : mLayers)
		{
			for (auto listener : mListeners)
			{
				listener->automatonLayerRemoved(layer);
			}
		}
		mLayers.clear();
		mType = type;
		for (auto listener : mListeners)
		{
			listener->automatonTypeChanged();
		}
	}
}

std::shared_ptr<Layer> Automaton::createLayer()
{
	auto layer = mLayerFactory->create(mType, mWidth, mHeight);
	layer->setSpikeProcessor(mSpikeProcessor);
	mLayers.push_back(layer);

	Lock lock;
	for (auto listener : mListeners)
	{
		listener->automatonLayerCreated(layer);
	}
	return layer;
}

void Automaton::removeLayer(std::shared_ptr<Layer> layer)
{
	// Repeatedly search for synapses referencing this layer and remove them
	bool keepTrying = true;
	while (keepTrying && !mSynapses.empty())
	{
		keepTrying = false;
		auto zombie = std::find_if(mSynapses.begin(), mSynapses.end(), [layer](auto synapses)
		{
			return (!synapses->source()) || (synapses->source() == layer || (!synapses->target()) || synapses->target() == layer);
		});
		if (zombie != mSynapses.end())
		{
			removeSynapse(*zombie);
			keepTrying = true;
		}
	}

	// Remove the layer
	mLayers.erase(std::remove(mLayers.begin(), mLayers.end(), layer), mLayers.end());

	// Tell listeners about the removed layer
	Lock lock;
	for (auto listener : mListeners)
	{
		listener->automatonLayerRemoved(layer);
	}
}

std::shared_ptr<SynapseMatrix> Automaton::createSynapse()
{
	if (!mLayers.empty())
	{
		auto synapses = make_shared<SynapseMatrix>();
		synapses->setSource(mLayers[0]);
		synapses->setTarget(mLayers[0]);
		mSynapses.push_back(synapses);
		Lock lock;
		for (auto listener : mListeners)
		{
			listener->automatonSynapsesCreated(synapses);
		}
		return synapses;
	}
	else
	{
		LOG("Attempt to create synapses when no layers exist - ignored");
		return std::shared_ptr<SynapseMatrix>();
	}
}

void Automaton::removeSynapse(std::shared_ptr<SynapseMatrix> synapses)
{
	mSynapses.erase(std::remove(mSynapses.begin(), mSynapses.end(), synapses), mSynapses.end());
	Lock lock;
	for (auto listener : mListeners)
	{
		listener->automatonSynapsesRemoved(synapses);
	}
}

void Automaton::setSize(int width, int height)
{
	if (width != mWidth || height != mHeight)
	{
		LOG("Resizing automaton to [" << width << " x " << height << "]");
		mWidth = width;
		mHeight = height;
		for (auto layer : mLayers)
		{
			layer->resize(mWidth, mHeight);
		}
		mSpikeProcessor->clear();

		Lock lock;
		for (auto listener : mListeners)
		{
			listener->automatonSizechanged(width, height);
		}
	}
}

vector<string> Automaton::typeNames()
{
	return mLayerFactory->getNames();
}

void Automaton::setSpike(const SpikeProcessor::Spike & spike)
{
	mSpikeProcessor->setSpike(spike);
}

// Returns the layer with the given name, or an empty pointer if it can't find it.
shared_ptr<Layer> Automaton::findLayer(const std::string & name)
{
	auto layer = find_if(mLayers.begin(), mLayers.end(), [name](auto lay) { return lay->name() == name; });
	if (layer != mLayers.end())
	{
		return *layer;
	}
	LOG("Request for non existent layer [" << name << "]");
	return shared_ptr<Layer>();
}

inline Automaton::Lock::Lock()
{
	assert(!mLocked);
	if (mLocked)
		NEURONTHROW("Invalid use of automaton listeners");
	mLocked = true;
}

inline Automaton::Lock::~Lock()
{
	assert(mLocked);
	mLocked = false;
}
