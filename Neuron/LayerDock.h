#pragma once

#include <QDockWidget>
#include "ui_LayerDock.h"

#include "NeuronSim/ConfigSet.h"
#include "NeuronSim/LayerFactory.h"
#include "LayerConfig.h"

class Automaton;

class LayerDock : public QDockWidget, public Automaton::Listener
{
	Q_OBJECT

public:
	LayerDock(std::shared_ptr<Automaton> automaton, QWidget *parent = Q_NULLPTR);
	~LayerDock();

	void apply();

private: // from Automaton::Listener
	void automatonTypeChanged();
	void automatonLayerCreated(std::shared_ptr<Layer> layer);
	void automatonLayerRemoved(std::shared_ptr<Layer> layer);

private:
	Ui::LayerDock ui;
	std::shared_ptr<Automaton> mAutomaton;
	std::vector<std::shared_ptr<LayerConfig> > mLayerWidgets;
	ConfigSet mConfig;
};
