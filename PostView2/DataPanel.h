#pragma once
#include "CommandPanel.h"

class CMainWindow;

namespace Ui {
	class CDataPanel;
}

class CDataPanel : public CCommandPanel
{
	Q_OBJECT

public:
	CDataPanel(CMainWindow* pwnd, QWidget* parent = 0);

	void Update();

private:
	Ui::CDataPanel* ui;
};
