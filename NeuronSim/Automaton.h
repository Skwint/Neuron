#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <memory>
#include <vector>

#include "ConfigSet.h"
#include "SpikeProcessor.h"

class Layer;
class LayerFactory;
class SynapseMatrix;

class Automaton
{
public:
	class Listener
	{
	public:
		virtual void automatonTypeChanged() {};
		virtual void automatonSizechanged(int width, int height) {};
		virtual void automatonLayerCreated(std::shared_ptr<Layer> layer) {};
		virtual void automatonLayerRemoved(std::shared_ptr<Layer> layer) {};
		virtual void automatonSynapsesCreated(std::shared_ptr<SynapseMatrix> synapses) {};
		virtual void automatonSynapsesRemoved(std::shared_ptr<SynapseMatrix> synapses) {};
	};
	class Lock
	{
	public:
		inline Lock();
		inline ~Lock();
	private:
		static bool mLocked;
	};
public:
	Automaton();
	~Automaton();

	void setNetworkType(const std::string & type);
	const std::string & networkType() const { return mType; }
	void setSize(int width, int height);
	int width() { return mWidth; }
	int height() { return mHeight; }
	std::shared_ptr<Layer> createLayer();
	void removeLayer(std::shared_ptr<Layer> layer);
	const std::vector<std::shared_ptr<Layer> > layers() { return mLayers; }
	std::shared_ptr<Layer> findLayer(const std::string & name);
	std::vector<std::string> typeNames();
	std::shared_ptr<SynapseMatrix> createSynapse();
	void removeSynapse(std::shared_ptr<SynapseMatrix> synapses);
	void addListener(Listener * listener);
	void removeListener(Listener * listener);
	void tick();
	void setSpike(const SpikeProcessor::Spike & spike);
	void clearLayers();

private:
	std::vector<Listener *> mListeners;
	std::string mType;
	int mWidth;
	int mHeight;
	std::unique_ptr<LayerFactory> mLayerFactory;
	std::shared_ptr<SpikeProcessor> mSpikeProcessor;
	std::vector<std::shared_ptr<Layer> > mLayers;
	std::vector<std::shared_ptr<SynapseMatrix> > mSynapses;
};

#endif
