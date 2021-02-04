#ifndef NET_H
#define NET_H

#include "Layer.h"

#include <vector>

#include "Log.h"

template <typename Neuron>
class Net : public Layer
{
public:
	Net(int width, int height);
	virtual ~Net();

	void resize(int width, int height);
	Neuron * row(int r) { return &mNeurons[mWidth * r]; }
	int rowStep() { return mWidth; }
	void paint(uint32_t * image);
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
