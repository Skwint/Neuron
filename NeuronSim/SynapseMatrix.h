#ifndef SYNAPSE_MATRIX_H
#define SYNAPSE_MATRIX_H

#include <vector>

#include "Synapse.h"

class SynapseMatrix
{
public:
	SynapseMatrix();
	SynapseMatrix(int width, int height);
	~SynapseMatrix();

	inline int height() { return mHeight; }
	inline int width() { return mWidth; }
	inline Synapse * synapse(int row, int col) { return &mSynapses[row * mWidth + col]; }
	void setSize(int width, int height);
	void loadImage(uint32_t * pixels, int width, int height);

private:
	int mWidth;
	int mHeight;
	std::vector<Synapse> mSynapses;
};

#endif
