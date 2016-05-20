#pragma once
#include "CommandPanel.h"

class CMainWindow;
class CAbstractTool;

namespace Ui {
	class CToolsPanel;
}

class CToolsPanel : public CCommandPanel
{
	Q_OBJECT

public:
	CToolsPanel(CMainWindow* window, QWidget* parent = 0);

	void Update(bool breset);

private:
	void initTools();

	void hideEvent(QHideEvent* event);
	void showEvent(QShowEvent* event);

private slots:
	void on_buttons_buttonClicked(int id);

private:
	Ui::CToolsPanel*	ui;
	CAbstractTool*		activeTool;
};
