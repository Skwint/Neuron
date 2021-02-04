#pragma once

#include <QDockWidget>
#include <QDir>
#include "ui_ToolBox.h"

#include "NeuronSim/Automaton.h"
#include "NeuronSim/LayerFactory.h"
#include "NeuronSim/SynapseMatrix.h"
#include "NeuronSim/SpikeProcessor.h"

class ToolBox : public QDockWidget, public Automaton::Listener
{
	Q_OBJECT
public:
	ToolBox(std::shared_ptr<Automaton> automaton, QWidget *parent = Q_NULLPTR);
	~ToolBox();

	auto zoomFitToWindow() { return ui.btnViewZoomFitToWindow; }
	auto zoomIn() { return ui.btnViewZoomIn; }
	auto zoomOut() { return ui.btnViewZoomOut; }
	auto speed() { return ui.cmbSimSpeed; }
	auto play() { return ui.btnSimPlay; }
	auto pause() { return ui.btnSimPause; }
	auto step() { return ui.btnSimStep; }
	auto style() { return ui.cmbViewStyle; }

	void displayZoom(int zoom);
	void displayFrameTime(qint64 frameTime);
	bool showAllFrames() { return ui.chkShowAllFrames->isChecked(); }
	int delay();
	const SpikeProcessor::Spike & spike();

private:
	void netToggle();
	void netTypeChanged();
	void netSizeChanged();
	void viewToggle();

	void simToggle();

	void spikeChanged();
	void populateSpikes();

private:// from Automaton::Listener
	void automatonTypeChanged();
	void automatonSizeChanged(int width, int height);

private:
	Ui::ToolBox ui;
	std::shared_ptr<Automaton> mAutomaton;
	std::map<std::string, SpikeProcessor::Spike> mSpikes;
};
