#include "SynapseMatrix.h"

#include <algorithm>
#include <fstream>

#include "Constants.h"
#include "Exception.h"
#include "Layer.h"
#include "Log.h"
#include "StreamHelpers.h"

static const uint8_t TAG_WIDTH('w');
static const uint8_t TAG_HEIGHT('h');
static const uint8_t TAG_WEIGHT('W');
static const uint8_t TAG_DELAY('D');
static const uint8_t TAG_SOURCE('s');
static const uint8_t TAG_TARGET('t');
static const uint8_t TAG_SHUNT('S');
static const uint8_t TAG_DATA('d');
static const uint8_t TAG_NAME('n');
static const uint8_t TAG_END('E');

using namespace std;

SynapseMatrix::SynapseMatrix(Listener * listener) :
	mListener(listener),
	mWidth(1),
	mHeight(1),
	mWeight(1.0f),
	mDelay(DELAY_NONE),
	mShunt(false)
{
	mSynapses.resize(1);
}

SynapseMatrix::SynapseMatrix(Listener * listener, int width, int height) :
	mListener(listener)
{
	setSize(width, height);
}

SynapseMatrix::~SynapseMatrix()
{

}

// The delays of the synapses will be recalculated to accomodate size changes.
void SynapseMatrix::setSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	mSynapses.resize(mWidth * mHeight);
	setDelay(mDelay);
	mListener->synapseMatrixChanged(this);
}

void SynapseMatrix::setDelay(Delay delay)
{
	mDelay = delay;
	switch (delay)
	{
	case DELAY_LINEAR:
	{
		int hb = -mHeight / 2;
		int he = mHeight / 2 + (mHeight & 1);
		int wb = -mWidth / 2;
		int we = mWidth / 2 + (mWidth & 1);
		Synapse * synapse = &mSynapses[0];
		for (int h = hb; h < he; ++h)
		{
			for (int w = wb; w < we; ++w)
			{
				synapse->delay = max(0, int(sqrt(h * h + w * w)) - 1);
				++synapse;
			}
		}
		break;
	}
	case DELAY_GRID:
	{
		int hb = -mHeight / 2;
		int he = mHeight / 2 + (mHeight & 1);
		int wb = -mWidth / 2;
		int we = mWidth / 2 + (mWidth & 1);
		Synapse * synapse = &mSynapses[0];
		for (int h = hb; h < he; ++h)
		{
			for (int w = wb; w < we; ++w)
			{
				synapse->delay = max(0, h + w - 1);
				++synapse;
			}
		}
		break;
	}
	case DELAY_ONE:
	{
		for (auto & synapse : mSynapses)
		{
			synapse.delay = 1;
		}
		break;
	}
	case DELAY_NONE:
	default:
		for (auto & synapse : mSynapses)
		{
			synapse.delay = 0;
		}
		break;
	}
	mListener->synapseMatrixChanged(this);
}

// load map will treat the lowest 8 bits of the width x height array as
// the weight of a synapse connection at that location, scaled to [0,weight].
// The delays of the synapses will be recalculated to accomodate size changes.
void SynapseMatrix::loadImage(uint32_t * synapseMap, int width, int height, float weight, const string & name)
{
	mImageName = name;
	mWeight = weight;
	setSize(width, height);
	Synapse * synapse = &mSynapses[0];
	uint32_t * data = synapseMap;
	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			uint32_t b = (*data & 0x000000FF);
			synapse->weight = weight * (float(b) / 0xFF);
			++data;
			++synapse;
		}
	}
	mListener->synapseMatrixChanged(this);
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
			readPod(width, ifs);
			break;
		case TAG_HEIGHT:
			readPod(height, ifs);
			break;
		case TAG_WEIGHT:
			readPod(mWeight, ifs);
			break;
		case TAG_DELAY:
		{
			uint8_t delay;
			readPod(delay, ifs);
			if (delay >= DELAY_COUNT)
				delay = DELAY_NONE;
			setDelay(Delay(delay));
			break;
		}
		case TAG_SOURCE:
			readString(mSourceName, ifs);
			break;
		case TAG_TARGET:
			readString(mTargetName, ifs);
			break;
		case TAG_NAME:
			readString(mImageName, ifs);
			break;
		case TAG_SHUNT:
		{
			uint8_t shunt;
			readPod(shunt, ifs);
			mShunt = shunt ? true : false;
			break;
		}
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
		writePod(TAG_WEIGHT, ofs);
		writePod(mWeight, ofs);
		writePod(TAG_DELAY, ofs);
		writePod(uint8_t(mDelay), ofs);
		writePod(TAG_SHUNT, ofs);
		writePod(uint8_t(mShunt), ofs);
		writePod(TAG_SOURCE, ofs);
		writeString(source->name(), ofs);
		writePod(TAG_TARGET, ofs);
		writeString(target->name(), ofs);
		writePod(TAG_NAME, ofs);
		writeString(mImageName, ofs);
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

uint32_t SynapseMatrix::maximumDelay()
{
	uint32_t delay = 0;
	for (auto synapse : mSynapses)
	{
		delay = max(delay, synapse.delay);
	}
	return delay;
}

void SynapseMatrix::setShunt(bool shunt)
{
	if (mShunt != shunt)
	{
		mShunt = shunt;
		mListener->synapseMatrixChanged(this);
	}
}
