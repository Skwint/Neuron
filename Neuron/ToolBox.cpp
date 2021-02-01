#include "ToolBox.h"

#include <qcombobox.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qdiriterator.h>
#include <qimage.h>

#include "NeuronSim/Log.h"

ToolBox::ToolBox(std::shared_ptr<LayerFactory> layerFactory, QWidget *parent)
	: QDockWidget(parent),
	mSynapseDir("Data/Synapses")
{
	ui.setupUi(this);
	mFixedConfigRows = ui.netConfigLayout->rowCount();

	mLayerFactory = layerFactory;
	auto names = mLayerFactory->getNames();
	for (auto name : names)
	{
		ui.cmbType->addItem(name.c_str());
	}
	populateSynapses();

	connect(ui.cmbType, &QComboBox::currentTextChanged, this, &ToolBox::netTypeChanged);
	connect(ui.netGroup, &QGroupBox::toggled, this, &ToolBox::netToggle);
	connect(ui.viewGroup, &QGroupBox::toggled, this, &ToolBox::viewToggle);
	connect(ui.simGroup, &QGroupBox::toggled, this, &ToolBox::simToggle);
	connect(ui.synapseGroup, &QGroupBox::toggled, this, &ToolBox::synapseToggle);
	connect(ui.btnNetApply, &QPushButton::clicked, this, &ToolBox::netApply);
	connect(ui.cmbSynapse, &QComboBox::currentTextChanged, this, &ToolBox::synapseChanged);

	ui.cmbType->setCurrentText("Life");
	netTypeChanged();
	ui.cmbSynapse->setCurrentText("Conway.png");
	synapseChanged();
}

ToolBox::~ToolBox()
{
}

void ToolBox::netToggle()
{
	ui.netPanel->setVisible(ui.netGroup->isChecked());
}

void ToolBox::netApply()
{
	int width = ui.spinNetWidth->value();
	int height = ui.spinNetHeight->value();
	std::string type = ui.cmbType->currentText().toStdString();
	for (int item = 0; item < mConfigWidgets.size(); ++item)
	{
		QDoubleSpinBox * spin = dynamic_cast<QDoubleSpinBox *>(mConfigWidgets[item]);
		if (spin)
			mConfig[item].value = spin->value();
	}
	emit netBuild(type, mConfig, width, height);
}

void ToolBox::netTypeChanged()
{
	while (ui.netConfigLayout->rowCount() > mFixedConfigRows)
	{
		ui.netConfigLayout->removeRow(mFixedConfigRows);
	}
	mConfigWidgets.clear();

	auto & config = mLayerFactory->config(ui.cmbType->currentText().toStdString());
	mConfig = config;

	int row = ui.netConfigLayout->rowCount();
	for (auto & item : config)
	{
		auto label = new QLabel(ui.netPanel);
		label->setText(item.name.c_str());

		ui.netConfigLayout->setWidget(row, QFormLayout::LabelRole, label);

		auto editor = new QDoubleSpinBox(ui.netPanel);
		editor->setMinimum(item.minimum);
		editor->setMaximum(item.maximum);
		editor->setSingleStep(0.1f);
		editor->setValue(item.def);
		mConfigWidgets.push_back(editor);

		ui.netConfigLayout->setWidget(row, QFormLayout::FieldRole, editor);

		++row;
	}

	ui.netPanel->update();
}

void ToolBox::viewToggle()
{
	ui.viewPanel->setVisible(ui.viewGroup->isChecked());
}

void ToolBox::simToggle()
{
	ui.simPanel->setVisible(ui.simGroup->isChecked());
}

void ToolBox::synapseToggle()
{
	ui.synapsePanel->setVisible(ui.synapseGroup->isChecked());
}

void ToolBox::synapseChanged()
{
	QImage image(mSynapseDir.absolutePath() + "/" + ui.cmbSynapse->currentText());
	ui.lblSynapse->setPixmap(QPixmap::fromImage(image));
	uint32_t * pixels = reinterpret_cast<uint32_t *>(image.bits());
	mSynapseMatrix.loadImage(pixels, image.width(), image.height());
	emit setSynapses(mSynapseMatrix);
}

int ToolBox::delay()
{
	if (!ui.cmbSimSpeed->currentText().compare("1 second"))
	{
		return 1000;
	}
	else if (!ui.cmbSimSpeed->currentText().compare("100 ms"))
	{
		return 100;
	}
	else if (!ui.cmbSimSpeed->currentText().compare("33 ms"))
	{
		return 33;
	}
	return 0;
}

void ToolBox::displayZoom(int zoom)
{
	ui.lblViewZoom->setText(QString::number(zoom * 100) + "%");
}

void ToolBox::populateSynapses()
{
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
}
