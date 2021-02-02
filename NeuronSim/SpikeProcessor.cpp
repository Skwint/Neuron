#include "SpikeProcessor.h"

#include <algorithm>
#include <cassert>

#include "Constants.h"

static const int MAX_FRAMES = MAX_SPIKE_LENGTH + MAX_SPIKE_DELAY;

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
		mFrames[frame].push_back(Target(target, weight, offset));
		++offset;
	}
	for (int frame = begin1; frame < end1; ++frame)
	{
		mFrames[frame].push_back(Target(target, weight, offset));
		++offset;
	}
}

void SpikeProcessor::tick()
{
	auto * frame = &mFrames[mCurrentFrame];
	for (auto target = frame->begin(); target != frame->end(); ++target)
	{
		*(target->potential) += target->weight * mSpike[target->offset];
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
