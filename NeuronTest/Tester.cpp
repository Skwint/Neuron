#include "Tester.h"

#include <iostream>

#include "TestAutomaton.h"
#include "TestConfigs.h"
#include "TestLife.h"
#include "TestMat33f.h"
#include "TestNet.h"
#include "TestPerformance.h"
#include "TestSpikeTrain.h"
#include "TestStability.h"
#include "TestVec3f.h"

using namespace std;

Tester::Tester()
{
	mTests.push_back([] { return make_shared<TestVec3f>(); });
	mTests.push_back([] { return make_shared<TestMat33f>(); });
	mTests.push_back([] { return make_shared<TestConfigs>(); });
	mTests.push_back([] { return make_shared<TestSpikeTrain>(); });
	mTests.push_back([] { return make_shared<TestNet>(); });
	mTests.push_back([] { return make_shared<TestAutomaton>(); });
	mTests.push_back([] { return make_shared<TestLife>(); });
	mTests.push_back([] { return make_shared<TestStability>(); });
	mTests.push_back([] { return make_shared<TestPerformance>(); });
}

Tester::~Tester()
{

}

void Tester::run()
{
	mPasses = 0;
	mFails = 0;
	mExceptions = 0;
	for (auto allocator : mTests)
	{
		auto test = allocator();
		TEST_LOG("Running [" << test->name() << "]");
		bool ok = false;
		try
		{
			test->run();
			ok = true;
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
		else if (ok)
		{
			++mPasses;
		}
	}

	TEST_LOG("--------------------------");
	TEST_LOG("Passed     : " << mPasses);
	TEST_LOG("Failed     : " << mFails);
	TEST_LOG("Exceptions : " << mExceptions);
}
