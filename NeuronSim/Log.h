#ifndef LOG_H
#define LOG_H

#include <sstream>
#include <vector>
#include <filesystem>

class Log
{
public:
	static void to(const std::string & path);
	static void finish();
	static std::stringstream & begin();
	static void end();

private:
	static std::stringstream mStream;
	static std::ofstream mFile;
};

#ifndef NDEBUG
#define LOGDEBUG(msg) LOG(msg)
#else
#define LOGDEBUG(msg)
#endif
#define LOG(msg) { Log::begin() << msg; Log::end(); }

#endif
