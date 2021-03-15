#include "ResponseGraph.h"

#include <memory>
#include <qchart.h>
#include <qlineseries.h>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/LayerFactory.h"
#include "NeuronSim/SynapseMatrix.h"

using namespace std;

static const int DATA_TIME(200);
static const int RELAX_TIME(20);
static const int TOTAL_TIME(DATA_TIME + RELAX_TIME);

ResponseGraph::ResponseGraph(std::shared_ptr<Automaton> automaton, std::shared_ptr<Layer> layer, QWidget *parent)
	: QDialog(parent),
	mAutomaton(automaton),
	mLayer(layer),
	mSynapse(this, 1, 1)
{
	ui.setupUi(this);
	mData.resize(TOTAL_TIME);
	mSeries = new QLineSeries();
	mChart = new QChart();
	mChart->legend()->hide();

	connect(ui.btnUpdate, &QPushButton::clicked, this, &ResponseGraph::plot);
}

ResponseGraph::~ResponseGraph()
{
}

void ResponseGraph::fire(const Spike & spike, int index, float weight, int delay)
{
	for (int pos = 0, tt = mDataPoint + delay; pos < spike.duration() && tt < TOTAL_TIME; ++tt, ++pos)
	{
		mData[tt] += spike.potential(pos);
	}
}

void ResponseGraph::synapseMatrixChanged(SynapseMatrix * matrix)
{
	// We ignore this - we know exactly what we are doing with it.
}

void ResponseGraph::plot()
{
	fill(mData.begin(), mData.end(), 0.0f);
	float weight = ui.spinWeight->value();
	int duration = ui.spinDuration->value();
	auto original = mLayer.lock();
	if (original)
	{
		auto layer = mAutomaton->layerFactory()->create(original->typeName(), 1, 1);
		layer->setConfig(original->getConfig());

		mDataPoint = 0;
		while (mDataPoint < RELAX_TIME)
		{
			layer->tick();
			layer->fireSpikes(&mSynapse, this);
			++mDataPoint;
		}
		for (int spike = 0; spike < duration && mDataPoint < TOTAL_TIME; ++spike)
		{
			layer->inject(0, 0, weight);
			layer->tick();
			layer->fireSpikes(&mSynapse, this);
			++mDataPoint;
		}
		while (mDataPoint < TOTAL_TIME)
		{
			layer->tick();
			layer->fireSpikes(&mSynapse, this);
			++mDataPoint;
		}

		mSeries->clear();
		for (int tt = 0; tt < TOTAL_TIME; ++tt)
		{
			mSeries->append(qreal(tt), mData[tt]);
		}
		mChart->addSeries(mSeries);
		mChart->createDefaultAxes();
		ui.graphicsView->setChart(mChart);
	}
}
