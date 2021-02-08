#include "Tester.h"

#include <iostream>

#include "TestAutomaton.h"
#include "TestConfigs.h"
#include "TestLife.h"
#include "TestMat33f.h"
#include "TestSpikeProcessor.h"
#include "TestStability.h"
#include "TestVec3f.h"

using namespace std;

Tester::Tester()
{
	mTests.push_back(make_shared<TestVec3f>());
	mTests.push_back(make_shared<TestMat33f>());
	mTests.push_back(make_shared<TestConfigs>());
	mTests.push_back(make_shared<TestSpikeProcessor>());
	mTests.push_back(make_shared<TestAutomaton>());
	mTests.push_back(make_shared<TestLife>());
	mTests.push_back(make_shared<TestStability>());
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
		TEST_LOG("Running [" << test->name() << "]");
		try
		{
			test->run();
		}
		catch (const runtime_error & re)
		{
			TEST_LOG("  Exception thrown [" << re.what() << "]");
			++mExceptions;
		}
		TEST_LOG("  " << test->passes() << " passed");
		if (test->fails())
		{
			TEST_LOG(" " << test->fails() << " FAILED");
			++mFails;
		}
		else
		{
			++mPasses;
		}
	}

	TEST_LOG("--------------------------");
	TEST_LOG("Passed     : " << mPasses);
	TEST_LOG("Failed     : " << mFails);
	TEST_LOG("Exceptions : " << mExceptions);
}
