#include "SynapseConfig.h"

#include <qcombobox.h>

#include "NeuronSim/Log.h"
#include "NeuronSim/SynapseMatrix.h"

using namespace std;

SynapseConfig::SynapseConfig(shared_ptr<SynapseMatrix> synapses, QWidget *parent)
	: QGroupBox(parent),
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

	connect(this, &QGroupBox::toggled, this, [this]() { ui.panel->setVisible(isChecked()); });
	connect(ui.cmbSynapse, &QComboBox::currentTextChanged, this, &SynapseConfig::synapseChanged);
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

bool SynapseConfig::referencesLayer(const std::string & name)
{
	return (ui.cmbSource->currentText().toStdString() == name ||
		ui.cmbTarget->currentText().toStdString() == name);
}

void SynapseConfig::synapseChanged()
{
	QImage image(mSynapseDir.absolutePath() + "/" + ui.cmbSynapse->currentText());
	ui.lblSynapse->setPixmap(QPixmap::fromImage(image));
	uint32_t * pixels = reinterpret_cast<uint32_t *>(image.bits());
	SynapseMatrix synapses;
	synapses.loadImage(pixels, image.width(), image.height());
	//emit setSynapses(synapses);
}
