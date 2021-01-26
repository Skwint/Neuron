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

	void tick();

private:
	void buildNet(int width, int height);
	void zoomFitToWindow();
	void zoomOneToOne();
	void zoomIn();
	void zoomOut();
	void centerNet();
	void setZoom(int zoom);

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
};
