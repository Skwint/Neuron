#include "SpikeTrain.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <sstream>

#include "Constants.h"
#include "Exception.h"
#include "Layer.h"
#include "Log.h"
#include "Spike.h"
#include "StreamHelpers.h"

using namespace std;

static const uint8_t TAG_SIZE('s');
static const uint8_t TAG_DEPTH('d');
static const uint8_t TAG_FRAME('f');
static const uint8_t TAG_DATA('D');
static const uint8_t TAG_END('E');

SpikeTrain::SpikeTrain()
{

}

SpikeTrain::SpikeTrain(shared_ptr<Layer> source, shared_ptr<Layer> target, int delay, bool shunting) :
	mSource(source),
	mTarget(target),
	mShunting(shunting),
	mCurrentFrame(0)
{
	mFrames.resize(delay + 1);
	for (auto & frame : mFrames)
	{
		frame.resize(source->width() * source->height());
		std::fill(frame.begin(), frame.end(), 0.0f);
	}
}

SpikeTrain::~SpikeTrain()
{
}

void SpikeTrain::tick()
{
	auto & frame = mFrames[mCurrentFrame];
	if (mShunting)
	{
		mTarget->receiveShunts(&frame[0]);
	}
	else
	{
		mTarget->receiveSpikes(&frame[0]);
	}
	std::fill(frame.begin(), frame.end(), 0.0f);
	mCurrentFrame = (mCurrentFrame + 1) % mFrames.size();
}

void SpikeTrain::clear()
{
	for (auto & frame : mFrames)
	{
		std::fill(frame.begin(), frame.end(), 0.0f);
	}
}

float * SpikeTrain::begin(int delay)
{
	assert(delay > 0);
	assert(delay < mFrames.size());
	return &mFrames[delay][0];
}

void SpikeTrain::fire(const Spike & spike, int index, float weight, int delay)
{
	int begin[2];
	int end[2];
	begin[0] = mCurrentFrame + delay;
	end[0] = begin[0] + int(spike.duration());
	begin[1] = begin[0] - int(mFrames.size());
	end[1] = begin[1] + int(spike.duration());
	end[0] = min(int(mFrames.size()), end[0]);
	begin[1] = max(0, begin[1]);
	int offset = 0;
	for (int ii = 0; ii < 2; ++ii)
	{
		for (int frame = begin[ii]; frame < end[ii]; ++frame)
		{
			mFrames[frame][index] += weight * spike.potential(offset);
			++offset;
		}
	}
}

void SpikeTrain::save(const filesystem::path & path)
{
	stringstream name;
	name << mSource->name() << "_" << mTarget->name();
	auto filename = path / name.str();
	if (mShunting)
		filename.replace_extension(SHUNT_EXTENSION);
	else
		filename.replace_extension(SPIKE_EXTENSION);
	LOG("Saving spike train to [" << filename << "]");

	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
	if (ofs)
	{
		writePod(TAG_DEPTH, ofs);
		writePod(uint32_t(mFrames.size()), ofs);
		writePod(TAG_SIZE, ofs);
		writePod(uint32_t(mFrames[0].size()), ofs);
		writePod(TAG_FRAME, ofs);
		writePod(uint32_t(mCurrentFrame), ofs);
		writePod(TAG_DATA, ofs);
		for (auto & frame : mFrames)
		{
			ofs.write(reinterpret_cast<char *>(&frame[0]), frame.size() * sizeof(float));
		}
		writePod(TAG_END, ofs);
	}

	if (!ofs || !ofs.good())
	{
		NEURONTHROW("Failed to write [" << filename << "]");
	}
}

void SpikeTrain::load(const filesystem::path & path)
{
	LOG("Loading spike train from [" << path << "]");
	mFrames.clear();
	mShunting = (path.extension() == SHUNT_EXTENSION);
	stringstream str(path.stem().string());
	string source;
	getline(str, source, '_');
	string target;
	getline(str, target, '_');

	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	bool end = false;
	while (!end && ifs && ifs.good())
	{
		uint8_t tag = 0;
		readPod(tag, ifs);
		switch (tag)
		{
		case TAG_DEPTH:
		{
			uint32_t depth;
			readPod(depth, ifs);
			mFrames.resize(depth);
			break;
		}
		case TAG_SIZE:
		{
			uint32_t size;
			readPod(size, ifs);
			for (auto & frame : mFrames)
			{
				frame.resize(size);
			}
			break;
		}
		case TAG_FRAME:
		{
			uint32_t frame;
			readPod(frame, ifs);
			mCurrentFrame = frame;
			break;
		}
		case TAG_DATA:
		{
			for (auto & frame : mFrames)
			{
				ifs.read(reinterpret_cast<char *>(&frame[0]), frame.size() * sizeof(float));
			}
			break;
		}
		case TAG_END:
			end = true;
			break;
		default:
			NEURONTHROW("Corrupt spike file [" << path.string() << "] unknown tag [" << tag << "]");
			break;
		}
	}
}
