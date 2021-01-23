#include "Net.h"

#include <random>

static std::mt19937 rnd;

Net::Net(int width, int height)
{
	resize(width, height);
}

Net::~Net()
{

}

void Net::tick()
{

}

void Net::resize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	int size = (mWidth + 2) * (mHeight + 2);
	cells.resize(size);
	mImage.resize(mWidth * mHeight);
	for (int rr = 0; rr < mHeight; ++rr)
	{
		Cell * cell = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			cell->output = rnd() > 0x80000000 ? 1.0f : 0.0f;
			++cell;
		}
	}
}

uint32_t * Net::image()
{
	uint32_t * pixel = &mImage[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		Cell * cell = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			*pixel = cell->colour();
			++cell;
			++pixel;
		}
	}
	return &mImage[0];
}
