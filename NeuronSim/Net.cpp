#include "Net.h"

#include <random>

static std::mt19937 rnd;
static const uint32_t outsideColour(0xFF202020);

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

void Net::paint(uint32_t * image)
{
	return paint(image, mWidth, 0, 0, mWidth, mHeight);
}

void Net::paint(uint32_t * image, int rowStep, int left, int top, int width, int height)
{
	for (int rr = top; rr < top + height; ++rr)
	{
		uint32_t * pixel = image + left + rr * rowStep;
		Cell * cell = row(rr) + left;
		for (int cc = left; cc < left + width; ++cc)
		{
			*pixel = cell->colour();
			++pixel;
			++cell;
		}
	}
	/*
	if (texWidth != mImageWidth || texHeight != mImageHeight)
	{
		// TODO - I need to repaint the outside if left or top change too
		mImage.resize(texWidth * texHeight);
		uint32_t * pixel = &mImage[0];
		for (int rr = 0; rr < texHeight; ++rr)
		{
			for (int cc = 0; cc < texWidth; ++cc)
			{
				*pixel = outsideColour;
				++pixel;
			}
		}
	}

	int bottom = std::min(texHeight, mHeight);
	int right = std::min(texWidth, mWidth);
	int texOffsetX = std::max(-left, 0);
	int texOffsetY = std::max(-top, 0);
	int cellOffsetX = std::max(left, 0);
	int cellOffsetY = std::max(top, 0);

	for (int rr = cellOffsetY; rr < bottom; ++rr)
	{
		uint32_t * pixel = &mImage[0] + texOffsetX + (texOffsetY + rr) * texWidth;
		Cell * cell = row(rr) + cellOffsetX;
		for (int cc = cellOffsetX; cc < right; ++cc)
		{
			*pixel = cell->colour();
			++pixel;
			++cell;
		}
	}
	*/
}
