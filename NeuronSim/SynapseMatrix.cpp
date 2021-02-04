#include "SynapseMatrix.h"

SynapseMatrix::SynapseMatrix() :
	mWidth(1),
	mHeight(1)
{
	mSynapses.resize(1);
}

SynapseMatrix::SynapseMatrix(int width, int height)
{
	setSize(width, height);
}

SynapseMatrix::~SynapseMatrix()
{

}

void SynapseMatrix::setSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	mSynapses.resize(mWidth * mHeight);
}

// loadImage takes an image of 32 bit pixels in RGBA format, with a size
// of width x height, and uses the red channel as the delay for signal
// propogation along the synapse, and the green and blue channels combined
// as the weight of the signal, normalized to the range [-1,1]
// The alpha channel is ignored.
void SynapseMatrix::loadImage(uint32_t * pixels, int width, int height)
{
	setSize(width, height);
	Synapse * synapse = &mSynapses[0];
	uint32_t * pixel = pixels;
	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			uint32_t gb = (*pixel & 0x0000FFFF);
			synapse->weight = float(gb) / 32767.0f - 1.0f;
			uint32_t r = (*pixel & 0x00FF0000) >> 16;
			synapse->delay = r;
			++pixel;
			++synapse;
		}
	}
}
