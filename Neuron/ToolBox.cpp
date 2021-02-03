#include "ToolBox.h"

#include <qcombobox.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qdiriterator.h>
#include <qimage.h>
#include <map>
#include <string>

#include "NeuronSim/Automaton.h"
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
	connect(ui.btnNetApply, &QPushButton::clicked, this, &ToolBox::netApply);
	connect(ui.cmbSpike, &QComboBox::currentTextChanged, this, &ToolBox::spikeChanged);

	ui.cmbType->setCurrentText("Life");
	netTypeChanged();
	ui.cmbSpike->setCurrentText(QString::fromStdString(mSpikes.begin()->first));
	spikeChanged();
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
	mAutomaton->setSize(width, height);
}

void ToolBox::netTypeChanged()
{
	mAutomaton->setNetworkType(ui.cmbType->currentText().toStdString());
}

void ToolBox::viewToggle()
{
	ui.viewPanel->setVisible(ui.viewGroup->isChecked());
}

void ToolBox::simToggle()
{
	ui.simPanel->setVisible(ui.simGroup->isChecked());
}

const SpikeProcessor::Spike & ToolBox::spike()
{
	std::string str = ui.cmbSpike->currentText().toStdString();
	return mSpikes[str];
}

void ToolBox::spikeChanged()
{
	emit setSpike(spike());
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
