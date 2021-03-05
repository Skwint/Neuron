#include "Layer.h"

#include <sstream>

#include "ConfigPresets.h"

static uint32_t nextName = 1;

using namespace std;

Layer::Layer(int width, int height) :
	mWidth(width),
	mHeight(height)
{
	regenerateName();
}

Layer::Layer(const Layer & other) :
	mName(other.mName),
	mWidth(other.mWidth),
	mHeight(other.mHeight),
	mSpike(other.mSpike)
{
}

Layer::~Layer()
{

}

void Layer::regenerateName()
{
	stringstream str;
	str << "Layer " << nextName;
	mName = str.str();
	// Let's just assume we never make 2^32 layers in one run.
	++nextName;
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
