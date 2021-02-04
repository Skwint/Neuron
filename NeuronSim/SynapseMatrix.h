#ifndef SYNAPSE_MATRIX_H
#define SYNAPSE_MATRIX_H

#include <algorithm>
#include <memory>
#include <vector>

#include "Synapse.h"

class Layer;

class SynapseMatrix
{
public:
	SynapseMatrix();
	SynapseMatrix(int width, int height);
	~SynapseMatrix();

	void setSize(int width, int height);
	inline int height() { return mHeight; }
	inline int width() { return mWidth; }
	void setSource(std::shared_ptr<Layer> source) { mSource = source; }
	inline std::shared_ptr<Layer> source() { return std::shared_ptr<Layer>(mSource); }
	void setTarget(std::shared_ptr<Layer> target) { mTarget = target; }
	inline std::shared_ptr<Layer> target() { return std::shared_ptr<Layer>(mTarget); }
	inline Synapse * synapse(int row, int col) { return &mSynapses[row * mWidth + col]; }
	inline Synapse * begin() { return &mSynapses[0]; }
	void loadImage(uint32_t * pixels, int width, int height);

	inline int lowWrapColBegin(int col, int width) { return std::max(0, col + width - mWidth / 2) - col; }
	inline int lowWrapColEnd(int col, int width) { return std::min(width, col + width + mWidth / 2 + 1) - col; }
	inline int lowWrapRowBegin(int row, int height) { return std::max(0, row + height - mHeight / 2) - row; }
	inline int lowWrapRowEnd(int row, int height) { return std::min(height, row + height + mHeight / 2 + 1) - row; }
	inline int normColBegin(int col, int width) { return std::max(0, col - mWidth / 2) - col; }
	inline int normColEnd(int col, int width) { return std::min(width, col + mWidth / 2 + 1) - col; }
	inline int normRowBegin(int row, int height) { return std::max(0, row - mHeight / 2) - row; }
	inline int normRowEnd(int row, int height) { return std::min(height, row + mHeight / 2 + 1) - row; }
	inline int highWrapColBegin(int col, int width) { return std::max(0, col - width - mWidth / 2) - col; }
	inline int highWrapColEnd(int col, int width) { return std::min(width, col - width + mWidth / 2 + 1) - col; }
	inline int highWrapRowBegin(int row, int height) { return std::max(0, row - height - mHeight / 2) - row; }
	inline int highWrapRowEnd(int row, int height) { return std::min(height, row - height + mHeight / 2 + 1) - row; }

private:
	int mWidth;
	int mHeight;
	std::vector<Synapse> mSynapses;
	std::weak_ptr<Layer> mSource;
	std::weak_ptr<Layer> mTarget;
};

#endif
