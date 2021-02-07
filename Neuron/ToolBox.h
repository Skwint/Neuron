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
	enum Rendering
	{
		RENDER_ALL_FRAMES,
		RENDER_WHEN_READY,
		RENDER_NEVER
	};
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
	Rendering rendering() { return mRendering; }
	int delay();
	const SpikeProcessor::Spike & spike();

signals:
	void redraw();

private:
	void netToggle();
	void viewToggle();
	void simToggle();
	void editingToggle();

	void netTypeChanged();
	void netSizeChanged();
	void renderingChanged();
	void spikeChanged();
	void editingClearLayer();
	void editingClearAll();
	void editingNoise(int density);
	void editingSpike();
	void loadSpikeMaps();

private:// from Automaton::Listener
	void automatonTypeChanged();
	void automatonSizeChanged(int width, int height);
	void automatonLayerCreated(std::shared_ptr<Layer> layer);
	void automatonLayerRemoved(std::shared_ptr<Layer> layer);

private:
	Ui::ToolBox ui;
	std::shared_ptr<Automaton> mAutomaton;
	Rendering mRendering;
	QDir mSpikeDir;
};
