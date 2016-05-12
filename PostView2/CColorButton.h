#pragma once
#include <QWidget>
#include <QPainter>
#include <QColorDialog>

//-----------------------------------------------------------------------------
class CColorButton : public QWidget
{
	Q_OBJECT

public:
	CColorButton(QWidget* parent = 0);

	void paintEvent(QPaintEvent* ev);

	void mouseReleaseEvent(QMouseEvent* ev);

	void setColor(const QColor& c) { m_col = c; }

	QColor color() const { return m_col; }

	QSize sizeHint() const { return QSize(23, 23); }



signals:
	void colorChanged(QColor col);

private:
	QColor	m_col;
};
