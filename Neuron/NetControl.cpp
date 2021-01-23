#include "NetControl.h"

NetControl::NetControl(Listener * listener, QWidget *parent)
	: QDockWidget(parent),
	mListener(listener)
{
	ui.setupUi(this);

	connect(ui.btnApply, &QPushButton::clicked, this, &NetControl::apply);
}

NetControl::~NetControl()
{
}

void NetControl::apply()
{
	int width = ui.spinWidth->value();
	int height = ui.spinHeight->value();
	mListener->buildNet(width, height);
}
