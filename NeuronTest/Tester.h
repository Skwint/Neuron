#ifndef TESTER_H
#define TESTER_H

#include <functional>
#include <vector>
#include <memory>

#include "Test.h"

// Tester is a collection of individual tests (@see Test)
class Tester
{
public:
	Tester();
	~Tester();

	void run();

private:
	std::vector<std::function<std::shared_ptr<Test>()>> mTests;
	int mPasses;
	int mFails;
	int mExceptions;
};

#endif
