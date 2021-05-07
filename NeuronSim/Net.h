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
#include "StreamHelpers.h"

const uint8_t TAG_WIDTH('w');
const uint8_t TAG_HEIGHT('h');
const uint8_t TAG_DATA('d');
const uint8_t TAG_SPIKE_SHAPE('s');
const uint8_t TAG_SPIKE_DURATION('D');
const uint8_t TAG_COLOR('c');
const uint8_t TAG_END('E');

// Net is a templated implementation of Layer which handles the common
// need for all layer specializations to maintain a 2D array of neurons,
// while each using different types.
// This could also be achieved with a virtual base type and factory
// functions, but templates are neater and easier to optimize.
// The save and load functions assume that the Neuron template type is
// POD. If it contains references to memory elsewhere, or virtual functions,
// the save and load functions will need to be overridden.
// The main responsibility of specializations of this class is to implement the
// tick function from Layer, and provide any config information required.
template <typename Neuron>
class Net : public Layer
{
public:
	// Construct a Net with a given width and height
	Net(int width, int height);
	// Copy constructor
	Net(const Net & other);
	// Destructor
	virtual ~Net();

	// Save this Net and its neurons to file
	void save(const std::filesystem::path & path);
	// Load a previously saved Net
	void load(const std::filesystem::path & path);
	// Feeds spikes from a SpikeTrain into neuron inputs
	void receiveSpikes(float * spikes) override;
	// Feeds spikes from a SpikeTrain into neuron shunts
	void receiveShunts(float * shunts) override;
	// Loop though all neurons and fire spikes along synapses from
	// all neurons that are firing.
	void fireSpikes(SynapseMatrix * synapses, Spiker * spiker) override;
	// Resize this Net to a new width and height.
	void resize(int width, int height);
	// Return a pointer to the first neuron. Neurons are stored in a single
	// contiguous block of memory in row major order.
	inline Neuron * begin() { return &mNeurons[0]; }
	// Return a pointer to the first neuron. Neurons are stored in a single
	// contiguous block of memory in row major order.
	inline Neuron * end() { return &mNeurons.back() + 1; }
	// Return a pointer to the first neuron. Neurons are stored in a single
	// contiguous block of memory in row major order.
	inline const Neuron * begin() const { return &mNeurons[0]; }
	// Take a pointer to the start of an array of pixels and populate them
	// with the spiking information about each neuron. The pixels should be
	// in ABGR order.
	void paintSpikes(uint32_t * image) override;
	// Reset all neurons in this net to their initial value
	void clear() override;
	// Inject a spike into a chosen neuron, with the given weight.
	// This is always an excitatory spike, not a shunting input.
	void inject(int col, int row, float weight) override;
protected:
	// Perform the default input behaviour for incoming spikes. The inputs
	// are divided by the shunt and then added to the potential of the neuron.
	// It is not required to call this function for neurons which work in other
	// ways. For those that do it would normally be called as the first step in
	// the implementation of tick.
	void processDendrites();
private:
	// Internal implementation detail of the fireSpikes function
	inline Synapse * fireSynapseSegment(Spiker * spiker, int cs, int ce, int dst, Synapse * synapse);
protected:
	// The block of data that stores the neurons.
	std::vector<Neuron> mNeurons;
};

template <typename Neuron>
Net<Neuron>::Net(int width, int height) :
	Layer(width, height)
{
	LOG("Creating layer [" << mName << "] size: " << mWidth << " x " << mHeight);
	// We have to call this explicitly - the v-table wasn't set up yet when the layer constructor
	// was called so it can't call it for us.
	resize(width, height);
}

template <typename Neuron>
Net<Neuron>::Net(const Net & other) :
	Layer(other)
{
	mNeurons = other.mNeurons;
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
	LOG("Saving layer to [" << filename << "]");
	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
	if (ofs)
	{
		writePod(TAG_WIDTH, ofs);
		writePod(mWidth, ofs);
		writePod(TAG_HEIGHT, ofs);
		writePod(mHeight, ofs);
		writePod(TAG_SPIKE_SHAPE, ofs);
		writePod(uint8_t(mSpike.shape()), ofs);
		writePod(TAG_SPIKE_DURATION, ofs);
		writePod(mSpike.duration(), ofs);
		writePod(TAG_COLOR, ofs);
		writePod(mColor, ofs);
		writePod(TAG_DATA, ofs);
		ofs.write(reinterpret_cast<char *>(&mNeurons[0]), mWidth * mHeight * sizeof(Neuron));
		writePod(TAG_END, ofs);
	}
	else
	{
		NEURONTHROW("Failed to write [" << filename << "]");
	}
	filename.replace_extension(CONFIG_EXTENSION);
	getConfig().write(filename);
}

template <typename Neuron>
void Net<Neuron>::load(const std::filesystem::path & path)
{
	LOG("Loading layer from [" << path << "]");
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	mName = path.stem().string();
	int width = 0;
	int height = 0;
	uint8_t shape = 0;
	int duration = 1;
	bool end = false;
	while(!end && ifs && ifs.good())
	{
		uint8_t tag = 0;
		readPod(tag, ifs);
		switch (tag)
		{
		case TAG_WIDTH:
			readPod(width, ifs);
			break;
		case TAG_HEIGHT:
			readPod(height, ifs);
			break;
		case TAG_DATA:
			if (!width || !height)
			{
				NEURONTHROW("Corrupt layer file [" << path << "]");
			}
			resize(width, height);
			ifs.read(reinterpret_cast<char *>(&mNeurons[0]), mWidth * mHeight * sizeof(Neuron));
			break;
		case TAG_SPIKE_SHAPE:
			readPod(shape, ifs);
			break;
		case TAG_SPIKE_DURATION:
			readPod(duration, ifs);
			break;
		case TAG_COLOR:
			readPod(mColor, ifs);
		case TAG_END:
			end = true;
			break;
		default:
			NEURONTHROW("Corrupt layer file [" << path.string() << "] unknown tag [" << tag << "]");
			break;
		}
	}
	setSpike(Spike::Shape(shape), duration);

	auto filename = path;
	filename.replace_extension(CONFIG_EXTENSION);
	ConfigSet config;
	config.read(filename);
	setConfig(config);
}

template <typename Neuron>
void Net<Neuron>::receiveSpikes(float * spikes)
{
	for (Neuron * cell = begin(); cell != end(); cell++)
	{
		cell->input += *spikes++;
	}
}

template <typename Neuron>
void Net<Neuron>::receiveShunts(float * shunts)
{
	for (Neuron * cell = begin(); cell != end(); cell++)
	{
		cell->shunt += *shunts++;
	}
}

// The models I have seen for shunting inhibition do not model it in any
// sophisticated manner. This is essentially a 100% leaky integrator, and
// I do not know if that is good or not, or what happens if we change it.
template <typename Neuron>
void Net<Neuron>::processDendrites()
{
	for (Neuron * iter = begin(); iter != end(); iter++)
	{
		iter->input /= iter->shunt;
		iter->shunt = 1.0f;
	}
}

template <typename Neuron>
inline Synapse * Net<Neuron>::fireSynapseSegment(Spiker * spiker, int cs, int ce, int dst, Synapse * synapse)
{
	dst += cs;
	for (int tc = cs; tc < ce; tc++)
	{
		spiker->fire(mSpike, dst++, synapse->weight, synapse->delay);
		synapse++;
	}
	return synapse;
}

// Default implementation of fireSpikes from Layer.
// It is expected that all types of layer/neuron will derive from
// net and not override this, but it is not required.
// This function works on the assumption that the Neuron template
// type has a public "firing" member variable that can be evaluated
// to true or false.
// This implementation wraps around at the edges of the array of neurons,
// which it does by dividing the synapse matrix into 9 sections and looping
// over each.
// An optimization _might_ be special casing the neurons near the edge,
// but has not been tested.
template <typename Neuron>
void Net<Neuron>::fireSpikes(SynapseMatrix * synapses, Spiker * spiker)
{
	Neuron * cell = begin();
	for (int rr = 0; rr < mHeight; rr++)
	{
		// For each row we have 3 sets of rows available to the synapsess:
		// those wrapping upwards, wrapping downwards, and not wrapping.
		int lowRowBegin = synapses->lowWrapRowBegin(rr, mHeight);
		int lowRowEnd = synapses->lowWrapRowEnd(rr, mHeight);
		int normRowBegin = synapses->normRowBegin(rr, mHeight);
		int normRowEnd = synapses->normRowEnd(rr, mHeight);
		int highRowBegin = synapses->highWrapRowBegin(rr, mHeight);
		int highRowEnd = synapses->highWrapRowEnd(rr, mHeight);

		int dst;
		for (int cc = 0; cc < mWidth; cc++)
		{
			if (cell->firing)
			{
				// For each column we have 3 sets of columns available to the
				// synapses: those wrapping left, wrapping right, and not wrapping.
				int lowColBegin = synapses->lowWrapColBegin(cc, mWidth);
				int lowColEnd = synapses->lowWrapColEnd(cc, mWidth);
				int normColBegin = synapses->normColBegin(cc, mWidth);
				int normColEnd = synapses->normColEnd(cc, mWidth);
				int highColBegin = synapses->highWrapColBegin(cc, mWidth);
				int highColEnd = synapses->highWrapColEnd(cc, mWidth);

				// The following loops have been manually unrolled. This is the innermost loop
				// of the entire system and some sacrifices towards optimization are justified.
				Synapse * synapse = synapses->begin();
				for (int tr = lowRowBegin; tr < lowRowEnd; tr++)
				{
					dst = mWidth * (rr + tr) + cc;
					synapse = fireSynapseSegment(spiker, lowColBegin, lowColEnd, dst, synapse);
					synapse = fireSynapseSegment(spiker, normColBegin, normColEnd, dst, synapse);
					synapse = fireSynapseSegment(spiker, highColBegin, highColEnd, dst, synapse);
				}
				for (int tr = normRowBegin; tr < normRowEnd; tr++)
				{
					dst = mWidth * (rr + tr) + cc;
					synapse = fireSynapseSegment(spiker, lowColBegin, lowColEnd, dst, synapse);
					synapse = fireSynapseSegment(spiker, normColBegin, normColEnd, dst, synapse);
					synapse = fireSynapseSegment(spiker, highColBegin, highColEnd, dst, synapse);
				}
				for (int tr = highRowBegin; tr < highRowEnd; tr++)
				{
					dst = mWidth * (rr + tr) + cc;
					synapse = fireSynapseSegment(spiker, lowColBegin, lowColEnd, dst, synapse);
					synapse = fireSynapseSegment(spiker, normColBegin, normColEnd, dst, synapse);
					synapse = fireSynapseSegment(spiker, highColBegin, highColEnd, dst, synapse);
				}
			}
			cell++;
		}
	}
}

template <typename Neuron>
void Net<Neuron>::resize(int width, int height)
{
	if (!mNeurons.empty())
		clear();
	Layer::resize(width, height);
	int size = mWidth * mHeight;
	mNeurons.resize(size);
}

template <typename Neuron>
void Net<Neuron>::paintSpikes(uint32_t * image)
{
	uint32_t * pixel = image;
	for (Neuron * neuron = begin(); neuron != end(); neuron++)
	{
		*pixel++ = 0xFF000000 | (neuron->firing * 0xFFFFFFFF);
	}
}

template <typename Neuron>
inline void Net<Neuron>::clear()
{
	for (auto neuron = begin(); neuron != end(); neuron++)
	{
		new (&*neuron) Neuron;
	}
}

template <typename Neuron>
void Net<Neuron>::inject(int col, int row, float weight)
{
	mNeurons[row * mWidth + col].input += weight;
}

template <typename Neuron>
std::ostream & operator<<(std::ostream & os, const Net<Neuron> & net)
{
	const Neuron * neuron = net.begin();
	for (int rr = 0; rr < net.height(); rr++)
	{
		for (int cc = 0; cc < net.width(); cc++)
		{
			os << neuron->input << (neuron->firing?"*":" ") << "   ";
			++neuron;
		}
		os << "\n";
	}
	return os;
}

#endif
