#include "Automaton.h"

#include "Layer.h"
#include "Log.h"
#include "LayerFactory.h"
#include "SpikeProcessor.h"
#include "SynapseMatrix.h"

using namespace std;

Automaton::Automaton() :
	mType("Life"),
	mWidth(512),
	mHeight(512)
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
	mListeners.push_back(listener);
}

void Automaton::removeListener(Listener * listener)
{
	mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
}

void Automaton::setNetworkType(const std::string & type)
{
	if (type != mType)
	{
		mSpikeProcessor->clear();
		mSynapses.clear();
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

	for (auto listener : mListeners)
	{
		listener->automatonLayerCreated(layer);
	}
	return layer;
}

void Automaton::removeLayer(std::shared_ptr<Layer> layer)
{
	mLayers.erase(std::remove(mLayers.begin(), mLayers.end(), layer), mLayers.end());
	for (auto listener : mListeners)
	{
		listener->automatonLayerRemoved(layer);
	}
}

void Automaton::createSynapse()
{
	if (!mLayers.empty())
	{
		auto synapses = make_shared<SynapseMatrix>();
		synapses->setSource(mLayers[0]);
		synapses->setTarget(mLayers[0]);
		mSynapses.push_back(synapses);
		for (auto listener : mListeners)
		{
			listener->automatonSynapsesCreated(synapses);
		}
	}
	else
	{
		LOG("Attempt to create synapses when no layers exist - ignored");
	}
}

void Automaton::removeSynapse(std::shared_ptr<SynapseMatrix> synapses)
{
	mSynapses.erase(std::remove(mSynapses.begin(), mSynapses.end(), synapses), mSynapses.end());
	for (auto listener : mListeners)
	{
		listener->automatonSynapsesRemoved(synapses);
	}
}

void Automaton::setSize(int width, int height)
{
	if (width != mWidth || height != mHeight)
	{
		mWidth = width;
		mHeight = height;
		for (auto layer : mLayers)
		{
			layer->resize(mWidth, mHeight);
		}
	}
}

vector<string> Automaton::typeNames()
{
	return mLayerFactory->getNames();
}

const ConfigSet & Automaton::config() const
{
	return mLayerFactory->config(mType);
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

