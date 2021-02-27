#ifndef NEURON_TRUE_NORTH_H
#define NEURON_TRUE_NORTH_H

#include "Cell.h"

#include <algorithm>
#include <stdint.h>

// The per-neuron state of a TrueNorth network.
struct NeuronTrueNorth : public Cell
{
	inline NeuronTrueNorth() :
		Cell(),
		v(0)
	{
	}

	// This function uses some rough guesses at likely values of v
	// to try to do something useful with colouring.
	uint32_t color()
	{
		float intensity = 8.0f * v;
		int g = std::clamp(int(intensity), 0, 0xFF);
		int r = std::clamp(int(-intensity), 0, 0xFF);
		return 0xFF000000 | (firing ? 0xFF0000 : 0) | r | (g << 8);
	}

	float v; //< membrane potential
};

#endif
