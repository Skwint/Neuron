#include "Automaton.h"

#include "Layer.h"
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

	for (auto layer : mLayers)
	{
		layer->tick();
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
	// TODO - this is INVALID because mListeners might change while we are iterating through it
	// which is BAD (we create a listener when we receive a layer created message).
	// Probably best if LayerConfig in the UI stops being a listener and we just forward info
	// to it from the LayerDock.
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

std::shared_ptr<SynapseMatrix> Automaton::createSynapse()
{
	auto synapses = make_shared<SynapseMatrix>();
	mSynapses.push_back(synapses);
	for (auto listener : mListeners)
	{
		listener->automatonSynapsesCreated(synapses);
	}
	return synapses;
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

std::vector<std::string> Automaton::typeNames()
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
