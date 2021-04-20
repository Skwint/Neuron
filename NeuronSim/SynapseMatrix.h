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

	// Resize this matrix. Calling this function does not initialize the weights
	// of the synapses and they will need to be set afterwards.
	void setSize(int width, int height);
	// Return the height of the matrix (in rows)
	inline int height() { return mHeight; }
	// Return the width of the matrix (in columns)
	inline int width() { return mWidth; }
	// Set the source layer that this matrix fires spikes from
	void setSource(std::shared_ptr<Layer> source) { mSource = source; }
	// Return the source layer that this matrix fires spikes from
	inline std::shared_ptr<Layer> source() { return mSource.lock(); }
	// Return the name of the source layer
	const std::string & sourceName();
	// Set the target layer that this matrix fires spikes to
	void setTarget(std::shared_ptr<Layer> target) { mTarget = target; }
	// Return the target layer that this matrix fires spikes to
	inline std::shared_ptr<Layer> target() { return mTarget.lock(); }
	// Return the name of the target layer
	const std::string & targetName();
	// Return the synapse at a given column and row
	inline Synapse * synapse(int col, int row) { return &mSynapses[row * mWidth + col]; }
	// Return a pointer to the first synapse (they are stored in row major format)
	inline Synapse * begin() { return &mSynapses[0]; }
	// Set the spike delay pattern for spikes fired through this matrix
	void setDelay(Delay delay);
	// Return the spike delay pattern for spikes fired through this matrix
	Delay delay() { return mDelay; }
	// Return the weight multiplier that was used in a previous call to loadImage
	float weight() { return mWeight; }
	// Load an image in ABGR format. Only the 8 bit red channel will be considered, and
	// the value will be scaled to between 0.0 and the value of the weight argument.
	void loadImage(uint32_t * pixels, int width, int height, float weight, const std::string & name = std::string());
	// Return the name passed in a previous call to loadImage. This name has no
	// meaning internally, but can be useful for keeping track in a GUI.
	const std::string & imageName() { return mImageName; }
	// Load a synapse matrix from a file on disk
	void load(const std::filesystem::path & path);
	// Save the synapse matrix to a file on disk
	void save(const std::filesystem::path & path);
	// Return true if spikes fired through this matrix target the shunting inhibition
	bool isShunt() { return mShunt; }
	// Set true to target the shunting inhibition instead of the input of target neurons
	void setShunt(bool shunt);
	// Calculate the maximum delay on data coming from spikes fired through this matrix
	uint32_t maximumDelay();

	// Convenience function for calculating coordinates wrapped around the low column edge
	inline int lowWrapColBegin(int col, int width) { return std::max(0, col + width - mWidth / 2) - col; }
	// Convenience function for calculating coordinates wrapped around the low column edge
	inline int lowWrapColEnd(int col, int width) { return std::min(width, col + width + mWidth / 2 + 1) - col; }
	// Convenience function for calculating coordinates wrapped around the low row edge
	inline int lowWrapRowBegin(int row, int height) { return std::max(0, row + height - mHeight / 2) - row; }
	// Convenience function for calculating coordinates wrapped around the low row edge
	inline int lowWrapRowEnd(int row, int height) { return std::min(height, row + height + mHeight / 2 + 1) - row; }
	// Convenience function for calculating coordinates wrapped which are not wrapped in columns
	inline int normColBegin(int col, int width) { return std::max(0, col - mWidth / 2) - col; }
	// Convenience function for calculating coordinates wrapped which are not wrapped in columns
	inline int normColEnd(int col, int width) { return std::min(width, col + mWidth / 2 + 1) - col; }
	// Convenience function for calculating coordinates wrapped which are not wrapped in rows
	inline int normRowBegin(int row, int height) { return std::max(0, row - mHeight / 2) - row; }
	// Convenience function for calculating coordinates wrapped which are not wrapped in rows
	inline int normRowEnd(int row, int height) { return std::min(height, row + mHeight / 2 + 1) - row; }
	// Convenience function for calculating coordinates wrapped around the high column edge
	inline int highWrapColBegin(int col, int width) { return std::max(0, col - width - mWidth / 2) - col; }
	// Convenience function for calculating coordinates wrapped around the high column edge
	inline int highWrapColEnd(int col, int width) { return std::min(width, col - width + mWidth / 2 + 1) - col; }
	// Convenience function for calculating coordinates wrapped around the high row edge
	inline int highWrapRowBegin(int row, int height) { return std::max(0, row - height - mHeight / 2) - row; }
	// Convenience function for calculating coordinates wrapped around the high row edge
	inline int highWrapRowEnd(int row, int height) { return std::min(height, row - height + mHeight / 2 + 1) - row; }

private:
	// The listener to inform of changes (usually the owning automaton object)
	Listener * mListener;
	// The width of the matrix
	int mWidth;
	// The height of the matrix
	int mHeight;
	// The weight multiplier. This is not directly used, but is saved and restored
	// so that users of the class can retain the information.
	float mWeight;
	// The delay function
	Delay mDelay;
	// Whether these synapses target shunting inhibition or input
	bool mShunt;
	// The name given when an image was loaded. This is not used, but is saved
	// and restored so that users of the class can retain the information.
	std::string mImageName;
	// The synapses, stored in row major form
	std::vector<Synapse> mSynapses;
	// The source layer from which spikes originate
	std::weak_ptr<Layer> mSource;
	// The target layer to which spikes are sent
	std::weak_ptr<Layer> mTarget;
	// The name of the source layer (necessary when loading synapses from file
	// without access to the layers).
	std::string mSourceName;
	// The name of the target layer (necessary when loading synapses from file
	// without access to the layers).
	std::string mTargetName;
};

#endif
