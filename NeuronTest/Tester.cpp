#include "Tester.h"

#include <iostream>

#include "TestVec3f.h"
#include "TestMat33f.h"
#include "TestSpikeProcessor.h"

using namespace std;

Tester::Tester()
{
	mTests.push_back(make_shared<TestVec3f>());
	mTests.push_back(make_shared<TestMat33f>());
	mTests.push_back(make_shared<TestSpikeProcessor>());
}

Tester::~Tester()
{

}

void Tester::run()
{
	mPasses = 0;
	mFails = 0;
	mExceptions = 0;
	for (auto test : mTests)
	{
		cout << "Running [" << test->name() << "]\n";
		try
		{
			test->run();
		}
		catch (const runtime_error & re)
		{
			cout << "  Exception thrown [" << re.what() << "]\n";
			++mExceptions;
		}
		cout << "  " << test->passes() << " passed";
		if (test->fails())
		{
			cout << " " << test->fails() << " FAILED";
			++mFails;
		}
		else
		{
			++mPasses;
		}
		cout << "\n";
	}

	cout << "\nPassed:     " << mPasses << "\nFailed:     " << mFails << "\nExceptions: " << mExceptions << "\n\n";
}
