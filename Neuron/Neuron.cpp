#include "Neuron.h"

#include <qtimer.h>

#include "NeuronSim/Life.h"
#include "ToolBox.h"

Neuron::Neuron(QWidget *parent)
	: QMainWindow(parent),
	mZoom(1),
	mLeft(0),
	mTop(0)
{
	ui.setupUi(this);

	mNet = std::make_unique<Life>();
	mToolBox = std::make_unique<ToolBox>();
	QMainWindow::addDockWidget(Qt::RightDockWidgetArea, mToolBox.get());
	connect(mToolBox.get(), &ToolBox::netBuild, this, &Neuron::buildNet);
	connect(mToolBox->zoomOneToOne(), &QToolButton::clicked, this, &Neuron::zoomOneToOne);
	connect(mToolBox->zoomFitToWindow(), &QToolButton::clicked, this, &Neuron::zoomFitToWindow);
	connect(mToolBox->zoomIn(), &QToolButton::clicked, this, &Neuron::zoomIn);
	connect(mToolBox->zoomOut(), &QToolButton::clicked, this, &Neuron::zoomOut);
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
	auto image = mNet->image(ui.view->width(), ui.view->height(), mLeft, mTop);
	ui.view->updateTexture(image);
	ui.view->update();
}

/**
Build a net of cells with a given width and height, and arrange the view of it to fit the available space while maintaining aspect ratio
*/
void Neuron::buildNet(int width, int height)
{
	mNet->resize(width, height);
	auto image = mNet->image(ui.view->width(), ui.view->height(), mLeft, mTop);
	ui.view->updateTexture(image);
}

void Neuron::zoomFitToWindow()
{
	int winWidth = ui.view->width();
	int winHeight = ui.view->height();
	int netWidth = mNet->width();
	int netHeight = mNet->height();
	float winAspect = float(winWidth) / float(winHeight);
	float netAspect = float(netWidth) / float(netHeight);
	float fzoom;
	if (winAspect > netAspect)
	{
		fzoom = winHeight / netHeight;
	}
	else
	{
		fzoom = winWidth / netWidth;
	}
	setZoom(int(fzoom));
	centerNet();
}

void Neuron::zoomOneToOne()
{
	setZoom(1);
}

void Neuron::zoomIn()
{
	setZoom(mZoom * 2);
}

void Neuron::zoomOut()
{
	setZoom(mZoom / 2);
}

void Neuron::setZoom(int zoom)
{
	mZoom = std::max(1, zoom);
	ui.view->setZoom(float(mZoom));
	mToolBox->displayZoom(mZoom);
}

void Neuron::centerNet()
{
	int winWidth = ui.view->width();
	int winHeight = ui.view->height();
	int netWidth = mNet->width();
	int netHeight = mNet->height();
	mTop = (netHeight - winHeight) / 2;
	mLeft = (netWidth - winWidth) / 2;
}
