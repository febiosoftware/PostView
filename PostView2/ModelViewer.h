#pragma once
#include "CommandPanel.h"

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
	void Update();

	void selectObject(CGLObject* po);

private slots:
	void on_modelTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* prev);

private:
	Ui::CModelViewer*	ui;
};
