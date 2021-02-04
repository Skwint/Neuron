#include "Test.h"

#include <iostream>

Test::Test() :
	mPasses(0),
	mFails(0)
{
}

Test::~Test()
{

}

void Test::run()
{
	mPasses = 0;
	mFails = 0;
}

bool Test::test(const std::string & file, int line, const std::string & expr, bool value)
{
	if (value)
	{
		++mPasses;
	}
	else
	{
		TEST_LOG(file << ":" << line << " [" << expr << "] FAILED");
		++mFails;
	}
	return value;
}
