#pragma once
#include <QWidget>

class QFormLayout;

namespace Post {
	class CPropertyList;
}

//-----------------------------------------------------------------------------
class CPropertyListForm : public QWidget
{
	Q_OBJECT

public:
	CPropertyListForm(QWidget* parent = 0);

	// set the property list
	void setPropertyList(Post::CPropertyList* pl);

	// update data
	void updateData();

private slots:
	void onDataChanged();

private:
	QFormLayout*			ui;
	Post::CPropertyList*	m_list;
	QList<QWidget*>			m_widget;
};
