#include "Layer.h"

#include "SpikeProcessor.h"

Layer::Layer(int width, int height) :
	mWidth(width),
	mHeight(height)
{
	mSpikeProcessor = std::make_unique<SpikeProcessor>();
}

Layer::~Layer()
{

}

void Layer::resize(int width, int height)
{
	mWidth = width;
	mHeight = height;
}

void Layer::setSpike(const SpikeProcessor::Spike & spike)
{
	mSpikeProcessor->setSpike(spike);
}
