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
	virtual void paint(uint32_t * image) = 0;
	virtual void setConfig(const ConfigSet & config) = 0;
	virtual ConfigSet getConfig() = 0;

	const std::string & name() { return mName; }
	virtual void resize(int width, int height);
	int width() { return mWidth; }
	int height() { return mHeight; }
	void setSpikeProcessor(std::shared_ptr<SpikeProcessor> spikeProcessor) { mSpikeProcessor = spikeProcessor; }
	void setUserData(void * data) { mUserData = data; }
	void * userData() { return mUserData; }

protected:
	std::string mName;
	int mWidth;
	int mHeight;
	std::shared_ptr<SpikeProcessor> mSpikeProcessor;

private:
	void * mUserData;
};

#endif
