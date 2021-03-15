#include "Automaton.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <thread>

#include "Constants.h"
#include "Exception.h"
#include "Layer.h"
#include "Log.h"
#include "LayerFactory.h"
#include "SpikeTrain.h"
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
	mMode(MODE_NORMAL),
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
	vector<thread> threads;

	if (mSpikeTrains.empty())
	{
		recalculateSpikeTrains();
	}
	if (mMode != MODE_DEPRESSED)
	{
		for (auto layer : mLayers)
		{
			threads.push_back(thread(&Automaton::tickTargetLayer, this, layer.get()));
		}
		for (auto & tt : threads)
		{
			tt.join();
		}
		threads.clear();
	}
	else
	{
		for (auto spikeTrain : mSpikeTrains)
		{
			spikeTrain->clear();
		}
	}

	for (auto layer : mLayers)
	{
		threads.push_back(thread(&Automaton::tickSourceLayer, this, layer.get()));
	}
	for (auto & tt : threads)
	{
		tt.join();
	}
	threads.clear();
}

// This function executes within a thread and is responsible for writing data
// associated with one layer and one layer only. It must not read or write data
// associated with any other layer.
// During this function, the data in a spike train is considered to be belonging
// to the layer it is targetted to.
void Automaton::tickTargetLayer(Layer * target)
{
	for (auto & spikeTrain : mSpikeTrains)
	{
		if (spikeTrain->target().get() == target)
		{
			spikeTrain->tick();
		}
	}
}

// This function executes within a thread and is responsible for writing data
// associated with one layer and one layer only. It must not read or write data
// associated with any other layer.
// During this function, the data in a spike train is consider to be belonging
// to the layer it is sourced from.
void Automaton::tickSourceLayer(Layer * source)
{
	source->tick();

	for (auto synapses : mSynapses)
	{
		if (synapses->source().get() == source)
		{
			for (auto spikeTrain : mSpikeTrains)
			{
				if (spikeTrain->source() == synapses->source() &&
					spikeTrain->target() == synapses->target())
				{
					spikeTrain->source()->fireSpikes(synapses.get(), spikeTrain.get());
				}
			}
		}
	}
}

void Automaton::reset()
{
	mSynapses.clear();
	mSpikeTrains.clear();
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

	for (auto spikeTrain : mSpikeTrains)
	{
		spikeTrain->save(folder);
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
}

void Automaton::load(const std::filesystem::path & path)
{
	LOG("Loading automaton from [" << path << "]");
	mSpikeTrains.clear();
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
	for (auto & spikefile : filesystem::directory_iterator(folder))
	{
		const filesystem::path file = spikefile;
		if (file.extension() == SPIKE_EXTENSION || file.extension() == SHUNT_EXTENSION)
		{
			stringstream str(file.stem().string());
			string source;
			getline(str, source, '_');
			auto sourceLayer = findLayer(source);
			string target;
			getline(str, target, '_');
			auto targetLayer = findLayer(target);
			if (sourceLayer && targetLayer)
			{
				auto spikeTrain = make_shared<SpikeTrain>(sourceLayer, targetLayer, 0, false);
				spikeTrain->load(file);
				mSpikeTrains.push_back(spikeTrain);
			}
			else
			{
				NEURONTHROW("Error loading [" << file << "] - name doesn't match known layers");
			}
		}
	}
}

// If a synapse matrix changes we need to make sure our spike trains are able
// to accomodate the delays it now uses
void Automaton::synapseMatrixChanged(SynapseMatrix * matrix)
{
	mSpikeTrains.clear();
}

void Automaton::recalculateSpikeTrains()
{
	LOG("Regenerating spike trains");
	mSpikeTrains.clear();
	for (auto source : mLayers)
	{
		for (auto target : mLayers)
		{
			int maxDelaySpike = -1;
			int maxDelayShunt = -1;
			for (auto synapse : mSynapses)
			{
				if (synapse->source() == source && synapse->target() == target)
				{
					if (synapse->isShunt())
					{
						maxDelayShunt = max(maxDelayShunt, int(synapse->maximumDelay()));
					}
					else
					{
						maxDelaySpike = max(maxDelaySpike, int(synapse->maximumDelay()));
					}
				}
			}
			if (maxDelayShunt != -1)
			{
				maxDelayShunt += source->spikeDuration() - 1;
				mSpikeTrains.push_back(make_shared<SpikeTrain>(source, target, maxDelayShunt, true));
			}
			if (maxDelaySpike != -1)
			{
				maxDelaySpike += source->spikeDuration() - 1;
				mSpikeTrains.push_back(make_shared<SpikeTrain>(source, target, maxDelaySpike, false));
			}
		}
	}
}

void Automaton::clearLayers()
{
	for (auto layer : mLayers)
	{
		layer->clear();
	}
	for (auto spikes : mSpikeTrains)
	{
		spikes->clear();
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
	while (findLayer(layer->name()))
	{
		layer->regenerateName();
	}
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
	auto synapses = make_shared<SynapseMatrix>(this);
	return synapses;
}

void Automaton::attachSynapses(std::shared_ptr<SynapseMatrix> synapses)
{
	mSynapses.push_back(synapses);
	synapseMatrixChanged(synapses.get());
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
		mSpikeTrains.clear();

		Lock lock;
		for (auto listener : mListeners)
		{
			listener->automatonSizeChanged(width, height);
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

float Automaton::currentSpikeDensity()
{
	float density = 0.0f;
	for (auto train : mSpikeTrains)
	{
		density += train->currentSpikeDensity();
	}
	return density / float(mSpikeTrains.size());
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
