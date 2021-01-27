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
	auto speed() { return ui.cmbSimSpeed; }
	auto play() { return ui.btnSimPlay; }
	auto pause() { return ui.btnSimPause; }
	auto step() { return ui.btnSimStep; }
	auto style() { return ui.cmbViewStyle; }

	void displayZoom(int zoom);
	bool showAllFrames() { return ui.chkShowAllFrames->isChecked(); }
	int delay();

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
