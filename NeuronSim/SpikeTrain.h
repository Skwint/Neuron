#ifndef SPIKE_TRAIN_H
#define SPIKE_TRAIN_H

#include "Spiker.h"

#include <memory>
#include <vector>

class Layer;
class SynapseMatrix;
class Spike;

namespace std
{
	namespace filesystem
	{
		class path;
	}
}

// A spike train handles the set of spikes which are in transit from a source
// layer to a target layer. These may or may not be the same layer.
// Because these exist between two layers they act mostly as a data storage
// structure rather than encapsulating logic and behaviour.
// Not all spikes in a train are necessarily from the same SynapseMatrix,
// Nor are they always the same shape or length.
// Spikes are superimposed for arrival time to save memory.
class SpikeTrain : public Spiker
{
private:
	// Used internally to store all spike potentials for a given timestep
	// Acts as a circular buffer.
	typedef std::vector<float> Frame;
public:
	// Constrcutor for an uninitialized spike train
	SpikeTrain();
	// Copy constructor
	SpikeTrain(const SpikeTrain & other);
	// Constructor
	// source - the layer this spike train receives spikes from
	// taget - the layer this spike train feeds spikes into
	// maxDelay - the maximum possible time after firing that a spike can
	// still be influencing this train.
	// shunting - determines if the spikes will go to the input or shunt
	// fields of the target neuron.
	SpikeTrain(std::shared_ptr<Layer> source, std::shared_ptr<Layer> target, int maxDelay, bool shunting);
	// Destructor
	virtual ~SpikeTrain();

	// The source layer
	std::shared_ptr<Layer> source() { return mSource; }
	// The target layer
	std::shared_ptr<Layer> target() { return mTarget; }

	// Push all spike potentials for the current time step onto the 
	// target layer.
	void tick();
	// Remove all potentials from the train.
	void clear();
	// Returns true if this spike train targets the shunt instead of the input
	// of its target layer
	bool shunting() { return mShunting; }
	// Returns the proportion of the target layers neurons which are
	// going to receive a non zero input on the next tick
	float currentSpikeDensity();
	// Save this spike train to a file
	void save(const std::filesystem::path & path);
	// Load a spike train from file
	void load(const std::filesystem::path & path);

public: // From Spiker
	void fire(const Spike & spike, int index, float weight, int delay) override;

private:
	std::shared_ptr<Layer> mSource; //< The source of the spikes in this train
	std::shared_ptr<Layer> mTarget; //< The layer spikes are sent to
	std::vector<Frame> mFrames;     //< Circular buffer of spike potentials
	bool mShunting;                 //< True if we target shunts not inputs
	int mCurrentFrame;              //< Circular buffer management
};

#endif
