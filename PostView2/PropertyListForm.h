#pragma once
#include <QWidget>

class QFormLayout;
class CPropertyList;

//-----------------------------------------------------------------------------
class CPropertyListForm : public QWidget
{
	Q_OBJECT

public:
	CPropertyListForm(QWidget* parent = 0);

	// set the property list
	void setPropertyList(CPropertyList* pl);

	// update data
	void updateData();

private slots:
	void onDataChanged();

private:
	QFormLayout*	ui;
	CPropertyList*	m_list;
	QList<QWidget*>	m_widget;
};
