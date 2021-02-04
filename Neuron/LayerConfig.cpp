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
}

LayerConfig::~LayerConfig()
{
}

void LayerConfig::repopulate()
{
	while (ui.formLayout->rowCount() > mFixedConfigRows)
	{
		ui.formLayout->removeRow(mFixedConfigRows);
	}
	mConfigWidgets.clear();

	mConfig = mLayer->getConfig();

	int row = ui.formLayout->rowCount();
	for (auto & item : mConfig)
	{
		auto label = new QLabel(ui.panel);
		label->setText(item.first.c_str());

		ui.formLayout->setWidget(row, QFormLayout::LabelRole, label);

		auto editor = new QDoubleSpinBox(ui.panel);
		editor->setSingleStep(1.0f);
		editor->setValue(item.second.value);
		mConfigWidgets.push_back(ConfigWidget(item.first, editor));

		ui.formLayout->setWidget(row, QFormLayout::FieldRole, editor);

		++row;
	}

	ui.panel->update();
}

void LayerConfig::apply()
{
	for (int item = 0; item < mConfigWidgets.size(); ++item)
	{
		auto & widget = mConfigWidgets[item];
		QDoubleSpinBox * spin = dynamic_cast<QDoubleSpinBox *>(widget.widget);
		if (spin)
			mConfig[widget.name] = ConfigItem(spin->value());
	}
	mLayer->setConfig(mConfig);
}

void LayerConfig::color()
{
	mLayerData->color = QColorDialog::getColor();
	ui.btnColour->setText(mLayerData->color.name(QColor::HexRgb));
}
