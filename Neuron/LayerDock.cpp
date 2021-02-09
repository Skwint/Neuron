#include "LayerDock.h"

#include <memory>

#include "NeuronSim/Layer.h"

using namespace std;

LayerDock::LayerDock(std::shared_ptr<Automaton> automaton, QWidget *parent)
	: QDockWidget(parent),
	mAutomaton(automaton)
{
	ui.setupUi(this);

	mAutomaton->addListener(this);
	connect(ui.btnNewLayer, &QPushButton::clicked, this, [this]() { mAutomaton->createLayer(); });
}

LayerDock::~LayerDock()
{
	mAutomaton->removeListener(this);
}

void LayerDock::automatonTypeChanged()
{
	for (auto widget : mLayerWidgets)
	{
		widget->repopulate();
	}
}

void LayerDock::automatonLayerCreated(shared_ptr<Layer> layer)
{
	auto widget = make_shared<LayerConfig>(mAutomaton, layer);
	mLayerWidgets.push_back(widget);
	ui.configLayout->insertWidget(0, widget.get());
}

void LayerDock::automatonLayerRemoved(shared_ptr<Layer> layer)
{
	mLayerWidgets.erase(remove_if(mLayerWidgets.begin(), mLayerWidgets.end(), [layer](auto iter) { return iter->layerName() == layer->name(); }), mLayerWidgets.end());
}
