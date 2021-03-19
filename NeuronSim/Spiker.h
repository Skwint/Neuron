#ifndef SPIKER_H
#define SPIKER_H

class Spike;

// Spiker is an interface class which allows spikes to be fired without introducing
// a dependency on the things which actually fires the spikes.
class Spiker
{
public:
	// Fire a spike
	// spike - the shape of the spike to fire
	// index - the index into a layer of neurons of the recipient of the spike
	// weight - the weight multiplier for the spike
	// delay - the delay, in time steps, before the start of the spike reaches
	// the recipient.
	virtual void fire(const Spike & spike, int index, float weight, int delay) = 0;
};

#endif
