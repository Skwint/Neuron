#ifndef NEURON_KUMAR_H
#define NEURON_KUMAR_H

#include "Cell.h"

#include <algorithm>
#include <stdint.h>

// The cells used for the Kumar networks.
// These have the generic data of the Cell structure and an
// internal state of u and v.
struct NeuronKumar : public Cell
{
	float u; //< membrane potential
	float v; //< membrane recovery variable
};

#endif
