#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Neuron.h"
#include <qtimer.h>
#include <qelapsedtimer.h>

#include "NeuronSim/Net.h"
#include "ToolBox.h"
#include "LayerDock.h"

class Automaton;
class Layer;
class SynapseDock;

class Neuron : public QMainWindow, public Automaton::Listener
{
	Q_OBJECT

public:
	Neuron(QWidget *parent = Q_NULLPTR);
	~Neuron();

	void step();

private:
	void tick();
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

private:
	void showEvent(QShowEvent *event);

private:
	Ui::NeuronClass ui;
	std::shared_ptr<Automaton> mAutomaton;
	std::unique_ptr<QTimer> mTimer;
	std::unique_ptr<ToolBox> mToolBox;
	std::unique_ptr<LayerDock> mLayerDock;
	std::unique_ptr<SynapseDock> mSynapseDock;
	int mZoom;
	int mLeft;
	int mTop;
	bool mTimerTicked;
	bool mWaitingForSwap;
	QElapsedTimer mFpsTimer;
	int mFpsFrameCounter;
};
