#ifndef NEURON_LIFE_H
#define NEURON_LIFE_H

#include <cstdint>
#include <random>

class NeuronLife
{
public:
	NeuronLife() : input(0.0f)
	{
		static std::mt19937 rnd;
		output = rnd() > 0x80000000 ? 1.0f : 0.0f;
	}
	virtual ~NeuronLife() {}

	uint32_t colour() { return 0xFF000000 + uint64_t(output * 0x00FFFFFF); }
public:
	float input;
	float output;
};

#endif
