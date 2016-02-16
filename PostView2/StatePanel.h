#pragma once
#include "CommandPanel.h"

namespace Ui{
	class CStatePanel;
}

class CMainWindow;

class CStatePanel : public CCommandPanel
{
	Q_OBJECT

public:
	CStatePanel(CMainWindow* pwnd, QWidget* parent = 0);

	void Update();

private slots:
	void on_stateList_doubleClicked(const QModelIndex& index);

private:
	Ui::CStatePanel* ui;
};
