#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Neuron.h"
#include <qtimer.h>

#include "NeuronSim/Net.h"
#include "ToolBox.h"

class Neuron : public QMainWindow
{
	Q_OBJECT

public:
	Neuron(QWidget *parent = Q_NULLPTR);

	void step();

private:
	void tick();
	void buildNet(int width, int height);
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
	std::unique_ptr<QTimer> mTimer;
	std::unique_ptr<Net> mNet;
	std::unique_ptr<ToolBox> mToolBox;
	int mZoom;
	int mLeft;
	int mTop;
	bool mTimerTicked;
	bool mWaitingForSwap;
};
