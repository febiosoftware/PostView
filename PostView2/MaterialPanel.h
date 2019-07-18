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

	void Update(bool breset);
	void UpdateStates();

private:
	void SetItemColor(int item, GLColor c);

private slots:
	void on_materialList_currentRowChanged(int nrow);
	void on_showButton_toggled(bool b);
	void on_enableButton_toggled(bool b);
	void on_editName_editingFinished();
	void on_props_dataChanged(int nprop);

private:
	Ui::CMaterialPanel*	ui;
	MaterialProps*	m_pmat;
};
