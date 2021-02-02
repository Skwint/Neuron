#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Neuron.h"
#include <qtimer.h>
#include <qelapsedtimer.h>

#include "NeuronSim/Net.h"
#include "ToolBox.h"

class Layer;
class LayerFactory;

class Neuron : public QMainWindow
{
	Q_OBJECT

public:
	Neuron(QWidget *parent = Q_NULLPTR);

	void step();

private:
	void tick();
	void buildNet(const std::string & type, const ConfigSet & config, int width, int height);
	void zoomFitToWindow();
	void zoomOneToOne();
	void zoomIn();
	void zoomOut();
	void centerNet();
	void setZoom(int zoom);
	void simSpeed();
	void simPause();
	void simPlay();
	void simStep();
	void startTimer(int delay);
	void onFrameSwapped();
	void setSynapses(const SynapseMatrix & synapses);
	void setSpike(const SpikeProcessor::Spike & spike);

private:
	void showEvent(QShowEvent *event);

private:
	Ui::NeuronClass ui;
	std::shared_ptr<LayerFactory> mLayerFactory;
	std::unique_ptr<QTimer> mTimer;
	std::shared_ptr<Layer> mNet;
	std::unique_ptr<ToolBox> mToolBox;
	int mZoom;
	int mLeft;
	int mTop;
	bool mTimerTicked;
	bool mWaitingForSwap;
	std::vector<uint32_t> mImage;
	QElapsedTimer mFpsTimer;
	int mFpsFrameCounter;
};
