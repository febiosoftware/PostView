#pragma once
#include <QWidget>
#include <vector>

class CTimeController : public QWidget
{
	Q_OBJECT
public:
	CTimeController(QWidget* parent = 0);

	void clearData();

	// set the time points
	void setTimePoints(const std::vector<double>& time);

	void UpdateScale();

	void setSelection(int i);
	void setRange(int nmin, int nmax);

protected:
	void paintEvent(QPaintEvent* ev);
	void resizeEvent(QResizeEvent* ev);
	void mousePressEvent(QMouseEvent* ev);
	void mouseMoveEvent(QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);

signals:
	void pointClicked(int n);
	void rangeChanged(int nmin, int nmax);

private:
	std::vector<double>	m_data;
	double	m_dataMin, m_dataMax;
	double	m_min, m_max, m_inc;
	int	    m_first, m_last;
	QRect	m_dataRect, m_timeRect;
	QRect	m_leftBox, m_rightBox;
	int		m_nselect;
	int		m_ndrag;
};
