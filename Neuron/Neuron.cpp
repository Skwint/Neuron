#include "Neuron.h"

#include <qtimer.h>
#include <qdir.h>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/Life.h"
#include "NeuronSim/Log.h"
#include "SynapseDock.h"
#include "ToolBox.h"

Neuron::Neuron(QWidget *parent)
	: QMainWindow(parent),
	mZoom(1),
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
	connect(mToolBox->paint(), &QComboBox::currentTextChanged, [this]() { ui.view->setPaint(View::Paint(mToolBox->paint()->currentIndex())); });

	connect(mToolBox->speed(), &QComboBox::currentTextChanged, this, &Neuron::simSpeed);
	connect(mToolBox->pause(), &QPushButton::clicked, this, &Neuron::simPause);
	connect(mToolBox->play(), &QPushButton::clicked, this, &Neuron::simPlay);
	connect(mToolBox->step(), &QPushButton::clicked, this, &Neuron::simStep);
	connect(mToolBox.get(), &ToolBox::redraw, this, &Neuron::redraw);
}

Neuron::~Neuron()
{
	mAutomaton->removeListener(this);
}

// We do not need to override this. However, when debugging it is often convenient
// to automatically initialize things on startup and this is the place to do it.
// It is also convenient when procedurally generation a large synapse structure is
// easier than using the GUI.
void Neuron::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
}

// TODO : this doesn't work when we are paused because we draw based on inputs but
// we are changing the spikes, and those don't happen till we step
// Maybe we should be drawing the spikes instead? That would require spikes to
// know their coordinates, which they don't at the moment.
void Neuron::redraw()
{
	ui.view->updateTextures();
	ui.view->update();
}

static const qint64 stepPerTimerUpdate(20);
void Neuron::step()
{
	mTimerTicked = false;
	auto rendering = mToolBox->rendering();
	mWaitingForSwap = (rendering == ToolBox::RENDER_ALL_FRAMES);
	mAutomaton->tick();
	if (rendering != ToolBox::RENDER_NEVER)
	{
		redraw();
	}

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
