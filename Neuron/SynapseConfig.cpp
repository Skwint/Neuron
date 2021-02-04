#include "SynapseConfig.h"

#include <qcombobox.h>
#include <qpushbutton.h>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/Log.h"
#include "NeuronSim/SynapseMatrix.h"

using namespace std;

SynapseConfig::SynapseConfig(shared_ptr<Automaton> automaton, shared_ptr<SynapseMatrix> synapses, QWidget *parent)
	: QGroupBox(parent),
	mAutomaton(automaton),
	mSynapseDir("Data/Synapses"),
	mSynapses(synapses)
{
	ui.setupUi(this);

	ui.cmbSynapse->clear();

	if (mSynapseDir.exists())
	{
		QStringList filters;
		filters << "*.png";
		mSynapseDir.setNameFilters(filters);
		ui.cmbSynapse->addItems(mSynapseDir.entryList());
	}
	else
	{
		LOG("No synapses data directory - expected " << mSynapseDir.absolutePath().toStdString());
	}

	ui.cmbSource->setCurrentText(QString::fromStdString(synapses->source()->name()));
	ui.cmbTarget->setCurrentText(QString::fromStdString(synapses->target()->name()));
	synapseChanged();

	connect(this, &QGroupBox::toggled, this, [this]() { ui.panel->setVisible(isChecked()); });
	connect(ui.cmbSynapse, &QComboBox::currentTextChanged, this, &SynapseConfig::synapseChanged);
	connect(ui.cmbSource, &QComboBox::currentTextChanged, this, &SynapseConfig::sourceChanged);
	connect(ui.cmbTarget, &QComboBox::currentTextChanged, this, &SynapseConfig::targetChanged);
	connect(ui.btnDelete, &QPushButton::clicked, this, [this]() { mAutomaton->removeSynapse(mSynapses); });
}

SynapseConfig::~SynapseConfig()
{
}

void SynapseConfig::addLayer(const string & name)
{
	ui.cmbSource->addItem(QString::fromStdString(name));
	ui.cmbTarget->addItem(QString::fromStdString(name));
}

void SynapseConfig::removeLayer(const string & name)
{
	QString str = QString::fromStdString(name);
	int index = ui.cmbSource->findText(str);
	if (index > -1)
	{
		ui.cmbSource->removeItem(index);
	}
	index = ui.cmbTarget->findText(str);
	if (index > -1)
	{
		ui.cmbTarget->removeItem(index);
	}
}

bool SynapseConfig::isReferencingLayer(const std::string & name)
{
	return (ui.cmbSource->currentText().toStdString() == name ||
		ui.cmbTarget->currentText().toStdString() == name);
}

void SynapseConfig::synapseChanged()
{
	LOG("Loading synapse matrix [" << ui.cmbSynapse->currentText().toStdString() << "]");
	QImage image(mSynapseDir.absolutePath() + "/" + ui.cmbSynapse->currentText());
	ui.lblSynapse->setPixmap(QPixmap::fromImage(image));
	uint32_t * pixels = reinterpret_cast<uint32_t *>(image.bits());
	mSynapses->loadImage(pixels, image.width(), image.height());
}

void SynapseConfig::sourceChanged()
{
	auto layer = mAutomaton->findLayer(ui.cmbSource->currentText().toStdString());
	mSynapses->setSource(layer);
}

void SynapseConfig::targetChanged()
{
	auto layer = mAutomaton->findLayer(ui.cmbSource->currentText().toStdString());
	mSynapses->setTarget(layer);
}
