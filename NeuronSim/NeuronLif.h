#ifndef NEURON_LIF_H
#define NEURON_LIF_H

#include "Cell.h"

#include <algorithm>
#include <cstdint>

// The neuron type used by the LIF (Leaky Integrate and Fire) automaton types.
// Synapses feed into the input, are integrated into the potential, and cause
// a spike if the refractory period is not blocking.
// @see LinearLif
struct NeuronLif : public Cell
{
	NeuronLif() : potential(0.0f), refractory(0) {	}

	float potential;
	int refractory;
};

#endif
