#include "ToolBox.h"

#include <qcombobox.h>
#include <qdiriterator.h>
#include <qgroupbox.h>
#include <qimage.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <map>
#include <random>
#include <string>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/Log.h"

ToolBox::ToolBox(std::shared_ptr<Automaton> automaton, QWidget *parent)
	: QDockWidget(parent),
	mAutomaton(automaton)
{
	ui.setupUi(this);
	ui.cmbType->clear();
	auto names = mAutomaton->typeNames();
	for (auto name : names)
	{
		ui.cmbType->addItem(name.c_str());
	}
	populateSpikes();

	connect(ui.cmbType, &QComboBox::currentTextChanged, this, &ToolBox::netTypeChanged);
	connect(ui.netGroup, &QGroupBox::toggled, this, &ToolBox::netToggle);
	connect(ui.viewGroup, &QGroupBox::toggled, this, &ToolBox::viewToggle);
	connect(ui.simGroup, &QGroupBox::toggled, this, &ToolBox::simToggle);
	connect(ui.editingGroup, &QGroupBox::toggled, this, &ToolBox::editingToggle);
	connect(ui.cmbSpike, &QComboBox::currentTextChanged, this, &ToolBox::spikeChanged);
	connect(ui.spinNetWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &ToolBox::netSizeChanged);
	connect(ui.spinNetHeight, QOverload<int>::of(&QSpinBox::valueChanged), this, &ToolBox::netSizeChanged);
	connect(ui.cmbRendering, &QComboBox::currentTextChanged, this, &ToolBox::renderingChanged);
	connect(ui.btnEditingClear, &QPushButton::clicked, this, &ToolBox::editingClear);
	connect(ui.btnEditingNoise, &QPushButton::clicked, this, &ToolBox::editingNoise);

	ui.cmbSpike->setCurrentText(QString::fromStdString(mSpikes.begin()->first));
	spikeChanged();
	renderingChanged();

	automatonTypeChanged();
	automatonSizeChanged(mAutomaton->width(), mAutomaton->height());
	mAutomaton->addListener(this);
}

ToolBox::~ToolBox()
{
	mAutomaton->removeListener(this);
}

void ToolBox::netToggle()
{
	ui.netPanel->setVisible(ui.netGroup->isChecked());
}

void ToolBox::viewToggle()
{
	ui.viewPanel->setVisible(ui.viewGroup->isChecked());
}

void ToolBox::simToggle()
{
	ui.simPanel->setVisible(ui.simGroup->isChecked());
}

void ToolBox::editingToggle()
{
	ui.editingPanel->setVisible(ui.editingGroup->isChecked());
}

void ToolBox::netSizeChanged()
{
	int width = ui.spinNetWidth->value();
	int height = ui.spinNetHeight->value();
	mAutomaton->setSize(width, height);
}

void ToolBox::netTypeChanged()
{
	mAutomaton->setNetworkType(ui.cmbType->currentText().toStdString());
}

const SpikeProcessor::Spike & ToolBox::spike()
{
	std::string str = ui.cmbSpike->currentText().toStdString();
	return mSpikes[str];
}

void ToolBox::spikeChanged()
{
	mAutomaton->setSpike(spike());
}

void ToolBox::editingClear()
{
	mAutomaton->clearLayers();
}

void ToolBox::editingNoise()
{
	float weight = ui.spinEditingWeight->value();
	auto layer = mAutomaton->findLayer(ui.cmbEditingTarget->currentText().toStdString());
	static std::mt19937 rnd;
	for (int row = 0; row < mAutomaton->height(); ++row)
	{
		for (int col = 0; col < mAutomaton->width(); ++col)
		{
			if (rnd() > 0xA0000000)
			{
				layer->fire(row, col, weight);
			}
		}
	}
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

void ToolBox::renderingChanged()
{
	mRendering = static_cast<Rendering>(ui.cmbRendering->currentIndex());
}

void ToolBox::displayZoom(int zoom)
{
	ui.lblViewZoom->setText(QString::number(zoom * 100) + "%");
}

void ToolBox::displayFrameTime(qint64 frameTime)
{
	ui.lblFrameTime->setText(QString::number(frameTime) + " ms");
}

void ToolBox::populateSpikes()
{
	ui.cmbSpike->clear();

	mSpikes["1 Flat"] = { 1.0f };
	mSpikes["3 Flat"] = { 1.0f, 1.0f, 1.0f };
	mSpikes["3 Triangle"] = { 0.5f, 1.0f, 0.5f };
	mSpikes["5 Trapezoid"] = { 0.5f, 1.0f, 1.0f, 1.0f, 0.5f };;
	mSpikes["7 Smooth"] = { 0.1f, 0.3f, 0.7f, 1.0f, 0.7f, 0.3f, 0.1f };

	for (auto & iter : mSpikes)
	{
		ui.cmbSpike->addItem(QString::fromStdString(iter.first));
	}
}

void ToolBox::automatonTypeChanged()
{
	ui.cmbType->blockSignals(true);
	ui.cmbType->setCurrentText(QString::fromStdString(mAutomaton->networkType()));
	ui.cmbType->blockSignals(false);
}

void ToolBox::automatonSizeChanged(int width, int height)
{
	ui.spinNetWidth->blockSignals(true);
	ui.spinNetHeight->blockSignals(true);
	ui.spinNetWidth->setValue(width);
	ui.spinNetHeight->setValue(height);
	ui.spinNetWidth->blockSignals(false);
	ui.spinNetHeight->blockSignals(false);
}

void ToolBox::automatonLayerCreated(std::shared_ptr<Layer> layer)
{
	ui.cmbEditingTarget->addItem(QString::fromStdString(layer->name()));
}

void ToolBox::automatonLayerRemoved(std::shared_ptr<Layer> layer)
{
	QString str = QString::fromStdString(layer->name());
	int index = ui.cmbEditingTarget->findText(str);
	if (index > -1)
	{
		ui.cmbEditingTarget->removeItem(index);
	}
}
