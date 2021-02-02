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

	virtual void tick() = 0;
	virtual std::string typeName() = 0;
	virtual void paint(uint32_t * image, int rowStep, int left, int top, int width, int height) = 0;
	virtual void paint(uint32_t * image) = 0;

	virtual void resize(int width, int height);
	int width() { return mWidth; }
	int height() { return mHeight; }
	const ConfigSet & config() { return mConfig; }
	virtual void setConfig(const ConfigSet & config) = 0;
	void setSynapses(SynapseMatrix synapses) { mSynapses = synapses; }
	void setSpike(const SpikeProcessor::Spike & spike);
	inline int synapseNormColBegin(int col) { return std::max(0, col - mSynapses.width() / 2) - col; }
	inline int synapseNormColEnd(int col) { return std::min(mWidth, col + mSynapses.width() / 2 + 1) - col; }
	inline int synapseNormRowBegin(int row) { return std::max(0, row - mSynapses.height() / 2) - row; }
	inline int synapseNormRowEnd(int row) { return std::min(mHeight, row + mSynapses.height() / 2 + 1) - row; }
	inline int synapseLowWrapColBegin(int col) { return std::max(0, col - mWidth - mSynapses.width() / 2) - col; }
	inline int synapseLowWrapColEnd(int col) { return std::min(mWidth, col - mWidth + mSynapses.width() / 2 + 1) - col; }
	inline int synapseLowWrapRowBegin(int row) { return std::max(0, row - mHeight - mSynapses.height() / 2) - row; }
	inline int synapseLowWrapRowEnd(int row) { return std::min(mHeight, row - mHeight + mSynapses.height() / 2 + 1) - row; }
	inline int synapseHighWrapColBegin(int col) { return std::max(0, col + mWidth - mSynapses.width() / 2) - col; }
	inline int synapseHighWrapColEnd(int col) { return std::min(mWidth, col + mWidth + mSynapses.width() / 2 + 1) - col; }
	inline int synapseHighWrapRowBegin(int row) { return std::max(0, row + mHeight - mSynapses.height() / 2) - row; }
	inline int synapseHighWrapRowEnd(int row) { return std::min(mHeight, row + mHeight + mSynapses.height() / 2 + 1) - row; }

protected:
	int mWidth;
	int mHeight;
	ConfigSet mConfig;
	SynapseMatrix mSynapses;
	std::unique_ptr<SpikeProcessor> mSpikeProcessor;
};

#endif
