#include "log.h"

#include <fstream>
#include <filesystem>

std::stringstream Log::mStream;
std::ofstream Log::mFile;

void Log::to(const std::string & path)
{
	mFile.open(path, std::ios_base::out);
}

void Log::finish()
{
	mFile.close();
}

std::stringstream & Log::begin()
{
	return Log::mStream;
}

void Log::end()
{
	mFile << mStream.str() << std::endl;
	mStream.str("");
}
