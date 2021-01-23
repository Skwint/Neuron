#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Neuron.h"
#include <qtimer.h>

#include "NeuronSim/Net.h"
#include "NetControl.h"

class Neuron : public QMainWindow, public NetControl::Listener
{
	Q_OBJECT

public:
	Neuron(QWidget *parent = Q_NULLPTR);

	void tick();

private: // from NetControl::Listener
	void buildNet(int width, int height);

private:
	void showEvent(QShowEvent *event);

private:
	Ui::NeuronClass ui;
	std::unique_ptr<QTimer> mTimer;
	std::unique_ptr<NetControl> mNetControl;
	std::unique_ptr<Net> mNet;
};
