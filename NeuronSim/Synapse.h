#ifndef SYNAPSE_H
#define SYNAPSE_H

#include <cstdint>

struct Synapse
{
	Synapse(float weight = 1.0f, uint32_t delay = 0) : weight(weight), delay(delay) {}
	float weight;
	uint32_t delay;
};

#endif
