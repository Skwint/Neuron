#ifndef COMMON_H
#define COMMON_H

#include <iostream>

#include "NeuronSim/Log.h"

#define GEN_LOG(x) { LOG("  " << __FUNCTION__ << ":" << __LINE__ << " " << x); std::cout << x << "\n"; }

#endif
