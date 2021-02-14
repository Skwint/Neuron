#include "Automaton.h"

#include <cassert>
#include <filesystem>
#include <fstream>

#include "Constants.h"
#include "Exception.h"
#include "Layer.h"
#include "Log.h"
#include "LayerFactory.h"
#include "SpikeProcessor.h"
#include "StreamHelpers.h"
#include "SynapseMatrix.h"

using namespace std;

bool Automaton::Lock::mLocked(false);

const uint8_t TAG_TYPE('t');
const uint8_t TAG_WIDTH('w');
const uint8_t TAG_HEIGHT('h');
const uint8_t TAG_SPIKE('s');
const uint8_t TAG_END('E');

Automaton::Automaton() :
	mType("Life"),
	mWidth(DEFAULT_NET_SIZE),
	mHeight(DEFAULT_NET_SIZE)
{
	LOG("Creating automaton");
	mLayerFactory = std::make_unique<LayerFactory>();
}

Automaton::~Automaton()
{
	LOG("Destroying automaton");
}

void Automaton::tick()
{
	for (auto layer : mLayers)
	{
		layer->spikeTick();
	}
	for (auto layer : mLayers)
	{
		layer->shuntTick();
	}
	for (auto layer : mLayers)
	{
		layer->preTick();
	}
	for (auto synapses : mSynapses)
	{
		auto source = synapses->source();
		if (source)
			source->tick(synapses.get());
	}
	for (auto layer : mLayers)
	{
		layer->postTick();
	}
}

void Automaton::reset()
{
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
}

void Automaton::save(const std::filesystem::path & path)
{
	LOG("Saving automaton as [" << path << "]");

	if (filesystem::exists(path) && path.extension() == ".neuron")
	{
		filesystem::remove(path);
	}

	auto folder = path;
	folder.replace_extension("");
	if (filesystem::exists(folder))
	{
		if (filesystem::is_directory(folder))
		{
			filesystem::remove_all(folder);
		}
		else
		{
			NEURONTHROW("Unable to save to [" << folder << "] because it already exists in an unexpected form");
		}
	}
	filesystem::create_directory(folder);

	ofstream ofs(path, ios::out | ios::binary);
	if (ofs)
	{
		writePod(TAG_TYPE, ofs);
		writeString(mType, ofs);
		writePod(TAG_WIDTH, ofs);
		writePod(mWidth, ofs);
		writePod(TAG_HEIGHT, ofs);
		writePod(mHeight, ofs);
		writePod(TAG_END, ofs);
	}

	if (!ofs || !ofs.good())
	{
		NEURONTHROW("Failed to save automaton");
	}

	for (auto layer : mLayers)
	{
		layer->save(folder);
	}

	int counter = 0;
	std::stringstream str;
	for (auto synapse : mSynapses)
	{
		str << "synapse_" << counter << SYNAPSE_EXTENSION;
		synapse->save(folder / str.str());
		str.str("");
		++counter;
	}

	// Write the spikes. We don't really care where they originate from, we just
	// need to write where they are going to, but that isn't stored information
	// and we have to work it out as we go.
	for (auto target : mLayers)
	{
		auto filename = folder / target->name();
		filename.replace_extension(SPIKE_EXTENSION);
		std::ofstream ofs(filename, std::ios::out | std::ios::binary);

		for (auto source : mLayers)
		{
			source->writeSpikes(target, ofs);
		}

		if (!ofs || !ofs.good())
		{
			NEURONTHROW("Failed to save automaton");
		}
	}
}

void Automaton::load(const std::filesystem::path & path)
{
	LOG("Loading automaton from [" << path << "]");
	while (!mLayers.empty())
	{
		removeLayer(mLayers.back());
	}

	ifstream ifs(path, ios::in | ios::binary);
	std::string type;
	int width = 0;
	int height = 0;
	bool end = false;
	while (!end && ifs && ifs.good())
	{
		uint8_t tag = 1;
		readPod(tag, ifs);
		switch (tag)
		{
		case TAG_TYPE:
			readString(type, ifs);
			break;
		case TAG_WIDTH:
			readPod(width, ifs);
			break;
		case TAG_HEIGHT:
			readPod(height, ifs);
			break;
		case TAG_END:
			end = true;
			break;
		default:
			NEURONTHROW("Error while reading automaton [" << path << "] - unexpected tag [" << tag << "]");
			break;
		}
	}
	
	setNetworkType(type);
	setSize(width, height);

	auto folder = path;
	folder.replace_extension("");
	for (auto & layerfile : filesystem::directory_iterator(folder))
	{
		const filesystem::path file = layerfile;
		if (file.extension() == LAYER_EXTENSION)
		{
			auto layer = createDetachedLayer();
			layer->load(file);
			layer->setName(file.stem().string());
			attachLayer(layer);
		}
	}
	for (auto & synapsefile : filesystem::directory_iterator(folder))
	{
		const filesystem::path file = synapsefile;
		if (file.extension() == SYNAPSE_EXTENSION)
		{
			auto synapses = createDetachedSynapses();
			synapses->load(file);
			auto source = findLayer(synapses->sourceName());
			if (!source)
			{
				LOG("Synapse references source layer [" << synapses->sourceName() << "] which doesn't exist");
			}
			synapses->setSource(source);
			auto target = findLayer(synapses->targetName());
			if (!target)
			{
				LOG("Synapse references source layer [" << synapses->sourceName() << "] which doesn't exist");
			}
			synapses->setTarget(target);
			attachSynapses(synapses);
		}
	}

	// Read the spikes. We didn't save where they originate from, and don't care.
	// After loading them here, the processor of each layer will contain the spikes
	// targetting it.
	for (auto layer : mLayers)
	{
		auto filename = folder / layer->name();
		filename.replace_extension(SPIKE_EXTENSION);
		std::ifstream ifs(filename, std::ios::in | std::ios::binary);

		layer->readSpikes(ifs);

		if (!ifs || !ifs.good())
		{
			NEURONTHROW("Failed to load automaton");
		}
	}
}

void Automaton::clearLayers()
{
	for (auto layer : mLayers)
	{
		layer->clear();
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
		reset();
		mType = type;
		for (auto listener : mListeners)
		{
			listener->automatonTypeChanged();
		}
	}
}

std::shared_ptr<Layer> Automaton::createDetachedLayer()
{
	auto layer = mLayerFactory->create(mType, mWidth, mHeight);
	return layer;
}

void Automaton::attachLayer(std::shared_ptr<Layer> layer)
{
	mLayers.push_back(layer);

	Lock lock;
	for (auto listener : mListeners)
	{
		listener->automatonLayerCreated(layer);
	}
}

std::shared_ptr<Layer> Automaton::createLayer()
{
	auto layer = createDetachedLayer();
	attachLayer(layer);
	return layer;
}

void Automaton::removeLayer(const std::string & name)
{
	auto layer = findLayer(name);
	if (layer)
		removeLayer(layer);
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
			return (synapses->source() == layer || synapses->target() == layer);
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

std::shared_ptr<SynapseMatrix> Automaton::createDetachedSynapses()
{
	if (!mLayers.empty())
	{
		auto synapses = make_shared<SynapseMatrix>();
		return synapses;
	}
	else
	{
		LOG("Attempt to create synapses when no layers exist - ignored");
		return std::shared_ptr<SynapseMatrix>();
	}
}

void Automaton::attachSynapses(std::shared_ptr<SynapseMatrix> synapses)
{
	mSynapses.push_back(synapses);
	Lock lock;
	for (auto listener : mListeners)
	{
		listener->automatonSynapsesCreated(synapses);
	}
}

std::shared_ptr<SynapseMatrix> Automaton::createSynapse()
{
	auto synapses = createDetachedSynapses();
	if (synapses)
	{
		attachSynapses(synapses);
	}
	return synapses;
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
