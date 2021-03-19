#ifndef NEURON_TRUE_NORTH_H
#define NEURON_TRUE_NORTH_H

#include "Cell.h"

#include <algorithm>
#include <stdint.h>

// The per-neuron state of a TrueNorth network.
// This has only one variable in addition to the standard Cell,
// which is the membane potential.
struct NeuronTrueNorth : public Cell
{
	// Constructor
	inline NeuronTrueNorth() :
		Cell(),
		v(0)
	{
	}

	float v; //< membrane potential
};

#endif
