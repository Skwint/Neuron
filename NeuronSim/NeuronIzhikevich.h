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

	float u; //< membrane potential
	float v; //< membrane recovery variable
};

#endif
