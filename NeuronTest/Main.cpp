#include <iostream>

#include "NeuronSim/Log.h"
#include "Tester.h"

// Main entry point
// Runs through every test and exits
// Although this catches exceptions it would be very unexpected for it
// to actually see one.
int main(int argc, char ** argv)
{
	try
	{
		Log::to("NeuronTest.log");
		Tester tester;
		tester.run();
	}
	catch (const std::runtime_error & re)
	{
		TEST_LOG("Test system failed with [" << re.what() << "]");
	}
	catch (...)
	{
		TEST_LOG("Test system failed with inexplicable error");
	}

	return 0;
}