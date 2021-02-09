#include "LayerConfig.h"

#include <qcolordialog.h>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Layer.h"

using namespace std;

LayerConfig::LayerConfig(std::shared_ptr<Layer> layer, QWidget *parent)
	: QGroupBox(parent),
	mLayer(layer),
	mLayerData(make_unique<LayerData>())
{
	ui.setupUi(this);
	mLayerData->color = QColor(0xFFFFFFFF);
	layer->setUserData(mLayerData.get());
	mFixedConfigRows = ui.formLayout->rowCount();

	repopulate();
	setTitle(QString::fromStdString(layer->name()));

	connect(this, &QGroupBox::toggled, this, [this]() { ui.panel->setVisible(isChecked()); });
	connect(ui.btnApply, &QPushButton::clicked, this, &LayerConfig::apply);
	connect(ui.btnColour, &QPushButton::clicked, this, &LayerConfig::color);
	connect(ui.cmbPreset, &QComboBox::currentTextChanged, this, &LayerConfig::presetSelected);
}

LayerConfig::~LayerConfig()
{
}

void LayerConfig::repopulate()
{
	ui.cmbPreset->clear();
	auto presets = mLayer->getPresets();
	for (auto & config : presets.configs())
	{
		ui.cmbPreset->addItem(QString::fromStdString(config.first));
	}

	while (ui.formLayout->rowCount() > mFixedConfigRows)
	{
		ui.formLayout->removeRow(mFixedConfigRows);
	}
	mConfigWidgets.clear();

	mConfig = presets.configs().begin()->second;

	int row = ui.formLayout->rowCount();
	for (auto & item : mConfig.items())
	{
		auto label = new QLabel(ui.panel);
		label->setText(item.first.c_str());

		ui.formLayout->setWidget(row, QFormLayout::LabelRole, label);

		switch (item.second.mType)
		{
		case ConfigItem::FLOAT:
		{
			auto editor = new QDoubleSpinBox(ui.panel);
			editor->setSingleStep(1.0f);
			editor->setValue(item.second.mFloat);
			mConfigWidgets.push_back(ConfigWidget(item.first, editor));
			ui.formLayout->setWidget(row, QFormLayout::FieldRole, editor);
			break;
		}
		case ConfigItem::INT:
		{
			auto editor = new QSpinBox(ui.panel);
			editor->setSingleStep(1.0f);
			editor->setValue(item.second.mInt);
			mConfigWidgets.push_back(ConfigWidget(item.first, editor));
			ui.formLayout->setWidget(row, QFormLayout::FieldRole, editor);
			break;
		}
		default:
			// We intentionally ignore this error - we should probably
			// report it instead, but an exception would be excessive.
			// TODO
			break;
		}

		++row;
	}

	ui.cmbPreset->setCurrentIndex(0);
	ui.panel->update();
}

void LayerConfig::apply()
{
	ConfigItem config;
	for (int item = 0; item < mConfigWidgets.size(); ++item)
	{
		auto & widget = mConfigWidgets[item];
		auto config = mConfig[widget.name];
		switch (config.mType)
		{
		case ConfigItem::FLOAT:
		{
			QDoubleSpinBox * spin = dynamic_cast<QDoubleSpinBox *>(widget.widget);
			if (spin)
				mConfig[widget.name] = float(spin->value());
			break;
		}
		case ConfigItem::INT:
		{
			QSpinBox * spin = dynamic_cast<QSpinBox *>(widget.widget);
			if (spin)
				mConfig[widget.name] = spin->value();
			break;
		}
		default:
			break;
		}
	}
	mLayer->setConfig(mConfig);
}

void LayerConfig::color()
{
	mLayerData->color = QColorDialog::getColor();
	ui.btnColour->setText(mLayerData->color.name(QColor::HexRgb));
}

void LayerConfig::presetSelected()
{
	auto & presets = mLayer->getPresets();
	auto & config = presets[ui.cmbPreset->currentText().toStdString()];
	for (auto & widget : mConfigWidgets)
	{
		switch (config[widget.name].mType)
		{
		case ConfigItem::FLOAT:
			dynamic_cast<QDoubleSpinBox *>(widget.widget)->setValue(config[widget.name].mFloat);
			break;
		case ConfigItem::INT:
			dynamic_cast<QSpinBox *>(widget.widget)->setValue(config[widget.name].mInt);
			break;
		}
	}
}
