#pragma once
#include <QWidget>
#include <vector>
#include <QtCore/QVariant>
#include <QPushButton>
#include "PostLib/PropertyList.h"
#include "CIntInput.h"
#include "CColorButton.h"

//-----------------------------------------------------------------------------
class QTableWidget;
class QLabel;

#include <QPainter>
#include <QColorDialog>

//-----------------------------------------------------------------------------

namespace Ui {
	class CPropertyListView;
};

class CPropertyListView : public QWidget
{
	Q_OBJECT

public:
	CPropertyListView(QWidget* parent = 0);

	void Update(Post::CPropertyList* plist);

	void resizeEvent(QResizeEvent* ev) override;

signals:
	void dataChanged(int);

private slots:
	void on_modelProps_clicked(const QModelIndex& index);
	void onDataChanged();

private:
	Ui::CPropertyListView*	ui;
};
