#ifndef CONSTANTS_H
#define CONSTANTS_H

// This file constains constants which are shared between multiple parts
// of the library.

// PI. We define our own PI because of the amazing nonsense involved in
// getting PI in a cross platform manner from various compilers.
const double PI(3.141592653589793238462643383279502884);
// PI. We define our own PI because of the amazing nonsense involved in
// getting PI in a cross platform manner from various compilers.
const float PIf(3.141592653589793238462643383279502884f);
// A very small value. Mostly used for checking if floating point numbers
// are very close to each other, or very close to 0.
const float TINY_VALUE(0.000001f);

// The size of the automaton created at startup. Smaller in debug modes
// because debug modes are significantly slower.
#ifdef NDEBUG
const int DEFAULT_NET_SIZE(512);
#else
const int DEFAULT_NET_SIZE(64);
#endif

// File extension for saved layers
static const char * LAYER_EXTENSION(".layer");
// File extension for saved synapse matrices
static const char * SYNAPSE_EXTENSION(".synapse");
// File extension for saved spike trains going to inputs
static const char * SPIKE_EXTENSION(".spike");
// File extension for saved spike trains going to shunts
static const char * SHUNT_EXTENSION(".shunt");
// File extension for saved configurations
static const char * CONFIG_EXTENSION(".cfg");

#endif
