#pragma once
#include "Test.h"

#include "Neuron/Vec3f.h"

// Test class for testing the 3D vector class.
// @see Vec3f
class TestVec3f :
	public Test
{
public:
	TestVec3f();
	~TestVec3f();

	std::string name() { return "Vec3f"; }
	void run();
};

