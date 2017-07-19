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

private slots:
	void on_materialList_currentRowChanged(int nrow);
	void on_hideButton_clicked();
	void on_showButton_clicked();
	void on_enableBox_stateChanged(int);
	void on_editName_editingFinished();
	void on_props_dataChanged();

private:
	Ui::CMaterialPanel*	ui;
	MaterialProps*	m_pmat;
};
