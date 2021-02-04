#include "LayerDock.h"

LayerDock::LayerDock(std::shared_ptr<Automaton> automaton, QWidget *parent)
	: QDockWidget(parent),
	mAutomaton(automaton)
{
	ui.setupUi(this);

	mAutomaton->addListener(this);
	connect(ui.btnNewLayer, &QPushButton::clicked, this, [this]() { mAutomaton->createLayer(); });
	connect(ui.btnApply, &QPushButton::clicked, this, &LayerDock::apply);
}

LayerDock::~LayerDock()
{
	mAutomaton->removeListener(this);
}

void LayerDock::apply()
{
	for (auto widget : mLayerWidgets)
	{
		widget->apply();
	}
}

void LayerDock::automatonTypeChanged()
{
	for (auto widget : mLayerWidgets)
	{
		widget->repopulate();
	}
}

void LayerDock::automatonLayerCreated(std::shared_ptr<Layer> layer)
{
	auto widget = std::make_shared<LayerConfig>(layer);
	mLayerWidgets.push_back(widget);
	ui.configLayout->insertWidget(0, widget.get());
	connect(widget->deleteButton(), &QPushButton::clicked, this, [this, layer]() { mAutomaton->removeLayer(layer); });
}

void LayerDock::automatonLayerRemoved(std::shared_ptr<Layer> layer)
{
	mLayerWidgets.erase(std::remove_if(mLayerWidgets.begin(), mLayerWidgets.end(), [layer](auto iter) { return iter->layer() == layer; }), mLayerWidgets.end());
}
