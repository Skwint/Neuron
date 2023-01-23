#ifndef SPIKE_H
#define SPIKE_H

#include <vector>
#include <ostream>

// A spike object represents a type of spike that can be fired, rather than
// an actual voltage spike that is in the process of being transmitted.
// For the latter see the SpikeTrain class.
class Spike
{
public:
	// The shape of the spike
	enum Shape
	{
		SHAPE_SQUARE,     //< Square wave of constant voltage
		SHAPE_TRIANGLE,   //< Symettrical triangular wave
		SHAPE_GAUSS,      //< Symettrical bell-curve wave
		SHAPE_EXPONENTIAL //< Instant spike with exponential decay
	};

	// Constructor
	Spike();

	// Set the shape and duration of this spike
	void setSpike(Shape shape, int duration);
	// Return the shape of this spike
	Shape shape() const { return mShape; }
	// Return the duraction of this spike
	int duration() const { return mDuration; }
	// Return the precalculated spike potential at a give time offset
	float potential(int index) const { return mPotentials[index]; }
	// Save this spike to an output stream
	void saveSpike(std::ofstream & ofs);
	// Load a spike from an input stream
	void loadSpike(std::ifstream & ifs);

private:
	// Spike shape
	Shape mShape;
	// Spike duration
	int mDuration;
	// Precalculated potentials over the duration
	std::vector<float> mPotentials;
};

#endif
