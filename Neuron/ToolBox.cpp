#include "ToolBox.h"
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qgroupbox.h>

ToolBox::ToolBox(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);

	connect(ui.netGroup, &QGroupBox::toggled, this, &ToolBox::netToggle);
	connect(ui.viewGroup, &QGroupBox::toggled, this, &ToolBox::viewToggle);
	connect(ui.simGroup, &QGroupBox::toggled, this, &ToolBox::simToggle);
	connect(ui.btnNetApply, &QPushButton::clicked, this, &ToolBox::netApply);
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
	emit netBuild(width, height);
}

void ToolBox::viewToggle()
{
	ui.viewPanel->setVisible(ui.viewGroup->isChecked());
}

void ToolBox::simToggle()
{
	ui.simPanel->setVisible(ui.simGroup->isChecked());
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
	else if (!ui.cmbSimSpeed->currentText().compare("10 ms"))
	{
		return 10;
	}
	else if (!ui.cmbSimSpeed->currentText().compare("Fast (no skipping)"))
	{
		return 0;
	}
	else if (!ui.cmbSimSpeed->currentText().compare("Fast (skipping)"))
	{
		return 0;
	}
	return 0;
}

void ToolBox::displayZoom(int zoom)
{
	ui.lblViewZoom->setText(QString::number(zoom * 100) + "%");
}

