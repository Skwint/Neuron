#ifndef NET_H
#define NET_H

#include "Layer.h"

#include <algorithm>
#include <vector>

#include "Log.h"

template <typename Neuron>
class Net : public Layer
{
public:
	Net(int width, int height);
	virtual ~Net();

	void tick(SynapseMatrix * synapses);
	void resize(int width, int height);
	Neuron * row(int r) { return &mNeurons[mWidth * r]; }
	int rowStep() { return mWidth; }
	void paint(uint32_t * image);
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
					dst = cell + tr * rowStep();
					tickSegment(lowColBegin, lowColEnd, dst, synapse);
					synapse += lowStep;
					tickSegment(normColBegin, normColEnd, dst, synapse);
					synapse += normStep;
					tickSegment(highColBegin, highColEnd, dst, synapse);
					synapse += highStep;
				}
				for (int tr = normRowBegin; tr < normRowEnd; ++tr)
				{
					dst = cell + tr * rowStep();
					tickSegment(lowColBegin, lowColEnd, dst, synapse);
					synapse += lowStep;
					tickSegment(normColBegin, normColEnd, dst, synapse);
					synapse += normStep;
					tickSegment(highColBegin, highColEnd, dst, synapse);
					synapse += highStep;
				}
				for (int tr = highRowBegin; tr < highRowEnd; ++tr)
				{
					dst = cell + tr * rowStep();
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

#endif
