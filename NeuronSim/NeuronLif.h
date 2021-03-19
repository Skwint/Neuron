#ifndef NEURON_LIF_H
#define NEURON_LIF_H

#include "Cell.h"

#include <algorithm>
#include <cstdint>

// The neuron type used by the LIF (Leaky Integrate and Fire) automaton types.
// Synapses feed into the input, are integrated into the potential.
// @see LinearLif
struct NeuronLif : public Cell
{
	// Constructor.
	NeuronLif() : potential(0.0f) {	}

	// Membrane potential.
	float potential;
};

#endif
