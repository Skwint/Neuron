#ifndef NEURON_LIFE_H
#define NEURON_LIFE_H

#include <cstdint>
#include <random>

class NeuronLife
{
public:
	NeuronLife() : potential(0.0f)
	{
		// This is very arbitrary - we randomly set the inputs of a little less
		// than half the cells to be enough to cause a spike in the default
		// rule set. This is a stop gap measure until we can do initial
		// conditions properly.
		static std::mt19937 rnd;
		input = rnd() > 0xA0000000 ? 3.0f : 0.0f;
	}
	virtual ~NeuronLife() {}

	uint32_t colour() { return 0xFF000000 | (potential > 2.25f && potential < 3.75f? 0x00FFFFFF: 0x0); }
public:
	float input;
	float potential;
};

#endif
