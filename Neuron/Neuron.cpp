#include "Neuron.h"

#include <qtimer.h>

#include "NeuronSim/Life.h"
#include "ToolBox.h"

Neuron::Neuron(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	mNet = std::make_unique<Life>();
	mToolBox = std::make_unique<ToolBox>();
	QMainWindow::addDockWidget(Qt::RightDockWidgetArea, mToolBox.get());
	connect(mToolBox.get(), &ToolBox::netBuild, this, &Neuron::buildNet);
}

// We wait until the show event to do these initialisations because otherwise the opengl
// view widget gets upset.
void Neuron::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	buildNet(512, 512);

	mTimer = std::make_unique<QTimer>(this);
	connect(mTimer.get(), &QTimer::timeout, this, QOverload<>::of(&Neuron::tick));
	mTimer->start(100);
}

void Neuron::tick()
{
	mNet->tick();
	ui.view->updateTexture(mNet->image());
	ui.view->update();
}

/**
Build a net of cells with a given width and height, and arrange the view of it to fit the available space while maintaining aspect ratio
*/
void Neuron::buildNet(int width, int height)
{
	mNet->resize(width, height);
	ui.view->resizeTexture(width, height);
	ui.view->updateTexture(mNet->image());
}

