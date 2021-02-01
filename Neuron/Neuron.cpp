#include "Neuron.h"

#include <qtimer.h>
#include <qdir.h>

#include "NeuronSim/Layer.h"
#include "NeuronSim/LayerFactory.h"
#include "NeuronSim/Life.h"
#include "NeuronSim/Log.h"
#include "ToolBox.h"

Neuron::Neuron(QWidget *parent)
	: QMainWindow(parent),
	mZoom(1),
	mLeft(0),
	mTop(0),
	mTimerTicked(false),
	mWaitingForSwap(false)
{
	ui.setupUi(this);

	Log::to("Neuron.log");
	LOG("Neuron startup");
	LOG("CWD => " << QDir::currentPath().toStdString());

	mLayerFactory = std::make_shared<LayerFactory>();
	mToolBox = std::make_unique<ToolBox>(mLayerFactory);
	QMainWindow::addDockWidget(Qt::RightDockWidgetArea, mToolBox.get());
	mTimer = std::make_unique<QTimer>(this);
	connect(mTimer.get(), &QTimer::timeout, this, QOverload<>::of(&Neuron::tick));
	connect(ui.view, &QOpenGLWidget::frameSwapped, this, &Neuron::onFrameSwapped);

	connect(mToolBox.get(), &ToolBox::netBuild, this, &Neuron::buildNet);
	connect(mToolBox.get(), &ToolBox::setSynapses, this, &Neuron::setSynapses);
	connect(mToolBox->zoomOneToOne(), &QToolButton::clicked, this, &Neuron::zoomOneToOne);
	connect(mToolBox->zoomFitToWindow(), &QToolButton::clicked, this, &Neuron::zoomFitToWindow);
	connect(mToolBox->zoomIn(), &QToolButton::clicked, this, &Neuron::zoomIn);
	connect(mToolBox->zoomOut(), &QToolButton::clicked, this, &Neuron::zoomOut);
	connect(mToolBox->style(), &QComboBox::currentTextChanged, [this]() { ui.view->setStyle(mToolBox->style()->currentText()); });

	connect(mToolBox->speed(), &QComboBox::currentTextChanged, this, &Neuron::simSpeed);
	connect(mToolBox->pause(), &QPushButton::clicked, this, &Neuron::simPause);
	connect(mToolBox->play(), &QPushButton::clicked, this, &Neuron::simPlay);
	connect(mToolBox->step(), &QPushButton::clicked, this, &Neuron::simStep);
}

// We wait until the show event to do these initialisations because otherwise the opengl
// view widget gets upset.
void Neuron::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	buildNet(Life::name(), Life::defaultConfig(), 512, 512);
}

void Neuron::step()
{
	mTimerTicked = false;
	mWaitingForSwap = mToolBox->showAllFrames();
	mNet->tick();
	mNet->paint(&mImage[0]);
	ui.view->updateTexture(&mImage[0]);
	ui.view->update();
}

void Neuron::tick()
{
	if (!mWaitingForSwap)
	{
		step();
	}
	else
	{
		mTimerTicked = true;
	}
}

void Neuron::onFrameSwapped()
{
	if (mTimerTicked)
	{
		step();
	}
	else
	{
		mWaitingForSwap = false;
	}
}

/**
Build a net of cells with a given width and height, and arrange the view of it to fit the available space while maintaining aspect ratio
*/
void Neuron::buildNet(const std::string & type, const ConfigSet & config, int width, int height)
{
	if (mNet && mNet->typeName() == type)
	{
		mNet->resize(width, height);
	}
	else
	{
		mNet = mLayerFactory->create(type, width, height);
	}
	mNet->setConfig(config);
	mNet->setSynapses(mToolBox->synapses());
	ui.view->resizeTexture(width, height);
	mImage.resize(width * height);
	centerNet();
	mNet->paint(&mImage[0]);
	ui.view->updateTexture(&mImage[0]);
}

void Neuron::setSynapses(const SynapseMatrix & synapses)
{
	mNet->setSynapses(synapses);
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
		fzoom = float(winHeight) / netHeight;
	}
	else
	{
		fzoom = float(winWidth) / netWidth;
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
	update();
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

void Neuron::startTimer(int delay)
{
	if (mTimer->isActive())
	{
		mTimer->stop();
	}
	mWaitingForSwap = false;
	mTimer->start(delay);
}

void Neuron::simSpeed()
{
	if (mTimer->isActive())
	{
		startTimer(mToolBox->delay());
	}
}

void Neuron::simPause()
{
	if (mTimer->isActive())
	{
		mTimer->stop();
	}
}

void Neuron::simPlay()
{
	if (!mTimer->isActive())
	{
		startTimer(mToolBox->delay());
	}
}

void Neuron::simStep()
{
	if (!mTimer->isActive())
	{
		tick();
	}
}

