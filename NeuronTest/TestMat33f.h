#ifndef TESTMAT33F_H
#define TESTMAT33F_H

#include "Test.h"
class TestMat33f :
	public Test
{
public:
	TestMat33f();
	~TestMat33f();

	std::string name() { return "Mat33f"; }
	void run();
};

#endif
