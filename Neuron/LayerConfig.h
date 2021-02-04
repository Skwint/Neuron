#pragma once

#include <QGroupBox>
#include "ui_LayerConfig.h"

#include "NeuronSim/Automaton.h"
#include "NeuronSim/ConfigItem.h"
#include "LayerData.h"

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
	LayerConfig(std::shared_ptr<Layer> layer, QWidget *parent = Q_NULLPTR);
	~LayerConfig();

	void repopulate();
	void apply();
	QPushButton * deleteButton() { return ui.btnDelete; }
	std::shared_ptr<Layer> layer() { return mLayer; }

private:
	void color();

private:
	Ui::LayerConfig ui;
	std::shared_ptr<Layer> mLayer;
	std::unique_ptr<LayerData> mLayerData;
	ConfigSet mConfig;
	int mFixedConfigRows;
	std::vector<ConfigWidget> mConfigWidgets;
};
