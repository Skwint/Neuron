#ifndef TEST_AUTOMATON_H
#define TEST_AUTOMATON_H

#include "Test.h"

#include "NeuronSim/Automaton.h"

class TestAutomaton : public Test, public Automaton::Listener
{
public:
	TestAutomaton();
	~TestAutomaton();

	std::string name() { return "Automaton"; }
	void run();

private:
	void testTypeChangeCallback();
	void testSizeChangeCallback();
	void testLayerCreatedRemovedCallbacks();
	void testSynapseCreatedRemovedCallbacks();
	void testAutoSynapseRemoval();
	void testLayerResize();

	void resetChanges();
	void checkNothingChanged();

private: // from Automaton::Listener
	void automatonTypeChanged() override;
	void automatonSizeChanged(int width, int height) override;
	void automatonLayerCreated(std::shared_ptr<Layer> layer) override;
	void automatonLayerRemoved(std::shared_ptr<Layer> layer) override;
	void automatonSynapsesCreated(std::shared_ptr<SynapseMatrix> synapses) override;
	void automatonSynapsesRemoved(std::shared_ptr<SynapseMatrix> synapses) override;

private:
	std::unique_ptr<Automaton> mAutomaton;
	std::shared_ptr<Layer> mLayer1;
	std::shared_ptr<Layer> mLayer2;
	std::shared_ptr<SynapseMatrix> mSynapses1;
	std::shared_ptr<SynapseMatrix> mSynapses2;
	bool mTypeChanged;
	int mWidthChanged;
	int mHeightChanged;
	std::shared_ptr<Layer> mLayerChanged;
	std::shared_ptr<SynapseMatrix> mSynapsesChanged;
};

#endif
