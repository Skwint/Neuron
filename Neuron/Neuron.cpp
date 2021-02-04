#include "Neuron.h"

#include <qtimer.h>
#include <qdir.h>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/Life.h"
#include "NeuronSim/Log.h"
#include "ToolBox.h"
#include "SynapseDock.h"

Neuron::Neuron(QWidget *parent)
	: QMainWindow(parent),
	mZoom(1),
	mLeft(0),
	mTop(0),
	mTimerTicked(false),
	mWaitingForSwap(false),
	mFpsFrameCounter(0)
{
	ui.setupUi(this);

	Log::to("Neuron.log");
	LOG("Neuron startup");
	LOG("CWD => " << QDir::currentPath().toStdString());

	mAutomaton = std::make_shared<Automaton>();
	mAutomaton->addListener(this);
	ui.view->setAutomaton(mAutomaton);
	mToolBox = std::make_unique<ToolBox>(mAutomaton);
	QMainWindow::addDockWidget(Qt::LeftDockWidgetArea, mToolBox.get());
	mLayerDock = std::make_unique<LayerDock>(mAutomaton);
	QMainWindow::addDockWidget(Qt::RightDockWidgetArea, mLayerDock.get());
	mSynapseDock = std::make_unique<SynapseDock>(mAutomaton);
	QMainWindow::addDockWidget(Qt::RightDockWidgetArea, mSynapseDock.get());

	mTimer = std::make_unique<QTimer>(this);
	connect(mTimer.get(), &QTimer::timeout, this, QOverload<>::of(&Neuron::tick));
	connect(ui.view, &QOpenGLWidget::frameSwapped, this, &Neuron::onFrameSwapped);

	connect(mToolBox->zoomFitToWindow(), &QToolButton::clicked, this, &Neuron::zoomFitToWindow);
	connect(mToolBox->zoomIn(), &QToolButton::clicked, this, &Neuron::zoomIn);
	connect(mToolBox->zoomOut(), &QToolButton::clicked, this, &Neuron::zoomOut);
	connect(mToolBox->style(), &QComboBox::currentTextChanged, [this]() { ui.view->setStyle(mToolBox->style()->currentText()); });

	connect(mToolBox->speed(), &QComboBox::currentTextChanged, this, &Neuron::simSpeed);
	connect(mToolBox->pause(), &QPushButton::clicked, this, &Neuron::simPause);
	connect(mToolBox->play(), &QPushButton::clicked, this, &Neuron::simPlay);
	connect(mToolBox->step(), &QPushButton::clicked, this, &Neuron::simStep);
}

Neuron::~Neuron()
{
	mAutomaton->removeListener(this);
}

// We wait until the show event to do these initialisations because otherwise the opengl
// view widget gets upset.
void Neuron::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	// temporary testing hack
	//auto l1 = mAutomaton->createLayer();
	//auto l2 = mAutomaton->createLayer();
	//static LayerData d2;
	//d2.color = QColor(0xFF00FF00);
	//l2->setUserData(&d2);
	//auto s1 = mAutomaton->createSynapse();
	//auto s2 = mAutomaton->createSynapse();
	//s2->setSource(l2);
	//s2->setTarget(l2);
}

static const qint64 stepPerTimerUpdate(20);
void Neuron::step()
{
	mTimerTicked = false;
	mWaitingForSwap = mToolBox->showAllFrames();
	mAutomaton->tick();
	ui.view->updateTextures();
	ui.view->update();

	++mFpsFrameCounter;
	if (mFpsFrameCounter >= stepPerTimerUpdate)
	{
		auto elapsed = mFpsTimer.elapsed();
		mToolBox->displayFrameTime(elapsed / stepPerTimerUpdate);
		mFpsTimer.restart();
		mFpsFrameCounter = 0;
	}
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

void Neuron::zoomOneToOne()
{
	int winWidth = ui.view->width();
	int winHeight = ui.view->height();
	int netWidth = mAutomaton->width();
	int netHeight = mAutomaton->height();
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

void Neuron::zoomFitToWindow()
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
	int netWidth = mAutomaton->width();
	int netHeight = mAutomaton->height();
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
		mFpsTimer.start();
		mFpsFrameCounter = 0;
		mToolBox->displayFrameTime(0);
	}
}

void Neuron::simStep()
{
	if (!mTimer->isActive())
	{
		mFpsFrameCounter = 0;
		tick();
	}
}

