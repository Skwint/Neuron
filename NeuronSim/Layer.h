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

// All new neuron types are implemented by specialising this class. However,
// an intermediate specialisation (Net) exists and it is expected that all
// neuron types will be implemented as specialisations of that.
// In each frame of animation these functions are called in the following order:
// - tick()       - once per frame
// - fireSpikes() - once per synapse sourced from this layer
// - paint()      - zero or one times per frame
class Layer
{
public:
	Layer(int width, int height);
	Layer(const Layer & other);
	virtual ~Layer();

	virtual void save(const std::filesystem::path & path) = 0;
	virtual void load(const std::filesystem::path & path) = 0;
	virtual void receiveSpikes(float * spikes) = 0;
	virtual void receiveShunts(float * shunts) = 0;
	virtual void tick() = 0;
	virtual void fireSpikes(SynapseMatrix * synapses, Spiker * spiker) = 0;
	virtual std::string typeName() = 0;
	virtual void paint(uint32_t * image) = 0;
	virtual void setConfig(const ConfigSet & config) = 0;
	virtual ConfigSet getConfig() = 0;
	virtual const ConfigPresets & getPresets() = 0;
	virtual void clear() = 0;
	virtual void inject(int col, int row, float weight) = 0;

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
	std::string mName;
	int mWidth;
	int mHeight;
	Spike mSpike;
};

#endif
