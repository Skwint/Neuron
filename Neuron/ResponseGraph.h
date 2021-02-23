#ifndef RESPONSE_GRAPH_H
#define RESPONSE_GRAPH_H

#include <QWidget>
#include "ui_ResponseGraph.h"
#include "NeuronSim/Spiker.h"
#include "NeuronSim/SynapseMatrix.h"

#include <memory>

class Automaton;
class Layer;

class ResponseGraph : public QDialog, public Spiker, public SynapseMatrix::Listener
{
	Q_OBJECT

public:
	ResponseGraph(std::shared_ptr<Automaton> automaton, std::shared_ptr<Layer> layer, QWidget *parent = Q_NULLPTR);
	~ResponseGraph();

	void plot();

private: // from Spiker
	void fire(const Spike & spike, int index, float weight, int delay) override;

private: // from SynapseMatrix::Listener
	void synapseMatrixChanged(SynapseMatrix * matrix) override;

private:
	Ui::ResponseGraph ui;
	std::shared_ptr<Automaton> mAutomaton;
	std::weak_ptr<Layer> mLayer;
	std::vector<float> mData;
	SynapseMatrix mSynapse;
	int mDataPoint;
	QLineSeries * mSeries;
	QChart * mChart;
};

#endif
