#ifndef TEST_STABILITY_H
#define TEST_STABILITY_H

#include "Test.h"

#include <random>

#include "NeuronSim/Automaton.h"

// Although this test does verify some of the functionality it excercises the
// main purpose is to check that nothing crashes.
class TestStability : public Test, public Automaton::Listener
{
private:
	enum Action
	{
		CREATE_LAYER,
		REMOVE_LAYER,
		CREATE_SYNAPSES,
		REMOVE_SYNAPSES,
		RESIZE,
		STEP,

		ACTION_COUNT
	};
public:
	TestStability();
	~TestStability();

	std::string name() { return "Stability"; }
	void run();

private: // from Automaton::Listener
	void automatonTypeChanged() override;
	void automatonSizeChanged(int width, int height) override;
	void automatonLayerCreated(std::shared_ptr<Layer> layer) override;
	void automatonLayerRemoved(std::shared_ptr<Layer> layer) override;
	void automatonSynapsesCreated(std::shared_ptr<SynapseMatrix> synapses) override;
	void automatonSynapsesRemoved(std::shared_ptr<SynapseMatrix> synapses) override;

private:
	std::unique_ptr<Automaton> mAutomaton;
	std::vector<std::shared_ptr<Layer>> mLayers;
	std::vector<std::shared_ptr<SynapseMatrix>> mSynapses;
	int mWidth;
	int mHeight;
	std::mt19937 mRandom;
};

#endif
