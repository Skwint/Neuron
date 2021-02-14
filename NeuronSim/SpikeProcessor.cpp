#include "SpikeProcessor.h"

#include <algorithm>
#include <cassert>
#include <fstream>

#include "Constants.h"
#include "Log.h"
#include "StreamHelpers.h"

static const int MAX_FRAMES = MAX_SPIKE_LENGTH + MAX_SPIKE_DELAY;
static const int END_FRAME_MARKER = -1;

using namespace std;

SpikeProcessor::SpikeProcessor() :
	mCurrentFrame(0)
{
	LOG("Creating spike processor");
	// preallocate a circular buffer (indexed on time until fire)
	mFrames.resize(MAX_FRAMES);
	// Our default spike is just a single 1
	setSpike(SHAPE_SQUARE, 1);
}

SpikeProcessor::~SpikeProcessor()
{
	LOG("Destroying spike processor");
}

void SpikeProcessor::setSpike(SpikeShape shape, int duration)
{
	assert(duration <= MAX_SPIKE_LENGTH);
	mSpikeShape = shape;
	mSpikeDuration = duration;

	mSpike.resize(duration);
	switch (shape)
	{
	case SHAPE_SQUARE:
		for (auto & potential : mSpike)
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
			mSpike[x] = 1.0f - fabs(xnorm);
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
			mSpike[x] = exp(-4.0f * xnorm * xnorm);
			pos += 1.0f;
		}
		break;
	}
	}
}

void SpikeProcessor::saveSpike(ofstream & ofs)
{
	uint32_t size = uint32_t(mSpike.size());
	ofs.write(reinterpret_cast<char *>(&size), sizeof(size));
	ofs.write(reinterpret_cast<char *>(&mSpike[0]), size * sizeof(float));
}

void SpikeProcessor::loadSpike(ifstream & ifs)
{
	uint32_t size;
	ifs.read(reinterpret_cast<char *>(&size), sizeof(size));
	mSpike.resize(size);
	ifs.read(reinterpret_cast<char *>(&mSpike[0]), size * sizeof(float));
}

void SpikeProcessor::save(std::ofstream & ofs, void * begin, void * end)
{
	for (int frameNum = 0; frameNum < mFrames.size(); ++frameNum)
	{
		auto & frame = mFrames[(mCurrentFrame + frameNum) % mFrames.size()];
		for (auto & target : frame)
		{
			if (target.weight != 0.0f && target.potential >= begin && target.potential < end)
			{
				int delta = static_cast<int>(target.potential - (float *)begin);
				writePod(delta, ofs);
				writePod(target.weight, ofs);
			}
		}
		writePod(END_FRAME_MARKER, ofs);
	}
}

void SpikeProcessor::load(std::ifstream & ifs, void * begin)
{
	mCurrentFrame = 0;
	for (int frameNum = 0; frameNum < mFrames.size() && !ifs.eof(); ++frameNum)
	{
		auto & frame = mFrames[frameNum];
		int index;
		float weight;
		while (!ifs.eof())
		{
			ifs.read(reinterpret_cast<char *>(&index), sizeof(index));
			if (index == END_FRAME_MARKER)
			{
				break;
			}
			ifs.read(reinterpret_cast<char *>(&weight), sizeof(weight));
			frame.push_back(Target((float *)begin + index, weight));
		}
	}
}

void SpikeProcessor::fire(float * target, float weight, int delay)
{
	delay = min(delay, MAX_SPIKE_DELAY);
	int begin0 = mCurrentFrame + delay;
	int end0 = begin0 + int(mSpike.size());
	int begin1 = begin0 - int(mFrames.size());
	int end1 = begin1 + int(mSpike.size());
	end0 = min(MAX_FRAMES, end0);
	begin1 = max(0, begin1);
	int offset = 0;
	for (int frame = begin0; frame < end0; ++frame)
	{
		mFrames[frame].push_back(Target(target, weight * mSpike[offset]));
		++offset;
	}
	for (int frame = begin1; frame < end1; ++frame)
	{
		mFrames[frame].push_back(Target(target, weight * mSpike[offset]));
		++offset;
	}
}

void SpikeProcessor::tick()
{
	auto * frame = &mFrames[mCurrentFrame];
	for (auto target = frame->begin(); target != frame->end(); ++target)
	{
		*(target->potential) += target->weight;
	}
	frame->clear();
	mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES;
}

void SpikeProcessor::clear()
{
	for (auto & frame : mFrames)
	{
		frame.clear();
	}
}
