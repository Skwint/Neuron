#include "SpikeProcessor.h"

#include <algorithm>
#include <cassert>
#include <fstream>

#include "Constants.h"
#include "Log.h"

static const int MAX_FRAMES = MAX_SPIKE_LENGTH + MAX_SPIKE_DELAY;
static const int END_FRAME_MARKER = -1;

using namespace std;

SpikeProcessor::SpikeProcessor() :
	mCurrentFrame(0)
{
	// preallocate a circular buffer (indexed on time until fire)
	mFrames.resize(MAX_FRAMES);
	// Our default spike is just a single 1
	mSpike.push_back(1.0f);
}

SpikeProcessor::~SpikeProcessor()
{

}

void SpikeProcessor::setSpike(const std::vector<float> & spike)
{
	assert(spike.size() <= MAX_SPIKE_LENGTH);
	mSpike = spike;
}

void SpikeProcessor::saveSpike(ofstream & ofs)
{
	size_t size = mSpike.size();
	ofs.write(reinterpret_cast<char *>(&size), sizeof(size));
	ofs.write(reinterpret_cast<char *>(&mSpike[0]), size * sizeof(float));
}

void SpikeProcessor::loadSpike(ifstream & ifs)
{
	size_t size;
	ifs.read(reinterpret_cast<char *>(&size), sizeof(size));
	mSpike.resize(size);
	ifs.read(reinterpret_cast<char *>(&mSpike[0]), size * sizeof(float));
}

void SpikeProcessor::save(std::ofstream & ofs, float * first, float * last)
{
	for (int frameNum = 0; frameNum < mFrames.size(); ++frameNum)
	{
		auto & frame = mFrames[(mCurrentFrame + frameNum) % mFrames.size()];
		for (auto & target : frame)
		{
			if (target.weight != 0.0f && target.potential >= first && target.potential <= last)
			{
				int delta = static_cast<int>(target.potential - first);
				ofs.write(reinterpret_cast<char *>(&delta), sizeof(delta));
				ofs.write(reinterpret_cast<char *>(&target.weight), sizeof(target.weight));
			}
		}
		ofs.write(reinterpret_cast<const char *>(&END_FRAME_MARKER), sizeof(END_FRAME_MARKER));
		float zero = 0.0f;
		ofs.write(reinterpret_cast<char *>(&zero), sizeof(zero));
	}
}

void SpikeProcessor::load(std::ifstream & ifs, float * first)
{
	mCurrentFrame = 0;
	int delay = 0;
	while (!ifs.eof())
	{
		int index;
		float weight;
		ifs.read(reinterpret_cast<char *>(&index), sizeof(index));
		ifs.read(reinterpret_cast<char *>(&weight), sizeof(weight));
		if (index == END_FRAME_MARKER)
		{
			++delay;
		}
		else
		{
			fire(first + index, weight, delay);
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
