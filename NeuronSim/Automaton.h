#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <memory>
#include <vector>

#include "ConfigSet.h"
#include "SynapseMatrix.h"

class Layer;
class LayerFactory;
class SpikeTrain;

class Automaton : public SynapseMatrix::Listener
{
public:
	enum OperatingMode
	{
		MODE_NORMAL,
		MODE_DEPRESSED
	};
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
	// The lock is to detect (and throw exceptions when) a user of
	// this class attempts to add or remove a listener while the listeners
	// are being iterated.
	// TODO - fix it so that it remembers dead listeners and removes them later
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
	Automaton(const Automaton & other);
	~Automaton();

	void reset();
	void setNetworkType(const std::string & type);
	const std::string & networkType() const { return mType; }
	void setOperatingMode(OperatingMode mode) { mMode = mode; }
	void setSize(int width, int height);
	int width() { return mWidth; }
	int height() { return mHeight; }
	std::shared_ptr<Layer> createLayer();
	void removeLayer(std::shared_ptr<Layer> layer);
	void removeLayer(const std::string & name);
	const std::vector<std::shared_ptr<Layer> > layers() { return mLayers; }
	std::shared_ptr<Layer> findLayer(const std::string & name);
	std::vector<std::string> typeNames();
	std::shared_ptr<SynapseMatrix> createSynapse();
	void removeSynapse(std::shared_ptr<SynapseMatrix> synapses);
	const std::vector<std::shared_ptr<SynapseMatrix>> synapses() { return mSynapses; }
	void addListener(Listener * listener);
	void removeListener(Listener * listener);
	void tick();
	void clearLayers();
	// Save the current state of all layers and synapses to the given path
	void save(const std::filesystem::path & path);
	// Load a previously saved state from a given path
	void load(const std::filesystem::path & path);
	void recalculateSpikeTrains();
	LayerFactory * layerFactory() { return mLayerFactory.get(); }
	float currentSpikeDensity();

private: // From SynapseMatrix::Listener
	void synapseMatrixChanged(SynapseMatrix * matrix) override;

private:
	std::shared_ptr<Layer> createDetachedLayer();
	void attachLayer(std::shared_ptr<Layer> layer);
	std::shared_ptr<SynapseMatrix> createDetachedSynapses();
	void attachSynapses(std::shared_ptr<SynapseMatrix> synapses);
	void tickTargetLayer(Layer * target);
	void tickSourceLayer(Layer * source);

private:
	std::vector<Listener *> mListeners;
	std::string mType;
	OperatingMode mMode;
	int mWidth;
	int mHeight;
	std::unique_ptr<LayerFactory> mLayerFactory;
	std::vector<std::shared_ptr<Layer> > mLayers;
	std::vector<std::shared_ptr<SynapseMatrix> > mSynapses;
	std::vector<std::shared_ptr<SpikeTrain>> mSpikeTrains;
};

#endif
