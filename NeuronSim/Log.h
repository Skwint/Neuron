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
	Log(); // intentionally unimplmented;
	// Initialise the logging system with a log file path.
	// Call this function once at startup.
	static void to(const std::string & path);
	// Close the file initialised in to().
	// Call this function once before exiting.
	static void finish();
	// Return a stream into which information can be logged.
	static std::stringstream & begin();
	// Write the contents of the stream returned by begin()
	// into the log file.
	static void end();

private:
	// A stream for collecting information to be logged
	static std::stringstream mStream;
	// A stream into which to put log messages to write them
	static std::ofstream mFile;
};

// While I would prefer to avoid the use of macros in C++, I do not know a way to
// achieve an interface as neat as this one without them.
#ifndef NDEBUG
#define LOGDEBUG(msg) LOG(msg)
#else
#define LOGDEBUG(msg)
#endif
// LOG can be used with any number of arguments in stream form, i.e.
// LOG("example " << message << " number " << n);
// provided that each argument has an appropriate << operator.
#define LOG(msg) { Log::begin() << msg; Log::end(); }

#endif
