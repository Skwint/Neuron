#pragma once

#include <QDockWidget>
#include "ui_NetControl.h"

class NetControl : public QDockWidget
{
	Q_OBJECT
public:
	class Listener
	{
	public:
		virtual void buildNet(int width, int height) = 0;
	};
public:
	NetControl(Listener * listener, QWidget *parent = Q_NULLPTR);
	~NetControl();

private:
	void apply();

private:
	Ui::NetControl ui;
	Listener * mListener;
};
