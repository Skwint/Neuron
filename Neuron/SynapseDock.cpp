#include "SynapseDock.h"

#include <qpushbutton.h>

#include "NeuronSim/Layer.h"
#include "SynapseConfig.h"

using namespace std;

SynapseDock::SynapseDock(shared_ptr<Automaton> automaton, QWidget *parent)
	: QDockWidget(parent),
	mAutomaton(automaton)
{
	ui.setupUi(this);
	mAutomaton->addListener(this);

	connect(ui.btnNew, &QPushButton::clicked, this, &SynapseDock::create);
}

SynapseDock::~SynapseDock()
{
	mAutomaton->removeListener(this);
}

void SynapseDock::create()
{
	mAutomaton->createSynapse();
}

void SynapseDock::automatonLayerCreated(shared_ptr<Layer> layer)
{
	for (auto synapse : mSynapseWidgets)
	{
		synapse->addLayer(layer->name());
	}
}

void SynapseDock::automatonLayerRemoved(shared_ptr<Layer> layer)
{
	// Remove the layer from the list of available sources and targets
	for (auto synapse : mSynapseWidgets)
	{
		synapse->removeLayer(layer->name());
	}
}

void SynapseDock::automatonSynapsesCreated(shared_ptr<SynapseMatrix> synapses)
{
	auto widget = make_shared<SynapseConfig>(mAutomaton, synapses);
	mSynapseWidgets.push_back(widget);
	ui.synapseLayout->insertWidget(0, widget.get());

	// Existing layers need adding - we won't get events for ones that already exist
	auto layers = mAutomaton->layers();
	for (auto layer : layers)
	{
		widget->addLayer(layer->name());
	}
}

void SynapseDock::automatonSynapsesRemoved(shared_ptr<SynapseMatrix> synapses)
{
	mSynapseWidgets.erase(remove_if(mSynapseWidgets.begin(), mSynapseWidgets.end(), [synapses](auto iter) { return iter->synapses() == synapses; }), mSynapseWidgets.end());
}
	
