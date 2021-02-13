#include "SynapseMatrix.h"

#include <fstream>

#include "Constants.h"
#include "Exception.h"
#include "Layer.h"
#include "Log.h"
#include "StreamHelpers.h"

static const uint8_t TAG_WIDTH('w');
static const uint8_t TAG_HEIGHT('h');
static const uint8_t TAG_SOURCE('s');
static const uint8_t TAG_TARGET('t');
static const uint8_t TAG_DATA('d');
static const uint8_t TAG_END('E');

using namespace std;

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

// loadImage takes an image of 32 bit pixels in ARGB format, with a size
// of width x height, and uses the red channel as the delay for signal
// propogation along the synapse, and the green and blue channels combined
// as the weight of the signal, normalized to the range [-1,1] and then
// multiplied by the weight
// The alpha channel is ignored.
void SynapseMatrix::loadImage(uint32_t * pixels, int width, int height, float weight)
{
	setSize(width, height);
	Synapse * synapse = &mSynapses[0];
	uint32_t * pixel = pixels;
	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			uint32_t gb = (*pixel & 0x0000FFFF);
			synapse->weight = weight * (float(gb) / 0x8000 - 1.0f);
			uint32_t r = (*pixel & 0x00FF0000) >> 16;
			synapse->delay = r;
			++pixel;
			++synapse;
		}
	}
}

void SynapseMatrix::load(const std::filesystem::path & path)
{
	LOGDEBUG("Loading synapses from [" << path << "]");
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	int width = 1;
	int height = 1;
	bool end = false;
	while (!end && ifs && ifs.good())
	{
		uint8_t tag;
		ifs.read(reinterpret_cast<char *>(&tag), sizeof(tag));
		switch (tag)
		{
		case TAG_WIDTH:
			ifs.read(reinterpret_cast<char *>(&width), sizeof(width));
			break;
		case TAG_HEIGHT:
			ifs.read(reinterpret_cast<char *>(&height), sizeof(height));
			break;
		case TAG_SOURCE:
			readString(mSourceName, ifs);
			break;
		case TAG_TARGET:
			readString(mTargetName, ifs);
			break;
		case TAG_DATA:
			setSize(width, height);
			ifs.read(reinterpret_cast<char *>(&mSynapses[0]), mWidth * mHeight * sizeof(Synapse));
			break;
		case TAG_END:
			end = true;
			break;
		default:
			NEURONTHROW("Failed to read [" << path << "]");
			break;
		}
	}
}

void SynapseMatrix::save(const std::filesystem::path & path)
{
	LOGDEBUG("Saving synapses to [" << path << "]");
	auto filename = path;
	filename.replace_extension(SYNAPSE_EXTENSION);
	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
	if (ofs)
	{
		auto source = mSource.lock();
		auto target = mTarget.lock();
		if (!source || !target)
		{
			NEURONTHROW("Invalid state - synapse source or target nolonger exists")
		}
		uint32_t sourceSize = uint32_t(source->name().size());
		uint32_t targetSize = uint32_t(target->name().size());
		writePod(TAG_WIDTH, ofs);
		writePod(mWidth, ofs);
		writePod(TAG_HEIGHT, ofs);
		writePod(mHeight, ofs);
		writePod(TAG_SOURCE, ofs);
		writeString(source->name(), ofs);
		writePod(TAG_TARGET, ofs);
		writeString(target->name(), ofs);
		writePod(TAG_DATA, ofs);
		ofs.write(reinterpret_cast<char *>(&mSynapses[0]), mWidth * mHeight * sizeof(Synapse));
		writePod(TAG_END, ofs);
	}
	if (!ofs || !ofs.good())
	{
		NEURONTHROW("Failed to write [" << filename << "]");
	}
}

const std::string & SynapseMatrix::sourceName()
{
	auto sourceLayer = mSource.lock();
	if (sourceLayer)
		return sourceLayer->name();
	return mSourceName;
}

const std::string & SynapseMatrix::targetName()
{
	auto targetLayer = mTarget.lock();
	if (targetLayer)
		return targetLayer->name();
	return mTargetName;
}
