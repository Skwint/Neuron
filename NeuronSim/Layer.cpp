#include "Layer.h"

Layer::Layer(int width, int height) :
	mWidth(width),
	mHeight(height)
{
	resize(width, height);
}

Layer::~Layer()
{

}

void Layer::resize(int width, int height)
{
	mWidth = width;
	mHeight = height;
}
