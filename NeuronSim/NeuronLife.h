#ifndef NEURON_LIFE_H
#define NEURON_LIFE_H

#include <cstdint>
#include <random>

static const float neuronLifeCheck(-12345.0f);

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
	virtual ~NeuronLife()
	{
		// This was added because an off by one error during development managed to
		// evade detection by std::vector and caused a crash that took most of a day
		// to track down.
		// It works by setting the value of deleted neurons to something they can
		// never naturally attain, and then asserting if a neuron with those values
		// is ever accessed.
		potential = input = neuronLifeCheck;
	}

	// This is no good it has hardcoded potential thresholds instead of using the config! TODO
	uint32_t color() { return (potential > 2.25f && potential < 3.75f? 0xFFFFFFFF: 0xFF000000); }
public:
	float input;
	float potential;
};

#endif
