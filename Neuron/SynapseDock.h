#pragma once

#include <QDockWidget>
#include "ui_SynapseDock.h"

#include "NeuronSim/Automaton.h"

class SynapseConfig;
class SynapseMatrix;

class SynapseDock : public QDockWidget, public Automaton::Listener
{
	Q_OBJECT

public:
	SynapseDock(std::shared_ptr<Automaton> automaton, QWidget *parent = Q_NULLPTR);
	~SynapseDock();

	void create();
	void apply();

private: // from Automaton::Listener
	void automatonLayerCreated(std::shared_ptr<Layer> layer);
	void automatonLayerRemoved(std::shared_ptr<Layer> layer);
	void automatonSynapsesCreated(std::shared_ptr<SynapseMatrix> synapses);
	void automatonSynapsesRemoved(std::shared_ptr<SynapseMatrix> synapses);

private:
	Ui::SynapseDock ui;
	std::shared_ptr<Automaton> mAutomaton;
	std::vector<std::shared_ptr<SynapseConfig>> mSynapseWidgets;
};
