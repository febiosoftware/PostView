#pragma once
#include "CommandPanel.h"
#include <vector>

//-----------------------------------------------------------------------------
class QTreeWidgetItem;
class CGLObject;

//-----------------------------------------------------------------------------
namespace Ui {
	class CModelViewer;
}

//-----------------------------------------------------------------------------
class CModelViewer : public CCommandPanel
{
	Q_OBJECT

public:
	CModelViewer(CMainWindow* pwnd, QWidget* parent = 0);

public:
	void Update(bool breset);

	// this is called when the view needs to be updated
	void UpdateView();

	void selectObject(CGLObject* po);

protected:
	void DocumentUpdate(CDocument* doc);

private slots:
	void on_modelTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* prev);
	void on_modelTree_itemDoubleClicked(QTreeWidgetItem* item, int column);
	void on_nameEdit_editingFinished();
	void on_deleteButton_clicked();
	void on_props_dataChanged();
	void on_enabled_stateChanged(int nstate);

private:
	Ui::CModelViewer*			ui;
	std::vector<CGLObject*>		m_obj;
};
