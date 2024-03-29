#pragma once

#include <QGroupBox>
#include "ui_SynapseConfig.h"
#include <qdir.h>

class Automaton;
class SynapseMatrix;

class SynapseConfig : public QGroupBox
{
	Q_OBJECT

public:
	SynapseConfig(std::shared_ptr<Automaton> automaton, std::shared_ptr<SynapseMatrix> synapses, QWidget *parent = Q_NULLPTR);
	~SynapseConfig();

	void addLayer(const std::string & name);
	void removeLayer(const std::string & name);
	void synapseChanged(bool update);
	void sourceChanged();
	void targetChanged();
	void typeChanged();
	void delaysChanged();
	std::shared_ptr<SynapseMatrix> synapses() { return mSynapses; }

private:
	Ui::SynapseConfig ui;
	std::shared_ptr<Automaton> mAutomaton;
	QDir mSynapseDir;
	std::shared_ptr<SynapseMatrix> mSynapses;
};
