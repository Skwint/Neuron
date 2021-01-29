#include <iostream>

#include "Tester.h"

// Main entry point
// Runs through every test and exits
// Although this catches exceptions it would be very unexpected for it
// to actually see one.
int main(int argc, char ** argv)
{
	try
	{
		Tester tester;
		tester.run();
	}
	catch (const std::runtime_error & re)
	{
		std::cout << "Test system failed with [" << re.what() << "]\n";
	}
	catch (...)
	{
		std::cout << "Test system failed with inexplicable error\n";
	}

	return 0;
}