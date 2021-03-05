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
	typedef std::vector<float> Frame;
public:
	SpikeTrain();
	SpikeTrain(const SpikeTrain & other);
	SpikeTrain(std::shared_ptr<Layer> source, std::shared_ptr<Layer> target, int maxDelay, bool shunting);
	virtual ~SpikeTrain();

	std::shared_ptr<Layer> source() { return mSource; }
	std::shared_ptr<Layer> target() { return mTarget; }

	void tick();
	void clear();
	bool shunting() { return mShunting; }
	float currentSpikeDensity();
	void save(const std::filesystem::path & path);
	void load(const std::filesystem::path & path);

public: // From Spiker
	void fire(const Spike & spike, int index, float weight, int delay) override;

private:
	std::shared_ptr<Layer> mSource;
	std::shared_ptr<Layer> mTarget;
	std::vector<Frame> mFrames;
	bool mShunting;
	int mCurrentFrame;
};

#endif
