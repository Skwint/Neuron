#ifndef NET_H
#define NET_H

#include <vector>

#include "Layer.h"

template <typename Neuron>
class Net : public Layer
{
public:
	Net(int width = 512, int height = 512);
	virtual ~Net();

	void tick();

	void resize(int width, int height);
	Neuron * row(int r) { return &mNeurons[1 + mWidth * r]; }
	int rowStep() { return mWidth; }
	void paint(uint32_t * image, int rowStep, int left, int top, int width, int height);
	void paint(uint32_t * image);
protected:
	std::vector<Neuron> mNeurons;
};

template <typename Neuron>
Net<Neuron>::Net(int width, int height) :
	Layer(width, height)
{
	// We have to call this explicitly - the v-table wasn't set up yet when the layer constructor
	// was called so the polymorph didn't happen.
	resize(width, height);
}

template <typename Neuron>
Net<Neuron>::~Net()
{

}

template <typename Neuron>
void Net<Neuron>::tick()
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
	return paint(image, mWidth, 0, 0, mWidth, mHeight);
}

template <typename Neuron>
void Net<Neuron>::paint(uint32_t * image, int rowStep, int left, int top, int width, int height)
{
	for (int rr = top; rr < top + height; ++rr)
	{
		uint32_t * pixel = image + left + rr * rowStep;
		Neuron * neuron = row(rr) + left;
		for (int cc = left; cc < left + width; ++cc)
		{
			*pixel = neuron->colour();
			++pixel;
			++neuron;
		}
	}
}

#endif
