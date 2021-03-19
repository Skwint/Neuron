#ifndef SYNAPSE_MATRIX_H
#define SYNAPSE_MATRIX_H

#include <algorithm>
#include <filesystem>
#include <memory>
#include <vector>

#include "Synapse.h"

class Layer;

// A SynapseMatrix is a 2D array of Synapse objects, each of which has a weight and
// a delay. It can be thought of as being overlayed over a Layer with the center of
// the matrix on a neuron which is firing. Each Synapse is a connection to another
// neuron which will receive the spike.
// A SynapseMatrix is expected to be an odd number width and height to make the 
// meaning of the center clear. Even it has an even width or height the center
// will be rounded down to the lower row and column coordinates.
class SynapseMatrix
{
public:
	// A listener to inform owners of a SynapseMatrix that it has changed in a way
	// that might require action (such as a change in size or maximum delay)
	class Listener
	{
	public:
		virtual void synapseMatrixChanged(SynapseMatrix * matrix) = 0;
	};
	// The delay functions that can be applied to a matrix
	enum Delay
	{
		// No delay - spikes are received in the next time step
		DELAY_NONE = 0,
		// Linear delay - spikes travel at a fixed speed
		DELAY_LINEAR,
		// Grid delay - spikes travel along one axis at once at fixed speed
		DELAY_GRID,
		// One - spikes are reived after a delay of one time step
		DELAY_ONE,

		// The number of available delay functions
		DELAY_COUNT
	};
public:
	// Constrcutor
	SynapseMatrix(Listener * listener);
	// Constructor
	SynapseMatrix(Listener * listener, int width, int height);
	// Destructor
	~SynapseMatrix();

	void setSize(int width, int height);
	inline int height() { return mHeight; }
	inline int width() { return mWidth; }
	void setSource(std::shared_ptr<Layer> source) { mSource = source; }
	inline std::shared_ptr<Layer> source() { return mSource.lock(); }
	const std::string & sourceName();
	void setTarget(std::shared_ptr<Layer> target) { mTarget = target; }
	inline std::shared_ptr<Layer> target() { return mTarget.lock(); }
	const std::string & targetName();
	inline Synapse * synapse(int col, int row) { return &mSynapses[row * mWidth + col]; }
	inline Synapse * begin() { return &mSynapses[0]; }
	void setDelay(Delay delay);
	Delay delay() { return mDelay; }
	float weight() { return mWeight; }
	void loadImage(uint32_t * pixels, int width, int height, float weight, const std::string & name = std::string());
	const std::string & imageName() { return mImageName; }
	void load(const std::filesystem::path & path);
	void save(const std::filesystem::path & path);
	bool isShunt() { return mShunt; }
	void setShunt(bool shunt);
	uint32_t maximumDelay();

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
	Listener * mListener;
	int mWidth;
	int mHeight;
	float mWeight;
	Delay mDelay;
	bool mShunt;
	std::string mImageName;
	std::vector<Synapse> mSynapses;
	std::weak_ptr<Layer> mSource;
	std::weak_ptr<Layer> mTarget;
	std::string mSourceName;
	std::string mTargetName;
};

#endif
