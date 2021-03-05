// Genetics.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"
#include "Control.h"

int main(int argc, char ** argv)
{
	try
	{
		Log::to("Genetics.log");
		Control control;
		control.run();
	}
	catch (const std::runtime_error & re)
	{
		GEN_LOG("Genetics system failed with [" << re.what() << "]");
	}
	catch (...)
	{
		GEN_LOG("Genetics system failed with inexplicable error");
	}

	return 0;
}

