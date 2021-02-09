#include "ToolBox.h"

#include <qcombobox.h>
#include <qdiriterator.h>
#include <qfiledialog.h>
#include <qgroupbox.h>
#include <qimage.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <map>
#include <random>
#include <string>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Constants.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/Log.h"

using namespace std;

static const QString NOISE_DENSE("Noise (dense)");
static const QString NOISE_SPARSE("Noise (sparse)");
static const QString NEURON_FILETYPE("*.neuron");
static const QString SAVE_DIR("Data/Saves/");

ToolBox::ToolBox(shared_ptr<Automaton> automaton, QWidget *parent)
	: QDockWidget(parent),
	mAutomaton(automaton),
	mSpikeDir("Data/Spikes")
{
	ui.setupUi(this);
	ui.cmbType->clear();
	auto names = mAutomaton->typeNames();
	for (auto name : names)
	{
		ui.cmbType->addItem(name.c_str());
	}
	ui.spinSpikeDuration->setMaximum(MAX_SPIKE_LENGTH);
	loadSpikeMaps();

	connect(ui.cmbType, &QComboBox::currentTextChanged, this, &ToolBox::netTypeChanged);
	connect(ui.netGroup, &QGroupBox::toggled, this, &ToolBox::netToggle);
	connect(ui.viewGroup, &QGroupBox::toggled, this, &ToolBox::viewToggle);
	connect(ui.simGroup, &QGroupBox::toggled, this, &ToolBox::simToggle);
	connect(ui.editingGroup, &QGroupBox::toggled, this, &ToolBox::editingToggle);
	connect(ui.cmbSpikeShape, &QComboBox::currentTextChanged, this, &ToolBox::spikeChanged);
	connect(ui.spinSpikeDuration, QOverload<int>::of(&QSpinBox::valueChanged), this, &ToolBox::spikeChanged);
	connect(ui.spinNetWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &ToolBox::netSizeChanged);
	connect(ui.spinNetHeight, QOverload<int>::of(&QSpinBox::valueChanged), this, &ToolBox::netSizeChanged);
	connect(ui.cmbRendering, &QComboBox::currentTextChanged, this, &ToolBox::renderingChanged);
	connect(ui.btnEditingClearLayer, &QPushButton::clicked, this, &ToolBox::editingClearLayer);
	connect(ui.btnEditingClearAll , &QPushButton::clicked, this, &ToolBox::editingClearAll);
	connect(ui.btnEditingSpike, &QPushButton::clicked, this, &ToolBox::editingSpike);
	connect(ui.btnLoad, &QToolButton::clicked, this, &ToolBox::load);
	connect(ui.btnSave, &QToolButton::clicked, this, &ToolBox::save);

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

void ToolBox::load()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Choose a location", SAVE_DIR, NEURON_FILETYPE);
	if (!fileName.isEmpty() && !fileName.isNull())
	{
		mAutomaton->load(fileName.toStdString());
	}
	ui.spinSpikeDuration->setValue(mAutomaton->spike().size());
	ui.cmbSpikeShape->setCurrentIndex(-1);
}

void ToolBox::save()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Choose a location", SAVE_DIR, NEURON_FILETYPE, nullptr);
	if (!fileName.isEmpty() && !fileName.isNull())
	{
		mAutomaton->save(fileName.toStdString());
	}
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

void ToolBox::spikeChanged()
{
	QString spikeShape = ui.cmbSpikeShape->currentText();
	int spikeDuration = ui.spinSpikeDuration->value();
	SpikeProcessor::Spike spike;
	if (!spikeShape.compare("Square") || spikeDuration == 1)
	{
		for (int ll = 0; ll < spikeDuration; ++ll)
		{
			spike.push_back(1.0f);
		}
	}
	else
	{
		float halfDuration = 0.5f * spikeDuration;
		float x = 0.5f - halfDuration;
		if (!spikeShape.compare("Triangle"))
		{
			for (int ll = 0; ll < spikeDuration; ++ll)
			{
				spike.push_back(1.0f - fabs(x) / halfDuration);
				x += 1.0f;
			}
		}
		else
		{
			for (int ll = 0; ll < spikeDuration; ++ll)
			{
				float xnorm = x / halfDuration;
				spike.push_back(exp(-4.0f*xnorm*xnorm));
				x += 1.0f;
			}
		}
	}

	mAutomaton->setSpike(spike);
}

void ToolBox::editingClearLayer()
{	
	auto layer = mAutomaton->findLayer(ui.cmbEditingTarget->currentText().toStdString());
	if (layer)
	{
		layer->clear();
	}
	emit redraw();
}

void ToolBox::editingClearAll()
{
	mAutomaton->clearLayers();
	emit redraw();
}

void ToolBox::editingNoise(int density)
{
	float weight = ui.spinEditingWeight->value();
	auto layer = mAutomaton->findLayer(ui.cmbEditingTarget->currentText().toStdString());
	if (layer)
	{
		static mt19937 rnd;
		for (int row = 0; row < mAutomaton->height(); ++row)
		{
			for (int col = 0; col < mAutomaton->width(); ++col)
			{
				if (rnd() < density)
				{
					layer->fire(col, row, weight, 0);
				}
			}
		}
	}
}

void ToolBox::editingSpike()
{
	auto layer = mAutomaton->findLayer(ui.cmbEditingTarget->currentText().toStdString());
	if (layer)
	{
		QString source = ui.cmbEditingSource->currentText();
		LOG("Spiking layer [" << layer->name() << "] with [" << source.toStdString() << "]");
		if (!source.compare(NOISE_DENSE))
		{
			editingNoise(0x50000000);
		}
		else if (!source.compare(NOISE_SPARSE))
		{
			editingNoise(0x20000000);
		}
		else
		{
			QImage image(mSpikeDir.absolutePath() + "/" + ui.cmbEditingSource->currentText());
			uint32_t * pixels = reinterpret_cast<uint32_t *>(image.bits());
			int width = image.width();
			int height = image.height();
			if (width <= layer->width() || height <= layer->height())
			{
				float weightMultiplier = ui.spinEditingWeight->value();
				float weight;
				int delay;
				int rowOffset = (layer->height() - height) / 2;
				int colOffset = (layer->width() - width) / 2;
				for (int rr = 0; rr < height; ++rr)
				{
					for (int cc = 0; cc < width; ++cc)
					{
						uint32_t pixel = *pixels;
						if (pixel & 0xFFFF)
						{
							weight = weightMultiplier * (float(pixel & 0xFFFF) / 32768.0f - 1.0f);
							delay = (pixel & 0x00FF0000) >> 16;
							layer->fire(cc + colOffset, rr + rowOffset,weight, delay);
						}
						++pixels;
					}
				}
			}
			else
			{
				LOG("  Unable to apply spikes - spike map is larger than target layer");
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

void ToolBox::automatonLayerCreated(shared_ptr<Layer> layer)
{
	ui.cmbEditingTarget->addItem(QString::fromStdString(layer->name()));
	emit redraw();
}

void ToolBox::automatonLayerRemoved(shared_ptr<Layer> layer)
{
	QString str = QString::fromStdString(layer->name());
	int index = ui.cmbEditingTarget->findText(str);
	if (index > -1)
	{
		ui.cmbEditingTarget->removeItem(index);
	}
	emit redraw();
}

void ToolBox::loadSpikeMaps()
{
	ui.cmbEditingSource->clear();

	ui.cmbEditingSource->addItem(NOISE_DENSE);
	ui.cmbEditingSource->addItem(NOISE_SPARSE);
	if (mSpikeDir.exists())
	{
		QStringList filters;
		filters << "*.png";
		mSpikeDir.setNameFilters(filters);
		ui.cmbEditingSource->addItems(mSpikeDir.entryList());
	}
	else
	{
		LOG("No spikes data directory - expected " << mSpikeDir.absolutePath().toStdString());
	}
}
