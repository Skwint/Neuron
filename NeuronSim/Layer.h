#ifndef LAYER_H
#define LAYER_H

#include <algorithm>
#include <stdint.h>
#include <string>

#include "ConfigSet.h"
#include "SynapseMatrix.h"
#include "SpikeProcessor.h"

class SpikeProcessor;
class ConfigPresets;

// In each frame of animation the functions are called in the following order:
// - preTick()    - once per frame
// - tick()       - once per synapse sourced from this layer
// - postTick()   - once per frame
// - paint()      - zero or one times per frame
// Under most circumstaces tick() does not need to be overridden.
class Layer
{
public:
	Layer(int width, int height);
	virtual ~Layer();

	virtual void save(const std::filesystem::path & path) = 0;
	virtual void load(const std::filesystem::path & path) = 0;
	void writeSpikes(std::shared_ptr<Layer> target, std::ofstream & ofs);
	void readSpikes(std::ifstream & ifs);
	void spikeTick();
	virtual void preTick() {}
	virtual void tick(SynapseMatrix * synapses) = 0;
	virtual void postTick() {}
	virtual std::string typeName() = 0;
	virtual void paint(uint32_t * image) = 0;
	virtual void setConfig(const ConfigSet & config) = 0;
	virtual ConfigSet getConfig() = 0;
	virtual const ConfigPresets & getPresets() = 0;
	virtual void fire(int col, int row, float weight, int delay) = 0;
	virtual void clear();

	const std::string & name() const { return mName; }
	void setName(const std::string & name) { mName = name; }
	virtual void resize(int width, int height);
	int width() const { return mWidth; }
	int height() const { return mHeight; }
	void setSpike(const SpikeProcessor::Spike & spike) { mSpikeProcessor->setSpike(spike); }
	const SpikeProcessor::Spike & spike() { return mSpikeProcessor->spike(); }
	void selectPreset(const std::string & name);

protected:
	// Return pointer to the first Neuron
	// Used internally for backtracking synapses during save operation
	virtual void * begin() = 0;
	// Return pointer past the end of the last Neuron
	// Used internally for backtracking synapses during save operation
	virtual void * end() = 0;

protected:
	std::string mName;
	int mWidth;
	int mHeight;
	std::unique_ptr<SpikeProcessor> mSpikeProcessor;
};

#endif
