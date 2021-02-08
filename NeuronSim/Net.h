#ifndef NET_H
#define NET_H

#include "Layer.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Constants.h"
#include "Exception.h"
#include "Log.h"

template <typename Neuron>
class Net : public Layer
{
public:
	Net(int width, int height);
	virtual ~Net();

	void save(const std::filesystem::path & path);
	void load(const std::filesystem::path & path);
	void tick(SynapseMatrix * synapses);
	void resize(int width, int height);
	inline Neuron * row(int r) { return &mNeurons[mWidth * r]; }
	void paint(uint32_t * image);
	void fire(int col, int row, float weight, int delay);
	void clear();
private:
	inline void tickSegment(int cs, int ce, Neuron * dst, Synapse * synapse);
protected:
	std::vector<Neuron> mNeurons;
};

template <typename Neuron>
Net<Neuron>::Net(int width, int height) :
	Layer(width, height)
{
	LOG("Creating layer");
	// We have to call this explicitly - the v-table wasn't set up yet when the layer constructor
	// was called so it can't call it for us.
	resize(width, height);
}

template <typename Neuron>
Net<Neuron>::~Net()
{
	LOG("Destroying layer [" << mName << "] size: " << mWidth << " x " << mHeight);
}

// This function works for any neuron type that can be treated as a block of data.
// If a neuron contains references to external data then this will need to be
// overloaded.
template <typename Neuron>
void Net<Neuron>::save(const std::filesystem::path & path)
{
	auto filename = path / mName;
	filename.replace_extension(LAYER_EXTENSION);
	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
	if (ofs)
	{
		ofs.write(reinterpret_cast<char *>(&mWidth), sizeof(mWidth));
		ofs.write(reinterpret_cast<char *>(&mHeight), sizeof(mHeight));
		ofs.write(reinterpret_cast<char *>(&mNeurons[0]), mWidth * mHeight * sizeof(Neuron));
		mSpikeProcessor->save(ofs, &mNeurons[0].input, &mNeurons.back().input);
	}
	else
	{
		NEURONTHROW("Failed to write [" << filename << "]");
	}
}

template <typename Neuron>
void Net<Neuron>::load(const std::filesystem::path & path)
{
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	if (ifs)
	{
		mName = path.stem().string();
		int width;
		int height;
		ifs.read(reinterpret_cast<char *>(&width), sizeof(width));
		ifs.read(reinterpret_cast<char *>(&height), sizeof(height));
		resize(width, height);
		ifs.read(reinterpret_cast<char *>(&mNeurons[0]), mWidth * mHeight * sizeof(Neuron));
		mSpikeProcessor->load(ifs, &mNeurons[0].input);
	}
	else
	{
		NEURONTHROW("Failed to read [" << path.string() << "]");
	}
}

template <typename Neuron>
inline void Net<Neuron>::tickSegment(int cs, int ce, Neuron * dst, Synapse * synapse)
{
	dst += cs;
	for (int tc = cs; tc < ce; ++tc)
	{
		mSpikeProcessor->fire(&dst->input, synapse->weight, synapse->delay);
		++dst;
		++synapse;
	}
}

template <typename Neuron>
void Net<Neuron>::tick(SynapseMatrix * synapses)
{
	auto target = synapses->target();
	if (!target)
		return;
	Net<Neuron> * targetNet = static_cast<Net<Neuron> *>(target.get());

	for (int rr = 0; rr < mHeight; ++rr)
	{
		int lowRowBegin = synapses->lowWrapRowBegin(rr, mHeight);
		int lowRowEnd = synapses->lowWrapRowEnd(rr, mHeight);
		int normRowBegin = synapses->normRowBegin(rr, mHeight);
		int normRowEnd = synapses->normRowEnd(rr, mHeight);
		int highRowBegin = synapses->highWrapRowBegin(rr, mHeight);
		int highRowEnd = synapses->highWrapRowEnd(rr, mHeight);

		Neuron * cell = row(rr);
		Neuron * dst;
		for (int cc = 0; cc < mWidth; ++cc)
		{
			if (cell->firing)
			{
				int lowColBegin = synapses->lowWrapColBegin(cc, mWidth);
				int lowColEnd = synapses->lowWrapColEnd(cc, mWidth);
				int normColBegin = synapses->normColBegin(cc, mWidth);
				int normColEnd = synapses->normColEnd(cc, mWidth);
				int highColBegin = synapses->highWrapColBegin(cc, mWidth);
				int highColEnd = synapses->highWrapColEnd(cc, mWidth);
				// TODO - these steps are a waste of time we are incrementing the synapse
				// in the tickSegment function and then forgetting and recalculating where
				// it ends up - we should probably inline that function manually.
				int lowStep = std::max(0, lowColEnd - lowColBegin);
				int normStep = std::max(0, normColEnd - normColBegin);
				int highStep = std::max(0, highColEnd - highColBegin);
				Synapse * synapse = synapses->begin();
				for (int tr = lowRowBegin; tr < lowRowEnd; ++tr)
				{
					dst = targetNet->row(rr + tr) + cc;
					tickSegment(lowColBegin, lowColEnd, dst, synapse);
					synapse += lowStep;
					tickSegment(normColBegin, normColEnd, dst, synapse);
					synapse += normStep;
					tickSegment(highColBegin, highColEnd, dst, synapse);
					synapse += highStep;
				}
				for (int tr = normRowBegin; tr < normRowEnd; ++tr)
				{
					dst = targetNet->row(rr + tr) + cc;
					tickSegment(lowColBegin, lowColEnd, dst, synapse);
					synapse += lowStep;
					tickSegment(normColBegin, normColEnd, dst, synapse);
					synapse += normStep;
					tickSegment(highColBegin, highColEnd, dst, synapse);
					synapse += highStep;
				}
				for (int tr = highRowBegin; tr < highRowEnd; ++tr)
				{
					dst = targetNet->row(rr + tr) + cc;
					tickSegment(lowColBegin, lowColEnd, dst, synapse);
					synapse += lowStep;
					tickSegment(normColBegin, normColEnd, dst, synapse);
					synapse += normStep;
					tickSegment(highColBegin, highColEnd, dst, synapse);
					synapse += highStep;
				}
			}
			++cell;
		}
	}
}

template <typename Neuron>
void Net<Neuron>::resize(int width, int height)
{
	Layer::resize(width, height);
	int size = mWidth * mHeight;
	mNeurons.resize(size);
}

template <typename Neuron>
void Net<Neuron>::paint(uint32_t * image)
{
	for (int rr = 0; rr < mHeight; ++rr)
	{
		uint32_t * pixel = image + rr * mWidth;
		Neuron * neuron = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			*pixel = neuron->color();
			++pixel;
			++neuron;
		}
	}
}

template <typename Neuron>
inline void Net<Neuron>::fire(int cc, int rr, float weight, int delay)
{
	Neuron * neuron = row(rr) + cc;
	mSpikeProcessor->fire(&neuron->input, weight, delay);
}

template <typename Neuron>
inline void Net<Neuron>::clear()
{
	for (int rr = 0; rr < mHeight; ++rr)
	{
		Neuron * neuron = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			new (neuron) Neuron;
			++neuron;
		}
	}
}


#endif
