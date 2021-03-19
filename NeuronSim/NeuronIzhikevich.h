#ifndef NEURON_IZHIKEVICH_H
#define NEURON_IZHIKEVICH_H

#include "Cell.h"

#include <algorithm>
#include <stdint.h>

// The cells used for the Izhikevich networks.
// These have the generic data of the Cell structure and an
// internal state of u and v.
struct NeuronIzhikevich : public Cell
{
	float u; //< membrane potential
	float v; //< membrane recovery variable
};

#endif
