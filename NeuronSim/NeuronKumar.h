#ifndef NEURON_KUMAR_H
#define NEURON_KUMAR_H

#include "Cell.h"

#include <algorithm>
#include <stdint.h>

struct NeuronKumar : public Cell
{
	inline NeuronKumar() :
		u(0.0f),
		v(0.0f)
	{
	}

	float u; //< membrane potential
	float v; //< membrane recovery variable
};

#endif
