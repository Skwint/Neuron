#pragma once

#include <QGroupBox>
#include "ui_LayerConfig.h"

#include "NeuronSim/Automaton.h"
#include "NeuronSim/ConfigPresets.h"

class LayerConfig : public QGroupBox
{
	Q_OBJECT
private:
	struct ConfigWidget
	{
		ConfigWidget(const std::string & name, QWidget * widget) :
			name(name),
			widget(widget)
		{}
		std::string name;
		QWidget * widget;
	};
public:
	LayerConfig(std::shared_ptr<Automaton> mAutomaton, std::shared_ptr<Layer> layer, QWidget *parent = Q_NULLPTR);
	~LayerConfig();

	void repopulate();
	const std::string & layerName() { return mLayerName; }

private:
	void color();
	void presetSelected();
	void loadPresets();
	void configItemChanged();

private:
	Ui::LayerConfig ui;
	std::shared_ptr<Automaton> mAutomaton;
	std::string mLayerName;
	ConfigSet mConfig;
	std::vector<ConfigWidget> mConfigWidgets;
	bool mLoadingPreset;
};
