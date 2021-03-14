#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <sstream>

// This is a simple wrapper around throwing a runtime exception
// Technically this is bad form, specifically in Out Of Memory situations
// But it seems unlikely that OOM will occur at the same time as one of
// these.
#define NEURONTHROW(msg) \
{ \
	std::stringstream stream; \
	stream << __FUNCTION__ << " " << msg; \
	throw std::runtime_error(stream.str()); \
}

#endif

