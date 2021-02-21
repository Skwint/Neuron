#ifndef SPIKER_H
#define SPIKER_H

class Spike;

class Spiker
{
public:
	virtual void fire(const Spike & spike, int index, float weight, int delay) = 0;
};

#endif
