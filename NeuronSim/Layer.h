#ifndef LAYER_H
#define LAYER_H

#include <algorithm>
#include <stdint.h>
#include <string>

#include "ConfigItem.h"
#include "SynapseMatrix.h"
#include "SpikeProcessor.h"

class SpikeProcessor;

class Layer
{
public:
	Layer(int width, int height);
	virtual ~Layer();

	virtual void tick(SynapseMatrix * synapses) = 0;
	virtual std::string typeName() = 0;
	virtual void paint(uint32_t * image, int rowStep, int left, int top, int width, int height) = 0;
	virtual void paint(uint32_t * image) = 0;
	virtual void setConfig(const ConfigSet & config) = 0;
	virtual ConfigSet getConfig() = 0;

	const std::string & name() { return mName; }
	virtual void resize(int width, int height);
	int width() { return mWidth; }
	int height() { return mHeight; }
	void setSpikeProcessor(std::shared_ptr<SpikeProcessor> spikeProcessor) { mSpikeProcessor = spikeProcessor; }

protected:
	std::string mName;
	int mWidth;
	int mHeight;
	std::shared_ptr<SpikeProcessor> mSpikeProcessor;
};

#endif
