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
const uint8_t TAG_END('E');

//
// The Neuron template argument should not contain virtual functions
// If it is necessary it can be done, but the save and load functions
// will then need to be overridden.
template <typename Neuron>
class Net : public Layer
{
public:
	Net(int width, int height);
	Net(const Net & other);
	virtual ~Net();

	void save(const std::filesystem::path & path);
	void load(const std::filesystem::path & path);
	void receiveSpikes(float * spikes) override;
	void receiveShunts(float * shunts) override;
	void preTick() override;
	void tick(SynapseMatrix * synapses, Spiker * spiker) override;
	void resize(int width, int height);
	inline Neuron * row(int r) { return &mNeurons[mWidth * r]; }
	inline Neuron * first() { return &mNeurons[0]; }
	inline const Neuron * first() const { return &mNeurons[0]; }
	void paint(uint32_t * image);
	void clear();
	void fire(int col, int row, float weight) override;
private:
	inline void tickSegment(Spiker * spiker, int cs, int ce, int dst, Synapse * synapse);
	void * begin() { return &mNeurons[0]; }
	void * end() { return &mNeurons.back() + 1; }
protected:
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
	Neuron * iter = &mNeurons[0];
	float * spike = spikes;
	for (int num = (int)mNeurons.size(); num; --num)
	{
		iter->input += *spike;
		++spike;
		++iter;
	}
}

template <typename Neuron>
void Net<Neuron>::receiveShunts(float * shunts)
{
	Neuron * iter = &mNeurons[0];
	float * shunt = shunts;
	for (int num = (int)mNeurons.size(); num; --num)
	{
		iter->shunt += *shunt;
		++shunt;
		++iter;
	}
}

template <typename Neuron>
void Net<Neuron>::preTick()
{
	Neuron * iter = &mNeurons[0];
	for (int num = (int)mNeurons.size(); num; --num)
	{
		iter->input /= iter->shunt;
		iter->shunt = 1.0f;
		++iter;
	}
}

template <typename Neuron>
inline void Net<Neuron>::tickSegment(Spiker * spiker, int cs, int ce, int dst, Synapse * synapse)
{
	dst += cs;
	for (int tc = cs; tc < ce; ++tc)
	{
		spiker->fire(mSpike, dst, synapse->weight, synapse->delay);
		++dst;
		++synapse;
	}
}

template <typename Neuron>
void Net<Neuron>::tick(SynapseMatrix * synapses, Spiker * spiker)
{
	bool shunt = synapses->isShunt();

	Neuron * cell = &mNeurons[0];
	for (int rr = 0; rr < mHeight; ++rr)
	{
		int lowRowBegin = synapses->lowWrapRowBegin(rr, mHeight);
		int lowRowEnd = synapses->lowWrapRowEnd(rr, mHeight);
		int normRowBegin = synapses->normRowBegin(rr, mHeight);
		int normRowEnd = synapses->normRowEnd(rr, mHeight);
		int highRowBegin = synapses->highWrapRowBegin(rr, mHeight);
		int highRowEnd = synapses->highWrapRowEnd(rr, mHeight);

		int dst;
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
					dst = mWidth * (rr + tr) + cc;
					tickSegment(spiker, lowColBegin, lowColEnd, dst, synapse);
					synapse += lowStep;
					tickSegment(spiker, normColBegin, normColEnd, dst, synapse);
					synapse += normStep;
					tickSegment(spiker, highColBegin, highColEnd, dst, synapse);
					synapse += highStep;
				}
				for (int tr = normRowBegin; tr < normRowEnd; ++tr)
				{
					dst = mWidth * (rr + tr) + cc;
					tickSegment(spiker, lowColBegin, lowColEnd, dst, synapse);
					synapse += lowStep;
					tickSegment(spiker, normColBegin, normColEnd, dst, synapse);
					synapse += normStep;
					tickSegment(spiker, highColBegin, highColEnd, dst, synapse);
					synapse += highStep;
				}
				for (int tr = highRowBegin; tr < highRowEnd; ++tr)
				{
					dst = mWidth * (rr + tr) + cc;
					tickSegment(spiker, lowColBegin, lowColEnd, dst, synapse);
					synapse += lowStep;
					tickSegment(spiker, normColBegin, normColEnd, dst, synapse);
					synapse += normStep;
					tickSegment(spiker, highColBegin, highColEnd, dst, synapse);
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
inline void Net<Neuron>::clear()
{
	for (auto neuron = mNeurons.begin(); neuron != mNeurons.end(); ++neuron)
	{
		new (&*neuron) Neuron;
	}
}

template <typename Neuron>
void Net<Neuron>::fire(int col, int row, float weight)
{
	mNeurons[row * mWidth + col].input += weight;
}

template <typename Neuron>
std::ostream & operator<<(std::ostream & os, const Net<Neuron> & net)
{
	const Neuron * neuron = net.first();
	for (int rr = 0; rr < net.height(); ++rr)
	{
		for (int cc = 0; cc < net.width(); ++cc)
		{
			os << neuron->input << (neuron->firing?"*":" ") << "   ";
			++neuron;
		}
		os << "\n";
	}
	return os;
}

#endif
