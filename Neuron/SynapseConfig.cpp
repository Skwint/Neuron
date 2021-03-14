#include "SynapseConfig.h"

#include <qcombobox.h>
#include <qpushbutton.h>
#include <qspinbox.h>

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
	ui.cmbSynapse->addItem("");
	if (mSynapseDir.exists())
	{
		QStringList filters;
		filters << "*.png";
		mSynapseDir.setNameFilters(filters);
		ui.cmbSynapse->addItems(mSynapseDir.entryList());
		int pos = ui.cmbSynapse->findText(QString::fromStdString(synapses->imageName()));
		if (pos >= 0)
		{
			ui.cmbSynapse->setCurrentIndex(pos);
			synapseChanged(false);
		}
	}
	else
	{
		LOG("No synapses data directory - expected " << mSynapseDir.absolutePath().toStdString());
	}
	ui.cmbSource->addItem("");
	ui.cmbTarget->addItem("");

	ui.cmbDelays->setCurrentText(QString::number(mSynapses->delay()));
	ui.spinWeight->setValue(mSynapses->weight());

	connect(this, &QGroupBox::toggled, this, [this]() { ui.panel->setVisible(isChecked()); });
	connect(ui.cmbSynapse, &QComboBox::currentTextChanged, this, [this]() { synapseChanged(true); });
	connect(ui.cmbSource, &QComboBox::currentTextChanged, this, &SynapseConfig::sourceChanged);
	connect(ui.cmbTarget, &QComboBox::currentTextChanged, this, &SynapseConfig::targetChanged);
	connect(ui.cmbType, &QComboBox::currentTextChanged, this, &SynapseConfig::typeChanged);
	connect(ui.spinWeight, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SynapseConfig::synapseChanged);
	connect(ui.cmbDelays, &QComboBox::currentTextChanged, this, &SynapseConfig::delaysChanged);
	connect(ui.btnDelete, &QPushButton::clicked, this, [this]() { mAutomaton->removeSynapse(mSynapses); });
}

SynapseConfig::~SynapseConfig()
{
}

void SynapseConfig::addLayer(const string & name)
{
	ui.cmbSource->addItem(QString::fromStdString(name));
	if (name == mSynapses->sourceName())
	{
		ui.cmbSource->setCurrentIndex(ui.cmbSource->count() - 1);
	}
	ui.cmbTarget->addItem(QString::fromStdString(name));
	if (name == mSynapses->targetName())
	{
		ui.cmbTarget->setCurrentIndex(ui.cmbTarget->count() - 1);
	}
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

void SynapseConfig::synapseChanged(bool update)
{
	if (!ui.cmbSynapse->currentText().isEmpty())
	{
		LOG("Loading synapse matrix [" << ui.cmbSynapse->currentText().toStdString() << "]");
		float weight = ui.spinWeight->value();
		QImage image(mSynapseDir.absolutePath() + "/" + ui.cmbSynapse->currentText());
		ui.lblSynapse->setPixmap(QPixmap::fromImage(image));
		if (update)
		{
			uint32_t * pixels = reinterpret_cast<uint32_t *>(image.bits());
			mSynapses->loadImage(pixels, image.width(), image.height(), weight, ui.cmbSynapse->currentText().toStdString());
		}
		ui.cmbDelays->setEnabled(true);
		ui.spinWeight->setEnabled(true);
	}
	else
	{
		ui.cmbDelays->setEnabled(false);
		ui.spinWeight->setEnabled(false);
	}
}

void SynapseConfig::delaysChanged()
{
	auto name = ui.cmbDelays->currentText();
	if (name == "None")
		mSynapses->setDelay(SynapseMatrix::DELAY_NONE);
	else if (name == "Linear")
		mSynapses->setDelay(SynapseMatrix::DELAY_LINEAR);
	else if (name == "Grid")
		mSynapses->setDelay(SynapseMatrix::DELAY_GRID);
	else
	{
		LOG("Unrecognized delay type in synapse [" << name.toStdString() << "]");
		ui.cmbDelays->setCurrentIndex(0);
	}
}

void SynapseConfig::typeChanged()
{
	if (ui.cmbType->currentText() == "Shunting")
	{
		mSynapses->setShunt(true);
	}
	else
	{
		mSynapses->setShunt(false);
	}
}

void SynapseConfig::sourceChanged()
{
	auto layer = mAutomaton->findLayer(ui.cmbSource->currentText().toStdString());
	mSynapses->setSource(layer);
}

void SynapseConfig::targetChanged()
{
	auto layer = mAutomaton->findLayer(ui.cmbTarget->currentText().toStdString());
	mSynapses->setTarget(layer);
}
