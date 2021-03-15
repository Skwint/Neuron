#ifndef LOG_H
#define LOG_H

#include <sstream>
#include <vector>
#include <filesystem>

// Log is not conceptually a class - it is a container for a set of globally accessible
// functions that are used throughout the code base to report useful information
// to a log file. Log should not be instantiated (and can't be).
// The author considers this approach to be superior to the singleton anti-pattern.
class Log
{
public:
	Log(); // unimplmented;
	static void to(const std::string & path);
	static void finish();
	static std::stringstream & begin();
	static void end();

private:
	static std::stringstream mStream;
	static std::ofstream mFile;
};

// While we would prefer to avoid the use of macros in C++, we do not know a way to
// achieve an interface as neat as this one without them.
#ifndef NDEBUG
#define LOGDEBUG(msg) LOG(msg)
#else
#define LOGDEBUG(msg)
#endif
// LOG can be used with any number of arguments in stream form, i.e.
// LOG("example " << message << " number " << n);
#define LOG(msg) { Log::begin() << msg; Log::end(); }

#endif
