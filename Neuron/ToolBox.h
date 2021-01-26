#pragma once

#include <QDockWidget>
#include "ui_ToolBox.h"

class ToolBox : public QDockWidget
{
	Q_OBJECT

public:
	ToolBox(QWidget *parent = Q_NULLPTR);
	~ToolBox();

	auto zoomFitToWindow() { return ui.btnViewZoomFitToWindow; }
	auto zoomOneToOne() { return ui.btnViewZoomOneToOne; }
	auto zoomIn() { return ui.btnViewZoomIn; }
	auto zoomOut() { return ui.btnViewZoomOut; }

	void displayZoom(int zoom);

signals:
	void netBuild(int width, int height);

private:
	void netToggle();
	void netApply();

	void viewToggle();
	void simToggle();

private:
	Ui::ToolBox ui;
};
