#include "Layer.h"

#include <sstream>

#include "ConfigPresets.h"

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
}

Layer::~Layer()
{

}

void Layer::clear()
{
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
