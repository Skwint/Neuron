#ifndef CELL_H
#define CELL_H

// This is a base class for neurons that contains the data the base layer class
// assumes will be present. A neuron does not have to derive from this class,
// nor does it have to have these three data items, provided that the appropriate
// functions of the layer are overridden.
struct Cell
{
	Cell() : input(0.0f), shunt(1.0f), firing(false) {}
	float input;
	float shunt;
	bool firing;

};

#endif
