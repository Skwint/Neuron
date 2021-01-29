#include "Test.h"

#include <iostream>

Test::Test()
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
		std::cout << file << ":" << line << " [" << expr << "] FAILED\n";
		++mFails;
	}
	return value;
}
