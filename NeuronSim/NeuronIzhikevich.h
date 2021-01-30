#ifndef NEURON_IZHIKEVICH_H
#define NEURON_IZHIKEVICH_H

#include <stdint.h>

struct NeuronIzhikevich
{
	inline NeuronIzhikevich() :
		u(0.0f),
		v(0.0f)
	{
	}
	float u;
	float v;

	uint32_t colour() { return 0xFF | (u > 0.0 ? 0xFF00 : 0) | (v > 0.0f ? 0xFF0000 : 0); }
};

#endif
