#ifndef SYNAPSE_H
#define SYNAPSE_H

#include <cstdint>

// An individual synapse has a weight and a delay. This is a data item
// used by the SynapseMatrix and does not have internal logic of its own.
struct Synapse
{
	Synapse(float weight = 1.0f, uint32_t delay = 0) :
		weight(weight),
		delay(delay) {}
	float weight;
	uint32_t delay;
};

#endif
