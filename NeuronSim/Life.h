#pragma once
#include "Net.h"
class Life : public Net
{
public:
	Life(int width = 512, int height = 512);
	~Life();

	void tick();
};

