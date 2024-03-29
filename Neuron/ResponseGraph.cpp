#include "ResponseGraph.h"

#include <algorithm>
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
	mInputSeries = new QLineSeries();
	mChart = new QChart();
	mInputChart = new QChart();
	mChart->legend()->hide();
	mInputChart->legend()->hide();

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

// Plot a graph of the spikes generated by a neuron of the current configuration
// when given a specific series of input spikes.
// This is achieved by creating a 1x1 layer of the neuron, injecting spikes
// into it, and intercepting the spikes it fires by implementing the Spiker
// interface.
void ResponseGraph::plot()
{
	int dutyOn = ui.spinDutyOn->value();
	int dutyOff = ui.spinDutyOff->value();
	int duty = dutyOn + dutyOff;
	float weight = ui.spinWeight->value();
	int duration = ui.spinDuration->value();
	auto original = mLayer.lock();
	float maxData = 0.0f;
	float minData = 0.0f;
	float maxInput = 0.0f;
	float minInput = 0.0f;
	if (original)
	{
		mInputSeries->clear();
		auto layer = mAutomaton->layerFactory()->create(original->typeName(), 1, 1);
		layer->setConfig(original->getConfig());
		fill(mData.begin(), mData.end(), 0.0f);
		for (mDataPoint = 0; mDataPoint < TOTAL_TIME; ++mDataPoint)
		{
			float input = 0.0f;
			if (mDataPoint >= RELAX_TIME && mDataPoint < RELAX_TIME + duration)
			{
				if (((mDataPoint - RELAX_TIME) % duty) < dutyOn)
				{
					input = weight;
					maxInput = max(input, maxInput);
					minInput = min(input, minInput);
					layer->inject(0, 0, weight);
				}
			}
			mInputSeries->append(qreal(mDataPoint), input);
			layer->tick();
			layer->fireSpikes(&mSynapse, this);
		}

		mSeries->clear();
		for (int tt = 0; tt < TOTAL_TIME; ++tt)
		{
			maxData = max(mData[tt], maxData);
			minData = min(mData[tt], minData);
			mSeries->append(qreal(tt), mData[tt]);
		}
		mChart->addSeries(mSeries);
		mChart->createDefaultAxes();
		mChart->axisY()->setRange(minData, maxData);
		ui.graphicsView->setChart(mChart);

		mInputChart->addSeries(mInputSeries);
		mInputChart->createDefaultAxes();
		mInputChart->axisY()->setRange(minInput, maxInput);
		ui.graphicsInput->setChart(mInputChart);
	}
}
