#ifndef NEURON_IZHIKEVICH_H
#define NEURON_IZHIKEVICH_H

#include "Cell.h"

#include <algorithm>
#include <stdint.h>

struct NeuronIzhikevich : public Cell
{
	inline NeuronIzhikevich() :
		u(0.0f),
		v(0.0f)
	{
	}

	// This function uses some rough guesses at likely values of u and v
	// to try to do something useful with colouring.
	uint32_t color()
	{
		return 0xFF000000 |
			(firing?0xFF0000:0) |
			(uint32_t(std::clamp((128.0f + 4.0f * u), 0.0f, 255.0f)) << 8) |
			uint32_t(std::clamp((128.0f + 8.0f * v), 0.0f, 255.0f));
	}

	float u; //< membrane potential
	float v; //< membrane recovery variable
};

#endif
