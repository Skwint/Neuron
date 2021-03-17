#ifndef NEURON_LIFE_H
#define NEURON_LIFE_H

#include "Cell.h"

#include <cstdint>
#include <random>

// Debugging tool - see comment in destructor
static const float neuronLifeCheck(-12345.0f);

// The neuron type used by the Life automaton type.
// This is the simplest possible neuron, and in Life
// the shunt will always be 1.0
struct NeuronLife : public Cell
{
	NeuronLife()
	{
	}
	~NeuronLife()
	{
		// This was added because an off by one error during development managed to
		// evade detection by std::vector and caused a crash that took most of a day
		// to track down.
		// It works by setting the value of deleted neurons to something they can
		// never naturally attain, and then asserting if a neuron with those values
		// is ever accessed.
		// This is only present in Life, the performance of which is unimportant to us.
		// This exists so that it can be used as a debugging tool in future, should
		// we need it.
		// The automated stability test excercises this mechanism.
		input = neuronLifeCheck;
	}
};

#endif
