#include "Layer.h"

#include <sstream>

#include "ConfigPresets.h"
#include "SpikeProcessor.h"

static uint32_t nextName = 1;

using namespace std;

Layer::Layer(int width, int height) :
	mWidth(width),
	mHeight(height)
{
	stringstream str;
	str << "Layer " << nextName;
	mName = str.str();
	// Let's just assume we never make 2^32 layers in one run.
	++nextName;

	mSpikeProcessor = std::make_unique<SpikeProcessor>();
}

Layer::~Layer()
{

}

void Layer::clear()
{
	clearSpikes();
}

void Layer::clearSpikes()
{
	mSpikeProcessor->clear();
}

void Layer::resize(int width, int height)
{
	clear();
	mWidth = width;
	mHeight = height;
}

void Layer::selectPreset(const string & name)
{
	setConfig(getPresets()[name]);
}

void Layer::spikeTick()
{
	mSpikeProcessor->tick();
}

void Layer::writeSpikes(shared_ptr<Layer> target, ofstream & ofs)
{
	mSpikeProcessor->save(ofs, target->begin(), target->end());
}

void Layer::readSpikes(ifstream & ifs)
{
	mSpikeProcessor->load(ifs, begin());
}
