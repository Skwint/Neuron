#pragma once

#include <QDockWidget>
#include <QDir>
#include "ui_ToolBox.h"

#include "NeuronSim/ConfigItem.h"
#include "NeuronSim/LayerFactory.h"
#include "NeuronSim/SynapseMatrix.h"

class ToolBox : public QDockWidget
{
	Q_OBJECT

public:
	ToolBox(std::shared_ptr<LayerFactory> layerFactory, QWidget *parent = Q_NULLPTR);
	~ToolBox();

	auto zoomFitToWindow() { return ui.btnViewZoomFitToWindow; }
	auto zoomOneToOne() { return ui.btnViewZoomOneToOne; }
	auto zoomIn() { return ui.btnViewZoomIn; }
	auto zoomOut() { return ui.btnViewZoomOut; }
	auto speed() { return ui.cmbSimSpeed; }
	auto play() { return ui.btnSimPlay; }
	auto pause() { return ui.btnSimPause; }
	auto step() { return ui.btnSimStep; }
	auto style() { return ui.cmbViewStyle; }

	void displayZoom(int zoom);
	bool showAllFrames() { return ui.chkShowAllFrames->isChecked(); }
	int delay();
	const SynapseMatrix & synapses() const { return mSynapseMatrix; }

signals:
	void netBuild(const std::string & type, const ConfigSet & config, int width, int height);
	void setSynapses(const SynapseMatrix & synapses);

private:
	void netToggle();
	void netApply();
	void netTypeChanged();
	void viewToggle();

	void simToggle();

	void synapseToggle();
	void synapseChanged();
	void populateSynapses();

private:
	Ui::ToolBox ui;
	std::shared_ptr<LayerFactory> mLayerFactory;
	std::vector<QWidget *> mConfigWidgets;
	ConfigSet mConfig;
	int mFixedConfigRows;
	QDir mSynapseDir;
	SynapseMatrix mSynapseMatrix;
};
