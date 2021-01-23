#pragma once

#include <cstdint>

class Cell
{
public:
	Cell() : input(0.0f), output(0.0f) {}
	virtual ~Cell() {}

	uint32_t colour() { return 0xFF000000 + uint64_t(output * 0x00FFFFFF); }
public:
	float input;
	float output;
};

