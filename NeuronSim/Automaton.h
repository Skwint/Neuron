#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <memory>
#include <vector>

#include "ConfigSet.h"
#include "SynapseMatrix.h"

class Layer;
class LayerFactory;
class SpikeTrain;

// An automaton is a collection of Layer objects connected by SynapseMatrix objects.
// It also maintains SpikeTrain objects to handle the spikes in transit from one
// layer to another.
// The expected usage is that an application will create one automaton instance and
// use it for everything, but it is not restricted in that way.
class Automaton : public SynapseMatrix::Listener
{
public:
	// The operating mode of the entire automaton.
	enum OperatingMode
	{
		MODE_NORMAL,   //< Normal mode
		MODE_DEPRESSED //< Depressed mode - no spikes are fired while depressed
	};
	// Observer interface for an automaton. Useful for external code that needs
	// to track the lifespan of layer and synapse components.
	class Listener
	{
	public:
		// Callback for the type of the automaton changing
		virtual void automatonTypeChanged() {};
		// Callback for the size of the automaton changing
		virtual void automatonSizeChanged(int width, int height) {};
		// Callback for a new layer being added to the automaton
		virtual void automatonLayerCreated(std::shared_ptr<Layer> layer) {};
		// Callback for a layer being removed from the automaton
		virtual void automatonLayerRemoved(std::shared_ptr<Layer> layer) {};
		// Callback for a new synapse matrix being added to the automaton
		virtual void automatonSynapsesCreated(std::shared_ptr<SynapseMatrix> synapses) {};
		// Callback for a synapse matrix being removed from the automaton
		virtual void automatonSynapsesRemoved(std::shared_ptr<SynapseMatrix> synapses) {};
	};
private:
	// The lock is to detect (and throw exceptions when) a user of
	// this class attempts to add or remove a listener while the listeners
	// are being iterated.
	// TODO - fix it so that it remembers dead listeners and removes them later
	// so that we don't need a lock at all.
	class Lock
	{
	public:
		inline Lock();
		inline ~Lock();
	private:
		static bool mLocked;
	};
public:
	// Default constructor
	Automaton();
	// Destructor
	~Automaton();

	// Reset the automaton. All layers and synapses are removed.
	void reset();
	// Set the network type. This will result in a reset.
	void setNetworkType(const std::string & type);
	// Get the network type.
	const std::string & networkType() const { return mType; }
	// Set the operating mode
	void setOperatingMode(OperatingMode mode) { mMode = mode; }
	// Set the width and height of the automaton
	void setSize(int width, int height);
	// Get the width of the automaton
	int width() { return mWidth; }
	// Get the height of the automaton
	int height() { return mHeight; }
	// Create a layer. The layer will have the type specified in
	// a preceding call to setNetworkType and a size that matches
	// the automaton width and height.
	std::shared_ptr<Layer> createLayer();
	// Remove a layer. All synapses connected to this layer as
	// either a source or target will also be removed.
	void removeLayer(std::shared_ptr<Layer> layer);
	// Remove a layer. All synapses connected to this layer as
	// either a source or target will also be removed.
	void removeLayer(const std::string & name);
	// Returns the set of all layers in this automaton.
	const std::vector<std::shared_ptr<Layer> > layers() { return mLayers; }
	// Find a layer by name. Returns an empty pointer if no layer
	// with the give name could be found.
	std::shared_ptr<Layer> findLayer(const std::string & name);
	// Return a list of types of network that are available.
	std::vector<std::string> typeNames();
	// Creates a new synapse matrix. It will default to a 1x1 matrix
	// with a weight of 1.0 and no layer connections.
	std::shared_ptr<SynapseMatrix> createSynapse();
	// Remove a synapse.
	void removeSynapse(std::shared_ptr<SynapseMatrix> synapses);
	// Return a list of all synapses in this automaton.
	const std::vector<std::shared_ptr<SynapseMatrix>> synapses() { return mSynapses; }
	// Add a listener. The listener will be informed of changes to the automaton.
	// The caller is responsible for calling removeListener
	// before being destroyed.
	void addListener(Listener * listener);
	// Remove a listener.
	void removeListener(Listener * listener);
	// Tick this automaton, which moves every layer withing it one iteration forwards,
	// and processes 1 time step of spikes.
	void tick();
	// Reset the state of all neurons and remove all active spikes.
	// There is no guarantee that this will make the automaton go quiet, since some
	// neurons could be self activating from their reset state.
	void clearLayers();
	// Save the current state of all layers and synapses to the given path
	void save(const std::filesystem::path & path);
	// Load a previously saved state from a given path
	void load(const std::filesystem::path & path);
	// Returns a layer factory that can be used to obtain information
	// about the layer types and configurations available.
	LayerFactory * layerFactory() { return mLayerFactory.get(); }
	// Returns the average number of spikes per neuron this time step.
	float currentSpikeDensity();

private: // From SynapseMatrix::Listener
	void synapseMatrixChanged(SynapseMatrix * matrix) override;

private:
	// Called after changes to the layers or synapses that might mean
	// new connections between layers need handling
	void recalculateSpikeTrains();
	// Create a layer without inserting it into the automaton.
	std::shared_ptr<Layer> createDetachedLayer();
	// Attach an existing layer to the automaton.
	void attachLayer(std::shared_ptr<Layer> layer);
	// Create a synapse matrix without inserting it into the automaton.
	std::shared_ptr<SynapseMatrix> createDetachedSynapses();
	// Attach an existing synapse matrix to the automaton.
	void attachSynapses(std::shared_ptr<SynapseMatrix> synapses);
	// Threaded implementation detail of Tick()
	void tickTargetLayer(Layer * target);
	// Threaded implementation detail of Tick()
	void tickSourceLayer(Layer * source);

private:
	// All listeners to this automaton
	std::vector<Listener *> mListeners;
	// The type of neuron we are using
	std::string mType;
	// The current operating mode
	OperatingMode mMode;
	// The width, in neurons, of the automaton
	int mWidth;
	// The height, in neurons, of the automaton
	int mHeight;
	// The layer factory instance we use for creating typed layers
	std::unique_ptr<LayerFactory> mLayerFactory;
	// The currently active layers
	std::vector<std::shared_ptr<Layer> > mLayers;
	// The currently active synapse matrices.
	// Each matrix is associated with both a source and target layer.
	std::vector<std::shared_ptr<SynapseMatrix> > mSynapses;
	// The spike trains for spikes passing along synapses. These are
	// data storage objects rather than logic processing blocks.
	std::vector<std::shared_ptr<SpikeTrain>> mSpikeTrains;
};

#endif
