#include "stdafx.h"
#include "TimeController.h"
#include <QPainter>
#include <QMouseEvent>

CTimeController::CTimeController(QWidget* parent) : QWidget(parent)
{
	m_min = m_max = 0.0;
	m_nselect = -1;
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
		if ((i >= 0) && (i<(int)m_data.size())) m_nselect = i;
	}
	update();
}

void CTimeController::setTimePoints(const std::vector<double>& time)
{
	// copy data
	m_data = time;

	// clear selection
	m_nselect = -1;

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

	int ndiv = (int) dataRange / m_inc;

	int W = rect().width(); 
	int nd = W / ndiv;
	if (nd < 20) m_inc *= 10;

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
		QRect rt = rect();
		int x0 = rt.left();
		int x1 = rt.right();
		int W = x1 - x0;

		int xp = ev->x();
		for (int i=0; i<(int)m_data.size(); ++i)
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

void CTimeController::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

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

	// draw the data
	painter.fillRect(m_dataRect, Qt::darkGray);
	painter.setBrush(Qt::green);
	const int R = 9;
	const int R2 = 11;
	int y = (m_dataRect.top() + m_dataRect.bottom()) / 2;
	for (int i = 0; i<(int)m_data.size(); ++i)
	{
		if (i != m_nselect)
		{
			double t = m_data[i];
			int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);
			painter.drawEllipse(x-R/2, y-R/2, R, R);
		}
	}
	if (m_nselect != -1)
	{
		painter.setBrush(Qt::white);
		double t = m_data[m_nselect];
		int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);
		painter.drawEllipse(x - R2 / 2, y - R2 / 2, R2, R2);
	}

	// draw the time bar
	QFontMetrics FM = painter.fontMetrics();

	int Y0 = m_timeRect.top();
	int Y1 = m_timeRect.bottom();
	painter.fillRect(m_timeRect, Qt::lightGray);
	double t = m_min;
	while (t <= m_max)
	{
		int x = x0 + (int)((t - m_min) / (m_max - m_min) * W);
		painter.drawLine(x, Y0, x, Y0+5);

		QString txt = QString::number(t);
		int w = FM.width(txt);
		painter.drawText(x - w/2, Y0+5 + FM.height(), txt);

		t += m_inc;
	}
}
