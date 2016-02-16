#pragma once
#include <QWidget>
#include <vector>
#include <QtCore/QVariant>
#include <QPushButton>
#include "PropertyList.h"

//-----------------------------------------------------------------------------
class QTableWidget;
class QLabel;

#include <QPainter>
#include <QColorDialog>

//-----------------------------------------------------------------------------
class CColorButton : public QWidget
{
	Q_OBJECT

public:
	CColorButton(QWidget* parent) : QWidget(parent)
	{
		m_col = QColor(Qt::black);
	}

	void paintEvent(QPaintEvent* ev)
	{
		QPainter p(this);
		p.fillRect(rect(), m_col);
	}

	void mouseReleaseEvent(QMouseEvent* ev)
	{
		QColorDialog dlg(this);
		QColor col = dlg.getColor(m_col);
		if (col.isValid())
		{
			m_col = col;
			repaint();
			emit colorChanged(m_col);
		}
	}

	void setColor(const QColor& c) { m_col = c; }

	QColor color() { return m_col; }

signals:
	void colorChanged(QColor col);

private:
	QColor	m_col;
};

//-----------------------------------------------------------------------------

namespace Ui {
	class CPropertyListView;
};

class CPropertyListView : public QWidget
{
	Q_OBJECT

public:
	CPropertyListView(QWidget* parent = 0);

	void Update(CPropertyList* plist);

private slots:
	void on_modelProps_clicked(const QModelIndex& index);
	void onDataChanged();

private:
	Ui::CPropertyListView*	ui;
};
