#ifndef SPIKE_PROCESSOR_H
#define SPIKE_PROCESSOR_H

#include <vector>
#include <iosfwd>

// SpikeProcessor
// The purpose of this class is to manage a set of synaptic spikes, each with a target
// neuron potential and, possibly, a delay before firing.
// All spikes are the same shape and length, defined with the setSpike function.
// Note that we are maintaining pointers to memory in this object and it must be
// cleared if the neurons that the spikes are destined for are destroyed.
// We are doing this because the alternative is to define a polymorphic interface
// and we cannot afford the processing cost per spike.
class SpikeProcessor
{
private:
	struct Target
	{
		Target(float * potential, float weight) :
			potential(potential),
			weight(weight)
		{ 
		}
		// The floating point value to which this spike will be added
		float * potential;
		// The weight of this spike
		float weight;
	};
	typedef std::vector<Target> Targets;
public:
	typedef std::vector<float> Spike;
public:
	SpikeProcessor();
	virtual ~SpikeProcessor();

	void setSpike(const Spike & spike);
	const Spike & spike() const { return mSpike; }
	void saveSpike(std::ofstream & ofs);
	void loadSpike(std::ifstream & ifs);
	void save(std::ofstream & ofs, float * first, float * last);
	void load(std::ifstream & ifs, float * first);
	void fire(float * target, float weight, int delay);
	void tick();
	void clear();

private:
	Spike mSpike;
	std::vector<Targets> mFrames;
	int mCurrentFrame;
};

#endif
