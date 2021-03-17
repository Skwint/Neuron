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

	float v; //< membrane potential
};

#endif
