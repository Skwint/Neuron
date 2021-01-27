#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <sstream>

#define NEURONTHROW(msg) \
{ \
	std::stringstream stream; \
	stream << __FUNCTION__ << " " << msg; \
	throw std::runtime_error(stream.str()); \
}

#endif

