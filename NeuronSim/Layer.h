#ifndef LAYER_H
#define LAYER_H

#include <algorithm>
#include <stdint.h>
#include <string>

#include "ConfigSet.h"
#include "SynapseMatrix.h"
#include "Spike.h"
#include "Spiker.h"

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
	virtual void receiveSpikes(float * spikes) = 0;
	virtual void receiveShunts(float * shunts) = 0;
	virtual void preTick() {}
	virtual void tick(SynapseMatrix * synapses, Spiker * spiker) = 0;
	virtual void postTick() {}
	virtual std::string typeName() = 0;
	virtual void paint(uint32_t * image) = 0;
	virtual void setConfig(const ConfigSet & config) = 0;
	virtual ConfigSet getConfig() = 0;
	virtual const ConfigPresets & getPresets() = 0;
	virtual void clear() = 0;
	virtual void fire(int col, int row, float weight) = 0;

	const std::string & name() const { return mName; }
	void setName(const std::string & name) { mName = name; }
	virtual void resize(int width, int height);
	int width() const { return mWidth; }
	int height() const { return mHeight; }
	void setSpike(Spike::Shape shape, int duration) { mSpike.setSpike(shape, duration); }
	Spike::Shape spikeShape() { return mSpike.shape();}
	int spikeDuration() { return mSpike.duration(); }
	void selectPreset(const std::string & name);
	void regenerateName();

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
	Spike mSpike;
};

#endif
