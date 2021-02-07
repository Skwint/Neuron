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
	void zoomIn();
	void zoomOut();
	void setZoom(int zoom);
	void simSpeed();
	void simPause();
	void simPlay();
	void simStep();
	void startTimer(int delay);
	void onFrameSwapped();
	void viewPress(QMouseEvent * ev);
	void viewMove(QMouseEvent * ev);
	void viewRelease(QMouseEvent * ev);
	void redraw();

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
	bool mTimerTicked;
	bool mWaitingForSwap;
	QElapsedTimer mFpsTimer;
	int mFpsFrameCounter;
};
