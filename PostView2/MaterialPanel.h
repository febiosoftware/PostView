#pragma once
#include "CommandPanel.h"

namespace Ui {
	class CMaterialPanel;
}

class MaterialProps;
class QListWidgetItem;

class CMaterialPanel : public CCommandPanel
{
	Q_OBJECT

public:
	CMaterialPanel(CMainWindow* wnd, QWidget* parent = 0);

	void Update();

private slots:
	void on_materialList_currentRowChanged(int nrow);

private:
	Ui::CMaterialPanel*	ui;
	MaterialProps*	m_pmat;
};
