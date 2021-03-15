#include "LayerConfig.h"

#include <qcolordialog.h>
#include <qspinbox.h>
#include <qmessagebox.h>
#include <qcombobox.h>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Constants.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/Log.h"
#include "ResponseGraph.h"

using namespace std;

LayerConfig::LayerConfig(std::shared_ptr<Automaton> automaton, std::shared_ptr<Layer> layer, QWidget *parent)
	: QGroupBox(parent),
	mAutomaton(automaton),
	mLayerName(layer->name()),
	mLoadingPreset(false)
{
	ui.setupUi(this);

	loadPresets();
	repopulate();
	setTitle(QString::fromStdString(mLayerName));
	mGraph = make_unique<ResponseGraph>(mAutomaton, layer, this);
	mGraph->hide();

	connect(this, &QGroupBox::toggled, this, [this]() { ui.panel->setVisible(isChecked()); });
	connect(ui.cmbPreset, &QComboBox::currentTextChanged, this, &LayerConfig::presetSelected);
	connect(ui.btnDelete, &QPushButton::clicked, this, [this]() { mAutomaton->removeLayer(mLayerName); });
	connect(ui.cmbSpikeShape, &QComboBox::currentTextChanged, this, &LayerConfig::spikeChanged);
	connect(ui.spinSpikeDuration, QOverload<int>::of(&QSpinBox::valueChanged), this, &LayerConfig::spikeChanged);
	connect(ui.btnGraph, &QPushButton::clicked, this, &LayerConfig::showGraph);
}

LayerConfig::~LayerConfig()
{
}

void LayerConfig::loadPresets()
{
	auto layer = mAutomaton->findLayer(mLayerName);
	if (layer)
	{
		auto presets = layer->getPresets();
		for (auto & config : presets.configs())
		{
			ui.cmbPreset->addItem(QString::fromStdString(config.first));
		}
		mConfig = layer->getConfig();
	}
}

void LayerConfig::repopulate()
{
	while (ui.configLayout->rowCount())
	{
		ui.configLayout->removeRow(0);
	}
	mConfigWidgets.clear();

	int row = 0;
	for (auto & item : mConfig.items())
	{
		auto label = new QLabel(ui.panel);
		label->setText(item.first.c_str());

		ui.configLayout->setWidget(row, QFormLayout::LabelRole, label);

		switch (item.second.mType)
		{
		case ConfigItem::FLOAT:
		{
			auto editor = new QDoubleSpinBox(ui.panel);
			editor->setSingleStep(1.0f);
			editor->setMinimum(-1000.0f);
			editor->setMaximum(1000.0f);
			editor->setDecimals(4);
			editor->setValue(item.second.mFloat);
			mConfigWidgets.push_back(ConfigWidget(item.first, editor));
			ui.configLayout->setWidget(row, QFormLayout::FieldRole, editor);
			connect(editor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LayerConfig::configItemChanged);
			break;
		}
		case ConfigItem::INT:
		{
			auto editor = new QSpinBox(ui.panel);
			editor->setSingleStep(1.0f);
			editor->setValue(item.second.mInt);
			mConfigWidgets.push_back(ConfigWidget(item.first, editor));
			ui.configLayout->setWidget(row, QFormLayout::FieldRole, editor);
			connect(editor, QOverload<int>::of(&QSpinBox::valueChanged), this, &LayerConfig::configItemChanged);
			break;
		}
		case ConfigItem::ENUM:
		{
			auto editor = new QComboBox(ui.panel);
			for (int index = 0; index < item.second.mEnumNames.size(); ++index)
			{
				editor->insertItem(index, QString::fromStdString(item.second.mEnumNames[index]));
			}
			editor->setCurrentIndex(item.second.mInt);
			mConfigWidgets.push_back(ConfigWidget(item.first, editor));
			ui.configLayout->setWidget(row, QFormLayout::FieldRole, editor);
			connect(editor, &QComboBox::currentTextChanged, this, &LayerConfig::configItemChanged);
			break;
		}
		default:
			QMessageBox::warning(0, "Malformed configuration data", QString::number(item.second.mType));
			LOG("Unexpected config type [" << item.second.mType << "] in " << mConfig.name());
			break;
		}

		++row;
	}

	auto layer = mAutomaton->findLayer(mLayerName);
	if (layer)
	{
		ui.cmbSpikeShape->setCurrentIndex(int(layer->spikeShape()));
		ui.spinSpikeDuration->setValue(layer->spikeDuration());
	}

	ui.cmbPreset->setCurrentIndex(0);
	ui.panel->update();
}

void LayerConfig::configItemChanged()
{
	auto layer = mAutomaton->findLayer(mLayerName);
	if (layer && !mLoadingPreset)
	{
		ConfigItem config;
		for (int item = 0; item < mConfigWidgets.size(); ++item)
		{
			auto & widget = mConfigWidgets[item];
			auto & config = mConfig[widget.name];
			switch (config.mType)
			{
			case ConfigItem::FLOAT:
			{
				QDoubleSpinBox * spin = dynamic_cast<QDoubleSpinBox *>(widget.widget);
				if (spin)
					config.mFloat = float(spin->value());
				break;
			}
			case ConfigItem::INT:
			{
				QSpinBox * spin = dynamic_cast<QSpinBox *>(widget.widget);
				if (spin)
					config.mInt = spin->value();
				break;
			}
			case ConfigItem::ENUM:
			{
				QComboBox * box = dynamic_cast<QComboBox *>(widget.widget);
				if (box)
					config.mInt = box->currentIndex();
				break;
			}
			default:
				// We ignore this. It's an error, but it has already been reported when
				// the config was loaded - we don't need to report it again.
				break;
			}
		}
		layer->setConfig(mConfig);
		ui.cmbPreset->setCurrentIndex(0);
	}
}

void LayerConfig::presetSelected()
{
	mLoadingPreset = true;
	auto layer = mAutomaton->findLayer(mLayerName);
	if (layer)
	{
		auto & presets = layer->getPresets();
		std::string name = ui.cmbPreset->currentText().toStdString();
		if (presets.contains(name))
		{
			auto & config = presets[name];
			for (auto & widget : mConfigWidgets)
			{
				switch (config[widget.name].mType)
				{
				case ConfigItem::FLOAT:
				{
					auto spinner = dynamic_cast<QDoubleSpinBox *>(widget.widget);
					if (spinner)
						spinner->setValue(config[widget.name].mFloat);
					break;
				}
				case ConfigItem::INT:
				{
					auto spinner = dynamic_cast<QSpinBox *>(widget.widget);
					if (spinner)
						spinner->setValue(config[widget.name].mInt);
					break;
				}
				case ConfigItem::ENUM:
				{
					QComboBox * box = dynamic_cast<QComboBox *>(widget.widget);
					if (box)
						box->setCurrentIndex(config[widget.name].mInt);
					break;
				}
				}
			}
			mConfig = config;
		}
		mLoadingPreset = false;
		layer->setConfig(mConfig);
	}
}

void LayerConfig::spikeChanged()
{
	auto layer = mAutomaton->findLayer(mLayerName);
	if (layer)
	{
		Spike::Shape shape = Spike::Shape(ui.cmbSpikeShape->currentIndex());
		int spikeDuration = ui.spinSpikeDuration->value();
		layer->setSpike(shape, spikeDuration);
	}
}

// Shows or updates a graph showing the impulse response of the currently configured neuron
void LayerConfig::showGraph()
{
	mGraph->show();
	mGraph->plot();
}
