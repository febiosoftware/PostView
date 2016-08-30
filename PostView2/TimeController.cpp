#include "stdafx.h"
#include "TimeController.h"
#include <QPainter>
#include <QMouseEvent>
#include <math.h>

CTimeController::CTimeController(QWidget* parent) : QWidget(parent)
{
	m_min = m_max = 0.0;
	m_nselect = -1;
	m_first = m_last = 0;
	m_ndrag = -1;
}

void CTimeController::clearData()
{
	m_data.clear();
	update();	
}

void CTimeController::setSelection(int i)
{
	m_nselect = -1;
	if (m_data.empty() == false)
	{
		if ((i >= m_first) && (i<= m_last)) m_nselect = i;
	}
	update();
}

void CTimeController::setRange(int nmin, int nmax)
{
	if (m_data.empty() == false)
	{
		if (nmin > nmax)
		{
			int tmp = nmin;
			nmin = nmax; nmax = tmp;
		}

		if (nmin < 0) nmin = 0;
		if (nmax >= (int) m_data.size()) nmax = (int) m_data.size()-1;

		m_first = nmin;
		m_last  = nmax;
		update();
	}
}

void CTimeController::setTimePoints(const std::vector<double>& time)
{
	// copy data
	m_data = time;

	// clear selection
	m_nselect = -1;
	m_ndrag = -1;

	// update range
	if (m_data.empty() == false)
	{
		m_dataMin = m_dataMax = m_data[0];
		for (int i=1; i<(int)m_data.size(); ++i)
		{
			if (m_data[i] < m_dataMin) m_dataMin = m_data[i];
			if (m_data[i] > m_dataMax) m_dataMax = m_data[i];
		}

		if (m_dataMin == m_dataMax) m_dataMax++;

		m_first = 0;
		m_last = (int) m_data.size() - 1;
	}
	else
	{
		m_dataMin = m_dataMax = 0.0;
	}

	UpdateScale();

	// redraw
	update();
}

void CTimeController::UpdateScale()
{
	// update scale
	double dataRange = m_dataMax - m_dataMin;
	double g = log10(dataRange);
	double d = floor(g) - 1.0;
	m_inc = pow(10, d);

	int W = rect().width();
	int nd = W / (int)(dataRange / m_inc);
	if (nd < 50)
	{
		double m_inc0 = m_inc;
		m_inc = 2*m_inc0;
		nd = W / (int)(dataRange / m_inc);
		if (nd < 50)
		{
			m_inc = 5*m_inc0;
			nd = W / (int)(dataRange / m_inc);
			if (nd < 50) m_inc = 10*m_inc0;
		}
	}

	m_min = floor(m_dataMin / m_inc)*m_inc;
	m_max = ceil(m_dataMax / m_inc)*m_inc;
}

void CTimeController::resizeEvent(QResizeEvent* ev)
{
	UpdateScale();
}

void CTimeController::mousePressEvent(QMouseEvent* ev)
{
	if (m_data.empty() == false)
	{
		// first see if one the range selectors was clicked
		m_ndrag = -1;
		QPoint pt = ev->pos();
		if (m_leftBox.contains(pt))
		{
			m_ndrag = 0;
			ev->accept();
			return;
		}

		if (m_rightBox.contains(pt))
		{
			m_ndrag = 1;
			ev->accept();
			return;
		}

		QRect rt = rect();
		int x0 = rt.left();
		int x1 = rt.right();
		int W = x1 - x0;

		int xp = ev->x();
		for (int i= m_first; i<= m_last; ++i)
		{
			double t = m_data[i];
			int xi = x0 + (int)((t - m_min) / (m_max - m_min) * W);

			int d = abs(xp - xi);
			if (d <= 9)
			{
				emit pointClicked(i);
				ev->accept();
				return;
			}
		}
	}
}

void CTimeController::mouseMoveEvent(QMouseEvent* ev)
{
	if (m_ndrag != -1)
	{
		QRect rt = rect();
		int x0 = rt.left();
		int x1 = rt.right();
		int W = x1 - x0;

		int xp = ev->x();
		int dmin = W*10, imin = -1;
		for (int i = 0; i < (int)m_data.size(); ++i)
		{
			double t = m_data[i];
			int xi = x0 + (int)((t - m_min) / (m_max - m_min) * W);
			int d = abs(xp - xi);
			if (d < dmin)
			{
				dmin = d;
				imin = i;
			}
		}

		if ((m_ndrag == 0) && (imin != m_first))
		{
			m_first = imin;
			if (m_first > m_last)
			{
				m_first = m_last;
				m_last = imin;
				m_ndrag = 1;
			}

			emit rangeChanged(m_first, m_last);

			update();
			ev->accept();
			return;
		}

		if ((m_ndrag == 1) && (imin != m_last))
		{
			m_last = imin;
			if (m_last < m_first)
			{
				m_last = m_first;
				m_first = imin;
				m_ndrag = 0;
			}

			emit rangeChanged(m_first, m_last);

			update();
			ev->accept();
			return;
		}
	}
}

void CTimeController::mouseReleaseEvent(QMouseEvent* ev)
{
	if (m_ndrag != -1)
	{
		m_ndrag = -1;
		update();
	}
}

void drawBox(QPainter& painter, const QRect& rt, const QColor& c)
{
	painter.fillRect(rt, c);
	painter.setPen(Qt::black);
	int x0 = rt.left();
	int x1 = rt.right();
	int y0 = rt.top();
	int y1 = rt.bottom();
	painter.drawLine(x0, y1, x1, y1);
	painter.drawLine(x1, y0, x1, y1);

	painter.setPen(Qt::white);
	painter.drawLine(x0, y0, x1, y0);
	painter.drawLine(x0, y0, x0, y1-1);
}

void CTimeController::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);

	// get the widget's rectangle
	QRect rt = rect();

	// if there is no data, just fill the rect and return
	if (m_data.empty())
	{
		painter.fillRect(rt, Qt::darkGray);
		return;
	}

	// update rectangles
	m_timeRect = rt; m_timeRect.setTop(rt.bottom() - 20);
	m_dataRect = rt; m_dataRect.setBottom(m_timeRect.top() - 1);
	int x0 = rt.left();
	int x1 = rt.right();
	int W = x1 - x0;

	int xf = x0 + (int)((m_data[m_first] - m_min) / (m_max - m_min) * W);
	int xl = x0 + (int)((m_data[m_last ] - m_min) / (m_max - m_min) * W);

	QRect rt1 = m_dataRect; rt1.setRight(xf);
	QRect rt2 = m_dataRect; rt2.setLeft(rt1.right()); rt2.setRight(xl);
	QRect rt3 = m_dataRect; rt3.setLeft(rt2.right());

	m_leftBox  = QRect(xf - 5, rt.top(), 10, 10);
	m_rightBox = QRect(xl - 5, rt.top(), 10, 10);

	if (rt1.width() > 0) painter.fillRect(rt1, QColor::fromRgb(64,64,64));
	if (rt2.width() > 0) painter.fillRect(rt2, Qt::darkGray);
	if (rt3.width() > 0) painter.fillRect(rt3, QColor::fromRgb(64, 64, 64));

	drawBox(painter, m_leftBox , (m_ndrag == 0 ? Qt::white : Qt::lightGray));
	drawBox(painter, m_rightBox, (m_ndrag == 1 ? Qt::white : Qt::lightGray));

	painter.setRenderHint(QPainter::Antialiasing, true);

	// draw the data
	const int R = 9;
	const int R2 = 11;
	int y = (m_dataRect.top() + m_dataRect.bottom()) / 2;
	painter.setPen(Qt::black);
	painter.setBrush(Qt::darkGreen);
	for (int i = 0; i<m_first; ++i)
	{
		double t = m_data[i];
		int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);
		painter.drawEllipse(x - R / 2, y, R, R);
	}
	painter.setBrush(Qt::green);
	for (int i = m_first; i<= m_last; ++i)
	{
		if (i != m_nselect)
		{
			double t = m_data[i];
			int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);
			painter.drawEllipse(x-R/2, y, R, R);
		}
	}
	painter.setBrush(Qt::darkGreen);
	for (int i = m_last+1; i<(int) m_data.size(); ++i)
	{
		double t = m_data[i];
		int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);
		painter.drawEllipse(x - R / 2, y, R, R);
	}

	if (m_nselect != -1)
	{
		painter.setBrush(Qt::white);
		double t = m_data[m_nselect];
		int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);
		painter.drawEllipse(x - R2 / 2, y, R2, R2);
	}

	// draw the time bar
	QFontMetrics FM = painter.fontMetrics();

	int Y0 = m_timeRect.top();
	int Y1 = m_timeRect.bottom();

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.fillRect(m_timeRect, Qt::lightGray);
	painter.setPen(Qt::white);
	painter.drawLine(x0, Y0, x1, Y0);
	painter.setPen(Qt::darkGray);
	painter.drawLine(x0, Y1, x1, Y1);
	painter.setPen(Qt::black);

	double t = m_min;
	while (t <= m_max)
	{
		int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);
		painter.drawLine(x, Y0, x, Y0+5);
		t += m_inc;
	}

	painter.setRenderHint(QPainter::Antialiasing, true);
	t = m_min;
	while (t <= m_max)
	{
		int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);

		QString txt = QString::number(t);
		int w = FM.width(txt);
		painter.drawText(x - w / 2, Y0 + 5 + FM.height(), txt);

		t += m_inc;
	}
}
