#pragma once

#include <vector>

#include "Cell.h"

class Net
{
public:
	Net(int width = 512, int height = 512);
	virtual ~Net();

	virtual void tick();

	void resize(int width, int height);
	Cell * row(int r) { return &cells[1 + (mWidth + 2) * (r + 1)]; }
	int rowStep() { return mWidth + 2; }
	int width() { return mWidth; }
	int height() { return mHeight; }
	uint32_t * image();
protected:
	int mWidth;
	int mHeight;
	std::vector<Cell> cells;
	std::vector<uint32_t> mImage;
};

