#include "Spike.h"

#include <cassert>
#include <fstream>

using namespace std;

Spike::Spike()
{
	setSpike(SHAPE_SQUARE, 1);
}

void Spike::setSpike(Shape shape, int duration)
{
	mShape = shape;
	mDuration = duration;

	mPotentials.resize(duration);
	switch (shape)
	{
	case SHAPE_SQUARE:
		for (auto & potential : mPotentials)
		{
			potential = 1.0f;
		}
		break;
	case SHAPE_TRIANGLE:
	{
		float halfDuration = 0.5f * duration;
		float pos = 0.5f;
		for (int x = 0; x < duration; ++x)
		{
			float xnorm = (pos - halfDuration) / halfDuration;
			mPotentials[x] = 1.0f - fabs(xnorm);
			pos += 1.0f;
		}
		break;
	}
	case SHAPE_GAUSS:
	{
		float halfDuration = 0.5f * duration;
		float pos = 0.5f;
		for (int x = 0; x < duration; ++x)
		{
			float xnorm = float(pos - halfDuration) / halfDuration;
			mPotentials[x] = exp(-4.0f * xnorm * xnorm);
			pos += 1.0f;
		}
		break;
	}
	case SHAPE_EXPONENTIAL:
	{
		int flat = duration / 5;
		for (int x = 0; x < flat; ++x)
		{
			mPotentials[x] = 1.0f;
		}
		if (duration > flat)
		{
			float val = 1.0f;
			float decay = pow(0.1f, 1.0f / (duration - flat));
			for (int x = flat; x < duration; ++x)
			{
				mPotentials[x] = val;
				val *= decay;
			}
		}
		break;
	}
	}
}

void Spike::saveSpike(ofstream & ofs)
{
	uint32_t size = uint32_t(mPotentials.size());
	ofs.write(reinterpret_cast<char *>(&size), sizeof(size));
	ofs.write(reinterpret_cast<char *>(&mPotentials[0]), size * sizeof(float));
}

void Spike::loadSpike(ifstream & ifs)
{
	uint32_t size;
	ifs.read(reinterpret_cast<char *>(&size), sizeof(size));
	mPotentials.resize(size);
	ifs.read(reinterpret_cast<char *>(&mPotentials[0]), size * sizeof(float));
}

